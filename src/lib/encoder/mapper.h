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

#ifndef LIBSAC_MAPPER_H_
#define LIBSAC_MAPPER_H_

#include <cstdlib>

#include "libsac.h"

namespace sac {

template <int NUM_ENTRIES>
class map_t {
  public:
    map_t(const short *map) : m_map(map) {}

    /// @brief Decode a single delta value.
    /// @param code The code to decode.
    /// @returns The decoded delta value.
    int decode_delta(uint8_t code) const {
      return m_map[code];
    }

    /// @brief Encode a single delta value.
    /// @param map The quantization map to use.
    /// @param delta The delta value to quantize.
    /// @returns The quantized delta value (i.e. LUT index).
    /// @note This routine uses binary search in the decoding table, which is
    /// memory efficient but not very fast. A reverse LUT could be used
    /// instead, but it would require more RAM.
    uint8_t encode_delta(int delta) const {
      if (delta >= max_delta())
        return NUM_ENTRIES / 2 - 1;
      else if (delta <= min_delta())
        return NUM_ENTRIES - 1;

      // Binary search...
      int abs_delta = std::abs(delta);
      uint8_t c1 = 0, c2 = NUM_ENTRIES / 2 - 1;
      while (c2 > (c1 + 1)) {
        uint8_t mid = (c1 + c2) >> 1;
        if (abs_delta >= m_map[mid])
          c1 = mid;
        else
          c2 = mid;
      }

      // Now map[c1] <= abs_delta < map[c2].
      uint8_t code;
      if (2 * abs_delta < m_map[c1] + m_map[c2])
        code = c1;
      else
        code = c2;

      // Adjust for sign.
      return delta >= 0 ? code : code + NUM_ENTRIES / 2;
    }

    int min_delta() const {
      return m_map[NUM_ENTRIES - 1];
    }

    int max_delta() const {
      return m_map[NUM_ENTRIES / 2 - 1];
    }

  private:
    const short *m_map;

    // mapper_t is allowed to do uninitialized construction (it knows what it's
    // doing).
    map_t() {}
    template <int A, int B> friend class mapper_t;
};

template <int NUM_MAPS, int NUM_ENTRIES>
class mapper_t {
  public:
    mapper_t(const short luts[][NUM_ENTRIES]) {
      for (int i = 0; i < NUM_MAPS; ++i) {
        m_maps[i] = map_t<NUM_ENTRIES>(luts[i]);
      }
    }

    const map_t<NUM_ENTRIES> &operator[](int i) const {
      return m_maps[i];
    }

  private:
    map_t<NUM_ENTRIES> m_maps[NUM_MAPS];
};

} // namespace sac

#endif // LIBSAC_MAPPER_H_
