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

#include "encode_dd8a.h"
#include "packed_data.h"
#include "util.h"

namespace sac {

packed_data_t *encode(int num_samples, int num_channels, int sample_rate, encoding format, const int16_t **channels) {
  // Check input arguments
  if (!channels || num_channels < 1 || num_samples < 1 || sample_rate < 1 || (format != FORMAT_DD4A && format != FORMAT_DD8A)) {
    return 0;
  }

  // Perform format dependent encoding.
  switch (format) {
    case FORMAT_DD4A:
      // NOT YET IMPLEMENTED!
      // return dd4a::encode(num_samples, num_channels, sample_rate, format, channels);
      return 0;
    case FORMAT_DD8A:
      return dd8a::encode(num_samples, num_channels, sample_rate, channels);
    case FORMAT_UNDEFINED:
    default:
      return 0;
  }
}

packed_data_t *encode_interleaved(int num_samples, int num_channels, int sample_rate, encoding format, const int16_t *data) {
  /* NOT YET IMPLEMENTED */
  return 0;
}

} // namespace sac

