#include "decode_dd8a.h"

#include "sac_data.h"
#include "util.h"

#include <algorithm>

namespace libsac {

namespace dd8a {

extern const short kQuantLut[8][256];

namespace {

const int kBlockSize = 16;
const int kBytesPerBlock = kBlockSize + 1;

/// @brief Decode a single block.
/// This is a slightly more generic version of the decoding routine that is
/// able to handle a starting offset.
/// @param in Block of data to be decoded.
/// @param out Decoded output samples.
/// @param offset First sample in the encoded block to output.
/// @param count Number of samples to output.
/// @param stride The output sample stride.
void decode_block(const uint8_t *in, int16_t *out, const int offset, int count, const int stride) {
  // Get the starting sample (16 bits).
  int16_t s16 = static_cast<int16_t>(in[0]) |
      (static_cast<int16_t>(in[1]) << 8);
  int s = static_cast<int>(s16);
  in += 2;

  // Get the decoding map for this block.
  const int map_no = s & 7;
  const short *decode_map = kQuantLut[map_no];

  // Decode but don't output offset samples.
  for (int i = 1; i < offset; ++i) {
    uint8_t code = *in++;
    s = clamp(s + decode_map[code]);
  }

  // Write the first sample to the output stream.
  *out = s;
  out += stride;

  for (int i = 1; i < count; ++i) {
    // Get the encoded byte from the input stream.
    uint8_t code = *in++;

    // Decode and clamp.
    s = clamp(s + decode_map[code]);

    // Write sample to the output stream.
    *out++ = s;
    out += stride;
  }
}

/// @brief Decode a single block.
/// This is a slightly simpler version that assumes that the starting sample is
/// block aligned.
/// @param in Block of data to be decoded.
/// @param out Decoded output samples.
/// @param count Number of samples to output.
/// @param stride The output sample stride.
void decode_block(const uint8_t *in, int16_t *out, int count, const int stride) {
  // Get the starting sample (16 bits).
  int16_t s16 = static_cast<int16_t>(in[0]) |
      (static_cast<int16_t>(in[1]) << 8);
  int s = static_cast<int>(s16);
  in += 2;

  // Get the decoding map for this block.
  const int map_no = s & 7;
  const short *decode_map = kQuantLut[map_no];

  // Write the first sample to the output stream.
  *out = s;
  out += stride;

  for (int i = 1; i < count; ++i) {
    // Get the encoded byte from the input stream.
    uint8_t code = *in++;

    // Decode and clamp.
    s = clamp(s + decode_map[code]);

    // Write sample to the output stream.
    *out = s;
    out += stride;
  }
}

} // anonymous namespace

void decode_channel_range(int16_t *out, const sac_data_t *in, int start, int count, int channel) {
  // Invalid channel?
  if (channel < 0 || channel >= in->num_channels())
    return;

  // Clamp arguments to the range of the input data.
  if (start < 0) {
    count += start;
    start = 0;
  }
  count = std::min(start + count, in->num_samples()) - start;

  // Nothing to do?
  if (count < 1)
    return;

  int start_block = start / kBlockSize;
  int offset = start - start_block * kBlockSize;

  // src points to the first block.
  uint8_t *src = in->data() + (start_block * in->num_channels() + channel) * kBytesPerBlock;

  // If the starting point is unaligned, do a run-in.
  if (offset > 0) {
    int local_count = std::min(kBlockSize - offset, count);
    decode_block(src, out, offset, local_count, 1);
    src += kBytesPerBlock * in->num_channels();
    out += local_count;
    count -= local_count;
  }

  // Proceed with block-aligned operations.
  for (; count > 0; count -= kBlockSize) {
    int local_count = std::min(kBlockSize, count);
    decode_block(src, out, local_count, 1);
    src += kBytesPerBlock * in->num_channels();
    out += local_count;
  }
}

void decode_interleaved_range(int16_t *out, const sac_data_t *in, int start, int count) {
  // Clamp arguments to the range of the input data.
  if (start < 0) {
    count += start;
    start = 0;
  }
  count = std::min(start + count, in->num_samples()) - start;

  // Nothing to do?
  if (count < 1)
    return;

  int start_block = start / kBlockSize;
  int offset = start - start_block * kBlockSize;

  // src points to the first block.
  uint8_t *src = in->data() + (start_block * in->num_channels()) * kBytesPerBlock;

  // If the starting point is unaligned, do a run-in.
  if (offset > 0) {
    int local_count = std::min(kBlockSize - offset, count);
    int16_t *out2 = out;
    for (int ch = 0; ch < in->num_channels(); ch++) {
      decode_block(src, out2, offset, local_count, in->num_channels());
      src += kBytesPerBlock;
      out2++;
    }
    out += local_count * in->num_channels();
    count -= local_count;
  }

  // Proceed with block-aligned operations.
  for (; count > 0; count -= kBlockSize) {
    int local_count = std::min(kBlockSize, count);
    int16_t *out2 = out;
    for (int ch = 0; ch < in->num_channels(); ch++) {
      decode_block(src, out2, local_count, in->num_channels());
      src += kBytesPerBlock;
      out2++;
    }
    out += local_count * in->num_channels();
  }
}

} // namespace dd8a

} // namespace libsac
