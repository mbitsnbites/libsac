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

#include "packed_data.h"

using namespace sac;

extern "C"
void sac_free(sac_packed_data_t *data_) {
  packed_data_t *data = reinterpret_cast<packed_data_t*>(data_);
  delete data;
}

extern "C"
int sac_get_size(const sac_packed_data_t *data_) {
  const packed_data_t *data = reinterpret_cast<const packed_data_t*>(data_);
  if (!data) {
    return 0;
  }
  return data->size();
}

extern "C"
int sac_get_num_samples(const sac_packed_data_t *data_) {
  const packed_data_t *data = reinterpret_cast<const packed_data_t*>(data_);
  if (!data) {
    return 0;
  }
  return data->num_samples();
}

extern "C"
int sac_get_num_channels(const sac_packed_data_t *data_) {
  const packed_data_t *data = reinterpret_cast<const packed_data_t*>(data_);
  if (!data) {
    return 0;
  }
  return data->num_channels();
}

extern "C"
int sac_get_sample_rate(const sac_packed_data_t *data_) {
  const packed_data_t *data = reinterpret_cast<const packed_data_t*>(data_);
  if (!data) {
    return 0;
  }
  return data->sample_rate();
}

extern "C"
sac_encoding_t sac_get_encoding(const sac_packed_data_t *data_) {
  const packed_data_t *data = reinterpret_cast<const packed_data_t*>(data_);
  if (!data) {
    return SAC_FORMAT_UNDEFINED;
  }
  return data->encoding();
}
