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

#ifndef LIBSAC_ANALYZER_H_
#define LIBSAC_ANALYZER_H_

#include "libsac.h"

namespace sac {

/// @brief The analysis result for a block of samples.
struct analysis_result_t {
  analysis_result_t(int predictor_no, int max_delta, int rms_delta) :
      predictor_no(predictor_no),
      max_delta(max_delta),
      rms_delta(rms_delta) {}

  /// The best predictor to use for this block.
  const int predictor_no;

  /// The maximum delta for this block using the given predictor.
  const int max_delta;

  /// The root mean square delta for this block using the given predictor.
  const int rms_delta;
};

/// @brief Analyze the given block.
/// @param block Start of the block.
/// @param count Number of samples in the block.
/// @param stride The input sample stride.
/// @returns The analysis result.
analysis_result_t analyze_block(const int16_t *block, int count, int stride);

} // namespace sac

#endif // LIBSAC_ANALYZER_H_
