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

#include "libsac.h"

#include <algorithm>

#include "decoder/decode_dd4a.h"
#include "decoder/decode_dd8a.h"
#include "packed_data.h"

using namespace sac;

extern "C"
void sac_decode_channel(int16_t *out, const sac_packed_data_t *in_, int start, int count, int channel) {
  const packed_data_t *in = reinterpret_cast<const packed_data_t*>(in_);

  // Missing input/output buffers?
  if (!in || !out) {
    return;
  }

  // Invalid channel?
  if (channel < 0 || channel >= in->num_channels()) {
    return;
  }

  // Clamp arguments to the range of the input data.
  if (start < 0) {
    count += start;
    start = 0;
  }
  count = std::min(start + count, in->num_samples()) - start;

  // Nothing to do?
  if (count < 1) {
    return;
  }

  // Perform format dependent decoding.
  switch (in->encoding()) {
    case SAC_FORMAT_DD4A:
      dd4a::decode_channel(out, in, start, count, channel);
      break;
    case SAC_FORMAT_DD8A:
      dd8a::decode_channel(out, in, start, count, channel);
      break;
    case SAC_FORMAT_UNDEFINED:
    default:
      break;
  }
}

extern "C"
void sac_decode_interleaved(int16_t *out, const sac_packed_data_t *in_, int start, int count) {
  const packed_data_t *in = reinterpret_cast<const packed_data_t*>(in_);

  // Missing input/output buffers?
  if (!in || !out) {
    return;
  }

  // Clamp arguments to the range of the input data.
  if (start < 0) {
    count += start;
    start = 0;
  }
  count = std::min(start + count, in->num_samples()) - start;

  // Nothing to do?
  if (count < 1) {
    return;
  }

  // Perform format dependent decoding.
  switch (in->encoding()) {
    case SAC_FORMAT_DD4A:
      dd4a::decode_interleaved(out, in, start, count);
      break;
    case SAC_FORMAT_DD8A:
      dd8a::decode_interleaved(out, in, start, count);
      break;
    case SAC_FORMAT_UNDEFINED:
    default:
      break;
  }
}
