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

#ifndef LIBSAC_DECODE_DD4A_H_
#define LIBSAC_DECODE_DD4A_H_

#include "../include/libsac.h"
#include "packed_data.h"

namespace sac {

namespace dd4a {

void decode_channel(int16_t *out, const packed_data_t *in, int start, int count, int channel);

void decode_interleaved(int16_t *out, const packed_data_t *in, int start, int count);

} // namespace dd4a

} // namespace sac

#endif // LIBSAC_DECODE_DD4A_H_
