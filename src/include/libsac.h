#ifndef _LIBSAC_H_
#define _LIBSAC_H_

#ifdef LIBSAC_CUSTOM_CSTDINT
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
#else
# include <stdint.h>
#endif

// Forward declarations (defined internally).
class sac_data_t;


//-----------------------------------------------------------------------------
// File and stream I/O.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Decoding.
//-----------------------------------------------------------------------------

void decode_channel_range(int16_t *out, const sac_data_t *in, int start, int count, int channel);
void decode_interleaved_range(int16_t *out, const sac_data_t *in, int start, int count);


//-----------------------------------------------------------------------------
// Encoding.
//-----------------------------------------------------------------------------



#endif // _LIBSAC_H_

