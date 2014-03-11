// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//-----------------------------------------------------------------------------
// Copyright (c) 2014 Marcus Geelnard
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//     1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software. If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//     2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//     3. This notice may not be removed or altered from any source
//     distribution.
//-----------------------------------------------------------------------------
// 4-bit DDPCM:
// - 32 samples per block
// - 18 bytes / block (28.1% of original size)
// - 64 quantization maps (16 entries per map)
// - Block format (bit layout):
//   [ssss|ssss][ssss|smmm][mmmp|D1..][D2..|D3..][D4..|D5..] ... [D30.|D31.]
//    s: Starting point (13 bits)
//    m: Map-selection (6 bits)
//    p: Predictor-selection (1 bit)
//   Dx: Delta samples (4 bits / delta)
//-----------------------------------------------------------------------------
// This encoder is fairly generic, and not very optimized. There are several
// possible optimizations, such as parallelization and pre-generating look up
// tables (requires more memory).
//-----------------------------------------------------------------------------

#include "encode_dd4a.h"

#include <algorithm>

#include "packed_data.h"
#include "util.h"

// If defined, use a slower but more accurate encoding algorithm.
//#define LIBSAC_ACCURATE_DD4A_ENCODE

namespace sac {

namespace dd4a {

// Defined in quant_lut_dd4a.cpp
extern const short kQuantLut[64][16];

namespace {

int block_size_in_bytes(const int num_samples) {
  return 2 + (num_samples + 1) / 2;
}

const int kBlockSize = 32;
const int kBytesPerBlock = block_size_in_bytes(kBlockSize);
const int kNumMaps = 64;
const int kEntiresPerMap = 16;

int get_max_delta(const short *map) {
  return map[kEntiresPerMap / 2 - 1];
}

int get_min_delta(const short *map) {
  return map[kEntiresPerMap - 1];
}

/// @brief Reverse map a single delta value.
/// @param map The quantization map to use.
/// @param delta The delta value to quantize.
/// @returns The quantized delta value (i.e. LUT index).
/// @note This routine uses binary search in the decoding table, which is
/// memory efficient but not very fast. A reverse LUT could be used
/// instead, but it would require more RAM.
uint8_t encode(const short *map, const int delta) {
  if (delta >= get_max_delta(map))
    return kEntiresPerMap / 2 - 1;
  else if (delta <= get_min_delta(map))
    return kEntiresPerMap - 1;

  // Binary search...
  int abs_delta = std::abs(delta);
  uint8_t c1 = 0, c2 = kEntiresPerMap / 2 - 1;
  while (c2 > (c1 + 1)) {
    uint8_t mid = (c1 + c2) >> 1;
    if (abs_delta >= map[mid])
      c1 = mid;
    else
      c2 = mid;
  }

  // Now map[c1] <= abs_delta < map[c2].
  uint8_t code;
  if (2 * abs_delta < map[c1] + map[c2])
    code = c1;
  else
    code = c2;

  // Adjust for sign.
  return delta >= 0 ? code : code + kEntiresPerMap / 2;
}

/// @brief Encode a single block.
/// @param in Samples to be encoded.
/// @param out Encoded output block.
/// @param count Number of samples to encode.
/// @param stride The input sample stride.
/// @param map_no The quantization map number to use.
/// @param predictor_no The predictor to use.
void encode_block(const int16_t *in, uint8_t *out, int count, const int stride, const int map_no, const int predictor_no) {
  // Get the starting sample.
  int s_original = *in;
  in += stride;
  int s1 = s_original;

  // Encode the high bits of the map number as part of the starting sample.
  s1 = ((s1 >> 3) << 3) | (map_no >> 3);

  {
    // Do some rounding to improve the accuracy.
    const int rounded1 = s1 + (1 << 3);
    const int rounded2 = s1 - (1 << 3);
    if (rounded1 <= 32767 && std::abs(rounded1 - s_original) < std::abs(s1 - s_original))
      s1 = rounded1;
    if (rounded2 >= -32768 && std::abs(rounded2 - s_original) < std::abs(s1 - s_original))
      s1 = rounded2;
  }

  // Output the starting sample (16 bits).
  *out++ = s1;
  *out++ = s1 >> 8;

  // First nibble is the low 3 bits of the map number and the predictor.
  uint8_t byte = (map_no << 5) | (predictor_no << 4);

  // Get the decoding map for this block.
  const short *decode_map = kQuantLut[map_no];

  int s2 = s1;
  int nibbles_left = count - 1;
  for (; nibbles_left >= 2; nibbles_left -= 2) {
    // Encode the delta.
    int predicted = predictor_no == 0 ? s1 : 2 * s1 - s2;
    uint8_t code = encode(decode_map, *in - predicted);
    in += stride;
    byte |= code;

    // Decode and clamp for the next pass...
    s2 = s1;
    s1 = clamp(predicted + decode_map[code]);


    // Write encoded byte to output stream.
    *out++ = byte;

    // Encode the delta.
    predicted = predictor_no == 0 ? s1 : 2 * s1 - s2;
    code = encode(decode_map, *in - predicted);
    in += stride;
    byte = code << 4;

    // Decode and clamp for the next pass...
    s2 = s1;
    s1 = clamp(predicted + decode_map[code]);
  }
  if (nibbles_left) {
    // Encode the delta.
    int predicted = predictor_no == 0 ? s1 : 2 * s1 - s2;
    uint8_t code = encode(decode_map, *in - predicted);
    byte |= code;
  }

  // Write encoded byte to output stream.
  *out++ = byte;
}

/// @brief Encode a single block.
/// This routine will find the best encoding parameters for the given block,
/// and encode it accordingly.
/// @param in Samples to be encoded.
/// @param out Encoded output block.
/// @param count Number of samples to encode.
/// @param stride The input sample stride.
void encode_block(const int16_t *in, uint8_t *out, int count, const int stride) {
  if (count < 1) {
    return;
  }

  // Select which predictor to use for this block.
  int predictor_no = select_predictor(in, count);

#ifdef LIBSAC_ACCURATE_DD4A_ENCODE
#error Not yet implemented.
  // Try all quantization maps and pick the best one.
  // TODO(m): Implement me!
#else
  // Find max delta for this block.
  int max_delta = 0;
  if (predictor_no == 0) {
    for (int i = 1; i < count; ++i) {
      int delta = std::abs(static_cast<int>(in[i]) - static_cast<int>(in[i - 1]));
      max_delta = std::max(max_delta, delta);
    }
  } else {
    max_delta = std::abs(static_cast<int>(in[1]) - static_cast<int>(in[0]));
    for (int i = 2; i < count; ++i) {
      int predicted = 2 * static_cast<int>(in[i - 1]) - static_cast<int>(in[i - 2]);
      int delta = std::abs(static_cast<int>(in[i]) - predicted);
      max_delta = std::max(max_delta, delta);
    }
  }

  // Find the map that best matches the max delta.
  int map_no = kNumMaps - 1;
  for (int m = 0; m < kNumMaps; ++m) {
    if (max_delta < get_max_delta(kQuantLut[m])) {
      map_no = m;
      break;
    }
  }

  // Encode the block.
  encode_block(in, out, count, stride, map_no, predictor_no);
#endif
}

} // anonymous namespace

packed_data_t *encode(int num_samples, int num_channels, int sample_rate, int16_t **channels) {
  // Calculate encoded size.
  const int num_full_blocks = num_samples / kBlockSize;
  const int final_samples = num_samples - num_full_blocks * kBlockSize;
  const int encoded_size = num_channels * (num_full_blocks * kBytesPerBlock + block_size_in_bytes(final_samples));

  // Create the packed data container.
  scoped_ptr<packed_data_t> data(new packed_data_t(encoded_size, num_samples, num_channels, sample_rate, SAC_FORMAT_DD4A));

  // Encode all the full blocks.
#ifdef LIBSAC_USE_OPENMP
  #pragma omp parallel for
#endif
  for (int k = 0; k < num_full_blocks; ++k) {
    uint8_t *dst = data->data() + k * num_channels * kBytesPerBlock;
    for (int ch = 0; ch < num_channels; ++ch) {
      const int16_t *src = &channels[ch][k * kBlockSize];
      encode_block(src, dst, kBlockSize, 1);
      dst += kBytesPerBlock;
    }
  }

  // Encode the final samples if necessary.
  if (final_samples > 0) {
    uint8_t *dst = data->data() + num_full_blocks * num_channels * kBytesPerBlock;
    for (int ch = 0; ch < num_channels; ++ch) {
      const int16_t *src = &channels[ch][num_full_blocks * kBlockSize];
      encode_block(src, dst, final_samples, 1);
      dst += block_size_in_bytes(final_samples);
    }
  }

  return data.release();
}

} // namespace dd4a

} // namespace sac
