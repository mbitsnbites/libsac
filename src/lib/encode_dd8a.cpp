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
// This encoder is fairly generic, and not very optimized. There are several
// possible optimizations that could improve encoding speeds.
//-----------------------------------------------------------------------------

#include "encode_dd8a.h"

#include <algorithm>
#include <cstdlib>

#include "analyzer.h"
#include "mapper.h"
#include "packed_data.h"
#include "util.h"

namespace sac {

namespace dd8a {

// Defined in quant_lut_dd8a.cpp
extern const short kQuantLut[8][256];

namespace {

int block_size_in_bytes(const int num_samples) {
  return num_samples + 1;
}

const int kBlockSize = 16;
const int kBytesPerBlock = block_size_in_bytes(kBlockSize);
const int kNumMaps = 8;
const int kEntriesPerMap = 256;

class encoder_t {
  public:
    encoder_t() : m_mapper(kQuantLut) {}

    /// @brief Encode a single block.
    /// This routine will find the best encoding parameters for the given block,
    /// and encode it accordingly.
    /// @param in Samples to be encoded.
    /// @param out Encoded output block.
    /// @param count Number of samples to encode.
    /// @param stride The input sample stride.
    void encode_block(const int16_t *in, uint8_t *out, int count, int stride) {
      if (count < 1) {
        return;
      }

      // Analyze the block (select predictor etc).
      const analysis_result_t analysis = analyze_block(in, count, stride);

      // Find the map that best matches the max delta.
      // TODO(m): Make better use of the analysis result.
      int map_no = kNumMaps - 1;
      for (int m = 0; m < kNumMaps; ++m) {
        if (analysis.max_delta < m_mapper[m].max_delta()) {
          map_no = m;
          break;
        }
      }

      // Encode the block.
      encode_block(in, out, count, stride, map_no, analysis.predictor_no);
    }

  private:
    /// @brief Encode a single block.
    /// This is the encoder core for the DD8A format.
    /// @param in Samples to be encoded.
    /// @param out Encoded output block.
    /// @param count Number of samples to encode.
    /// @param stride The input sample stride.
    /// @param map_no The quantization map number to use.
    /// @param predictor_no The predictor to use.
    void encode_block(const int16_t *in, uint8_t *out, int count, int stride, int map_no, int predictor_no) {
      // Get the starting sample.
      int s_original = *in;
      in += stride;
      int s1 = s_original;

      // Encode predictor and map number into the starting sample.
      s1 = ((s1 >> 4) << 4) | (map_no << 1) | predictor_no;

      {
        // Do some rounding to improve the accuracy.
        const int rounded1 = s1 + (1 << 4);
        const int rounded2 = s1 - (1 << 4);
        if (rounded1 <= 32767 && std::abs(rounded1 - s_original) < std::abs(s1 - s_original))
          s1 = rounded1;
        if (rounded2 >= -32768 && std::abs(rounded2 - s_original) < std::abs(s1 - s_original))
          s1 = rounded2;
      }

      // Output the starting sample (16 bits).
      *out++ = s1;
      *out++ = s1 >> 8;

      // Get the coding map for this block.
      const map_t<kEntriesPerMap> &map = m_mapper[map_no];

      int s2 = s1;
      for (int i = 1; i < count; ++i) {
        // Encode the delta.
        int predicted = predictor_no == 0 ? s1 : 2 * s1 - s2;
        uint8_t code = map.encode_delta(*in - predicted);
        in += stride;

        // Output encoded byte.
        *out++ = code;

        // Decode and clamp for the next pass...
        s2 = s1;
        s1 = clamp(predicted + map.decode_delta(code));
      }
    }

    mapper_t<kNumMaps, kEntriesPerMap> m_mapper;
};

} // anonymous namespace

packed_data_t *encode(int num_samples, int num_channels, int sample_rate, int16_t **channels) {
  // Calculate encoded size.
  const int num_full_blocks = num_samples / kBlockSize;
  const int final_samples = num_samples - num_full_blocks * kBlockSize;
  const int encoded_size = num_channels * (num_full_blocks * kBytesPerBlock + block_size_in_bytes(final_samples));

  // Create the packed data container.
  scoped_ptr<packed_data_t> data(new packed_data_t(encoded_size, num_samples, num_channels, sample_rate, SAC_FORMAT_DD8A));

  encoder_t encoder;

  // Encode all the full blocks.
#ifdef LIBSAC_USE_OPENMP
  #pragma omp parallel for
#endif
  for (int k = 0; k < num_full_blocks; ++k) {
    uint8_t *dst = data->data() + k * num_channels * kBytesPerBlock;
    for (int ch = 0; ch < num_channels; ++ch) {
      const int16_t *src = &channels[ch][k * kBlockSize];
      encoder.encode_block(src, dst, kBlockSize, 1);
      dst += kBytesPerBlock;
    }
  }

  // Encode the final samples if necessary.
  if (final_samples > 0) {
    uint8_t *dst = data->data() + num_full_blocks * num_channels * kBytesPerBlock;
    for (int ch = 0; ch < num_channels; ++ch) {
      const int16_t *src = &channels[ch][num_full_blocks * kBlockSize];
      encoder.encode_block(src, dst, final_samples, 1);
      dst += block_size_in_bytes(final_samples);
    }
  }

  return data.release();
}

} // namespace dd8a

} // namespace sac
