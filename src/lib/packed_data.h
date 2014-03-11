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

#ifndef LIBSAC_PACKED_DATA_H_
#define LIBSAC_PACKED_DATA_H_

#include "../include/libsac.h"

namespace sac {

class packed_data_t {
  public:
    packed_data_t(
        int size,
        int num_samples,
        int num_channels,
        int sample_rate,
        sac_encoding_t encoding)
        : m_size(size),
          m_num_samples(num_samples),
          m_num_channels(num_channels),
          m_sample_rate(sample_rate),
          m_encoding(encoding) {
      m_data = new uint8_t[size];
    }

    ~packed_data_t() {
      delete[] m_data;
    }

    uint8_t *data() const {
      return m_data;
    }

    int size() const {
      return m_size;
    }

    int num_samples() const {
      return m_num_samples;
    }

    int num_channels() const {
      return m_num_channels;
    }

    int sample_rate() const {
      return m_sample_rate;
    }

    sac_encoding_t encoding() const {
      return m_encoding;
    }

  private:
    packed_data_t();
    packed_data_t(const packed_data_t& other);
    packed_data_t& operator=(const packed_data_t& other);

    uint8_t *m_data;
    const int m_size;
    const int m_num_samples;
    const int m_num_channels;
    const int m_sample_rate;
    const sac_encoding_t m_encoding;
};

} // namespace sac

#endif // LIBSAC_PACKED_DATA_H_
