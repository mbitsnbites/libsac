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

#include "encoder/analyzer.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace sac {

analysis_result_t analyze_block(const int16_t *block, int count, int stride) {
  if (count < 2) {
    return analysis_result_t(0, 0, 0);
  }

  // Calculate error sums and max delta for predictors 0 and 1.
  double err0 = 0.0;
  double err1 = 0.0;
  int max_delta0 = 0;
  int max_delta1 = 0;
  {
    const int16_t *src = block;
    int s1 = *src;
    src += stride;
    int s2 = s1;
    for (int i = 1; i < count; ++i) {
      int predicted0 = s1;
      int predicted1 = 2 * s1 - s2;
      s2 = s1;
      s1 = *src;
      src += stride;
      int delta0 = s1 - predicted0;
      int delta1 = s1 - predicted1;
      max_delta0 = std::max(std::abs(delta0), max_delta0);
      max_delta1 = std::max(std::abs(delta1), max_delta1);
      err0 += static_cast<double>(delta0) * static_cast<double>(delta0);
      err1 += static_cast<double>(delta1) * static_cast<double>(delta1);
    }
  }

  // Select predictor and the corresponding delta figures.
  int predictor_no;
  int rms_delta;
  int max_delta;
  if (err1 > err0) {
    predictor_no = 0;
    double rms = std::sqrt(err0 / static_cast<double>(count - 1));
    rms_delta = static_cast<int>(rms);
    max_delta = max_delta0;
  } else {
    predictor_no = 1;
    double rms = std::sqrt(err1 / static_cast<double>(count - 1));
    rms_delta = static_cast<int>(rms);
    max_delta = max_delta1;
  }

  return analysis_result_t(predictor_no, max_delta, rms_delta);
}

} // namespace sac
