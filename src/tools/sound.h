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

#ifndef TOOLS_SOUND_H_
#define TOOLS_SOUND_H_

#include <libsac.h>
#include <vector>

namespace tools {

/// @brief 16-bit PCM sound container.
class sound_t {
  public:
    sound_t(int num_samples, int num_channels, int sample_rate) :
        m_num_samples(num_samples),
        m_sample_rate(sample_rate) {
      for (int i = 0; i < num_channels; ++i) {
        m_channels.push_back(new int16_t[num_samples]);
      }
    }

    ~sound_t() {
      std::vector<int16_t*>::iterator it;
      for (it = m_channels.begin(); it != m_channels.end(); it++) {
        delete[] *it;
      }
    }

    int16_t *channel(int channel) const {
      if (channel < 0 || channel >= static_cast<int>(m_channels.size())) {
        return 0;
      }
      return m_channels[channel];
    }

    int16_t **channels() const {
      // TODO(m): Fix constness...
      return const_cast<int16_t **>(&m_channels[0]);
    }

    int num_samples() const {
      return m_num_samples;
    }

    int num_channels() const {
      return static_cast<int>(m_channels.size());
    }

    int sample_rate() const {
      return m_sample_rate;
    }

  private:
    std::vector<int16_t*> m_channels;
    int m_num_samples;
    int m_sample_rate;
};

} // namespace tools

#endif // TOOLS_SOUND_H_
