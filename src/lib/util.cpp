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

#include "util.h"

#include <algorithm>

namespace sac {

int select_predictor(const int16_t *block, const int count) {
  int delta;

  double err1 = 0.0;
  for (int i = 1; i < count; ++i) {
    delta = std::abs(static_cast<int>(block[i]) - static_cast<int>(block[i - 1]));
    err1 += static_cast<double>(delta) * static_cast<double>(delta);
  }

  delta = std::abs(static_cast<int>(block[1]) - static_cast<int>(block[0]));
  double err2 = static_cast<double>(delta) * static_cast<double>(delta);
  for (int i = 2; i < count; ++i) {
    int predicted = 2 * static_cast<int>(block[i - 1]) - static_cast<int>(block[i - 2]);
    delta = std::abs(static_cast<int>(block[i]) - predicted);
    err2 += static_cast<double>(delta) * static_cast<double>(delta);
  }

  return err2 <= err1 ? 1 : 0;
}

} // namespace sac
