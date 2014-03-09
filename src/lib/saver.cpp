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

#include "../include/libsac.h"

#include <fstream>

#include "packed_data.h"
#include "util.h"

namespace libsac {

namespace {

void write_uint16(std::ostream &f, uint16_t x) {
  uint8_t buf[2];
  buf[0] = x;
  buf[1] = x >> 8;
  f.write(reinterpret_cast<char*>(buf), 2);
}

void write_uint32(std::ostream &f, uint32_t x) {
  uint8_t buf[4];
  buf[0] = x;
  buf[1] = x >> 8;
  buf[2] = x >> 16;
  buf[3] = x >> 24;
  f.write(reinterpret_cast<char*>(buf), 4);
}

} // anonymous namespace

void save_file(const char *file_name, const packed_data_t *data) {
  if (!file_name || !data) {
    return;
  }

  // Total file size.
  const int file_size = 8 + 8 + 14 + 8 + data->size();

  // Determine format fourcc code.
  uint32_t format_fourcc = 0;
  switch (data->data_encoding()) {
    case FORMAT_DD4A:
      format_fourcc = 0x41344444;
      break;

    case FORMAT_DD8A:
      format_fourcc = 0x41384444;
      break;

    default:
      // Unhandled...
      return;
  }

  std::ofstream f(file_name);

  // File master chunk.
  write_uint32(f, 0x4D435044);            // "DPCM"
  write_uint32(f, file_size - 8);         // Master chunk size.

  // Sub chunk: Format (must come before the data chunk).
  write_uint32(f, 0x544D5246);            // "FRMT"
  write_uint32(f, 14);                    // Chunk size.
  write_uint32(f, format_fourcc);         // Packed data format.
  write_uint32(f, data->num_samples());   // Number of samples.
  write_uint16(f, data->num_channels());  // Number of channels.
  write_uint32(f, data->sample_rate());   // Sample rate (Hz).

  // Sub chunk: Data.
  write_uint32(f, 0x41544144);            // "DATA"
  write_uint32(f, data->size());          // Chunk size.
  f.write(reinterpret_cast<char*>(data->data()), data->size());
}

} // namespace libsac
