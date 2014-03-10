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

#ifndef _LIBSAC_H_
#define _LIBSAC_H_

#ifdef LIBSAC_CUSTOM_STDINT
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
#else
# include <stdint.h>
#endif

namespace libsac {

//-----------------------------------------------------------------------------
// Packed data definitions.
//-----------------------------------------------------------------------------

enum encoding {
  FORMAT_UNDEFINED = 0,
  FORMAT_DD4A = 1,
  FORMAT_DD8A = 2
};

class packed_data_t;

void free_data(packed_data_t *data);


//-----------------------------------------------------------------------------
// File and stream I/O.
//-----------------------------------------------------------------------------

packed_data_t *load_file(const char *file_name);
void save_file(const char *file_name, const packed_data_t *data);


//-----------------------------------------------------------------------------
// Decoding.
//-----------------------------------------------------------------------------

void decode_channel(int16_t *out, const packed_data_t *in, int start, int count, int channel);
void decode_interleaved(int16_t *out, const packed_data_t *in, int start, int count);


//-----------------------------------------------------------------------------
// Encoding.
//-----------------------------------------------------------------------------

packed_data_t *encode(int num_samples, int num_channels, int sample_rate, encoding format, const int16_t **channels);
packed_data_t *encode_interleaved(int num_samples, int num_channels, int sample_rate, encoding format, const int16_t *data);


} // namespace libsac

#endif // _LIBSAC_H_
