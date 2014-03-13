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
// 8-bit DDPCM:
// - 16 samples per block
// - 17 bytes / block (53.1% of original size)
// - 8 quantization maps (256 entires per map)
// - Block format (bit layout):
//   [ssss|ssss][ssss|mmmp][D1......][D2......][D3......] ... [D15......]
//    s: Starting point (12 bits)
//    m: Map-selection (3 bits)
//    p: Predictor-selection (1 bit)
//   Dx: Delta samples (8 bits / delta)
//-----------------------------------------------------------------------------
// This decoder is fairly generic, and not very optimized. When performance is
// critical, and the configuraiton is known, it is recommended that a
// specialized decoder is written (e.g. if all decoding operations will be
// block aligned, and only interleaved / non-interleaved output is to be used).
//-----------------------------------------------------------------------------

#include "decoder/decode_dd8a.h"

#include <algorithm>

#include "packed_data.h"
#include "util.h"

namespace sac {

namespace dd8a {

// Defined in quant_lut_dd8a.cpp
extern const short kQuantLut[8][256];

namespace {

int block_size_in_bytes(int num_samples) {
  return num_samples + 1;
}

const int kBlockSize = 16;
const int kBytesPerBlock = block_size_in_bytes(kBlockSize);

/// @brief Decode a single block.
/// @param in Block of data to be decoded.
/// @param out Decoded output samples.
/// @param offset First sample in the encoded block to output.
/// @param count Number of samples to output.
/// @param stride The output sample stride.
void decode_block(const uint8_t *in, int16_t *out, int offset, int count, int stride) {
  // Get the starting sample (16 bits).
  int16_t s16 = static_cast<int16_t>(in[0]) |
      (static_cast<int16_t>(in[1]) << 8);
  int s1 = static_cast<int>(s16);
  in += 2;

  // Get the predictor for this block.
  const int predictor_no = s1 & 1;

  // Get the decoding map for this block.
  const short *decode_map = kQuantLut[(s1 >> 1) & 7];

  int s2 = s1;

  // Decode but don't output offset samples.
  for (int i = 0; i < offset; ++i) {
    int predicted = predictor_no == 0 ? s1 : 2 * s1 - s2;
    s2 = s1;
    s1 = clamp(predicted + decode_map[*in++]);
  }

  // Write the first sample to the output stream.
  *out = s1;
  out += stride;

  for (int i = 1; i < count; ++i) {
    // Predict the next sample.
    int predicted = predictor_no == 0 ? s1 : 2 * s1 - s2;

    // Decode and clamp.
    s2 = s1;
    s1 = clamp(predicted + decode_map[*in++]);

    // Write sample to the output stream.
    *out = s1;
    out += stride;
  }
}

} // anonymous namespace

void decode_channel(int16_t *out, const packed_data_t *in, int start, int count, int channel) {
  int start_block = start / kBlockSize;
  int offset = start - start_block * kBlockSize;

  // src points to the first block.
  uint8_t *src = in->data() + (start_block * in->num_channels() + channel) * kBytesPerBlock;

  // Decode as many blocks as required.
  while (count > 0) {
    int local_count = std::min(kBlockSize - offset, count);
    decode_block(src, out, offset, local_count, 1);
    src += kBytesPerBlock * in->num_channels();
    out += local_count;
    count -= local_count;

    // After the first pass, we are block aligned.
    offset = 0;
  }
}

void decode_interleaved(int16_t *out, const packed_data_t *in, int start, int count) {
  const int start_block = start / kBlockSize;
  int offset = start - start_block * kBlockSize;

  // src points to the first block.
  uint8_t *src = in->data() + (start_block * in->num_channels()) * kBytesPerBlock;

  // Decode as many blocks as required.
  while (count > 0) {
    int local_count = std::min(kBlockSize - offset, count);
    int16_t *out2 = out;
    for (int ch = 0; ch < in->num_channels(); ch++) {
      decode_block(src, out2, offset, local_count, in->num_channels());
      src += kBytesPerBlock;
      out2++;
    }
    out += local_count * in->num_channels();
    count -= local_count;

    // After the first pass, we are block aligned.
    offset = 0;
  }
}

} // namespace dd8a

} // namespace sac
