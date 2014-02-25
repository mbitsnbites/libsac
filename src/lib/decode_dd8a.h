#ifndef DECODE_DD8A_H
#define DECODE_DD8A_H

#include "../include/libsac.h"

namespace libsac {

namespace dd8a {

void decode_channel_range(int16_t *out, const sac_data_t *in, int start, int count, int channel);

void decode_interleaved_range(int16_t *out, const sac_data_t *in, int start, int count);

} // namespace dd8a

} // namespace libsac

#endif // DECODE_DD8A_H
