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
// - 8 different maps (4 KB using 16-bit deltas)
// - Block format:
//   [ssss|ssss][ssss|mmmp][D1][D2] ... [D15]
//    s: Starting point (12-bit precision)
//    m: Map-selection (3 bits)
//    p: Predictor-selection (1 bit)
//   Dx: Delta samples (8 bits / delta)
//-----------------------------------------------------------------------------
// This decoder is fairly generic, and not very optimized. When performance is
// critical, and the configuraiton is known, it is recommended that a
// specialized decoder is written (e.g. if all decoding operations will be
// block aligned, and only interleaved / non-interleaved is to be used).
//-----------------------------------------------------------------------------

#include "../include/libsac.h"

#include "decode_dd8a.h"
#include "packed_data.h"

namespace libsac {

void decode_channel(int16_t *out, const packed_data_t *in, int start, int count, int channel) {
  if (!in || !out)
    return;

  switch (in->data_encoding()) {
  case packed_data_t::DD8A:
    dd8a::decode_channel(out, in, start, count, channel);
    break;
  case packed_data_t::DD4A:
    /* NOT YET IMPLEMENTED */;
    break;
  }
}

void decode_interleaved(int16_t *out, const packed_data_t *in, int start, int count) {
  if (!in || !out)
    return;

  switch (in->data_encoding()) {
  case packed_data_t::DD8A:
    dd8a::decode_interleaved(out, in, start, count);
    break;
  case packed_data_t::DD4A:
    /* NOT YET IMPLEMENTED */;
    break;
  }
}

} // namespace libsac
