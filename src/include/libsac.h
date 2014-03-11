/* -*- Mode: c; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *-----------------------------------------------------------------------------
 * Copyright (c) 2014 Marcus Geelnard
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not
 *     be misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *     distribution.
 *---------------------------------------------------------------------------*/

#ifndef _LIBSAC_H_
#define _LIBSAC_H_

#if defined(_MSC_VER) && (_MSC_VER < 1600)
/* MSVC before 2010 didn't have stdint.h */
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
#else
# include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
 * Packed data definitions.
 *---------------------------------------------------------------------------*/

enum sac_encoding_t {
  SAC_FORMAT_UNDEFINED = 0,
  SAC_FORMAT_DD4A = 1,
  SAC_FORMAT_DD8A = 2
};

typedef void sac_packed_data_t;

void sac_free(sac_packed_data_t *data);

int sac_get_size(const sac_packed_data_t *data);
int sac_get_num_samples(const sac_packed_data_t *data);
int sac_get_num_channels(const sac_packed_data_t *data);
int sac_get_sample_rate(const sac_packed_data_t *data);
sac_encoding_t sac_get_encoding(const sac_packed_data_t *data);


/*-----------------------------------------------------------------------------
 * File and stream I/O.
 *---------------------------------------------------------------------------*/

sac_packed_data_t *sac_load_file(const char *file_name);
void sac_save_file(const char *file_name, const sac_packed_data_t *data);


/*-----------------------------------------------------------------------------
 * Decoding.
 *---------------------------------------------------------------------------*/

void sac_decode_channel(int16_t *out, const sac_packed_data_t *in, int start, int count, int channel);
void sac_decode_interleaved(int16_t *out, const sac_packed_data_t *in, int start, int count);


/*-----------------------------------------------------------------------------
 * Encoding.
 *---------------------------------------------------------------------------*/

sac_packed_data_t *sac_encode(int num_samples, int num_channels, int sample_rate, sac_encoding_t format, int16_t **channels);


#ifdef __cplusplus
}
#endif

#endif /* _LIBSAC_H_ */
