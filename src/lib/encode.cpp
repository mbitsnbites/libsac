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

#include "../include/libsac.h"

#include "encode_dd4a.h"
#include "encode_dd8a.h"
#include "packed_data.h"

using namespace sac;

extern "C"
sac_packed_data_t *sac_encode(int num_samples, int num_channels, int sample_rate, sac_encoding_t format, int16_t **channels) {
  // Check input arguments
  if (!channels || num_channels < 1 || num_samples < 1 || sample_rate < 1 || (format != SAC_FORMAT_DD4A && format != SAC_FORMAT_DD8A)) {
    return 0;
  }

  // Perform format dependent encoding.
  packed_data_t *out = 0;
  switch (format) {
    case SAC_FORMAT_DD4A:
      out = dd4a::encode(num_samples, num_channels, sample_rate, channels);
      break;
    case SAC_FORMAT_DD8A:
      out = dd8a::encode(num_samples, num_channels, sample_rate, channels);
      break;
    default:
      break;
  }

  return reinterpret_cast<sac_packed_data_t*>(out);
}
