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

uint16_t read_uint16(std::istream &f) {
  uint8_t buf[2];
  f.read(reinterpret_cast<char*>(buf), 2);
  return static_cast<uint16_t>(buf[0]) |
      (static_cast<uint16_t>(buf[1]) << 8);
}

uint32_t read_uint32(std::istream &f) {
  uint8_t buf[4];
  f.read(reinterpret_cast<char*>(buf), 4);
  return static_cast<uint32_t>(buf[0]) |
      (static_cast<uint32_t>(buf[1]) << 8) |
      (static_cast<uint32_t>(buf[2]) << 16) |
      (static_cast<uint32_t>(buf[3]) << 24);
}

} // anonymous namespace

packed_data_t *load_file(const char *file_name) {
  if (!file_name) {
    return 0;
  }

  std::ifstream f(file_name);

  // File master chunk.
  if (read_uint32(f) != 0x4D435044) {
    return 0;
  }
  const int file_size = read_uint32(f) + 8;
  int bytes_left = file_size - 8;

  scoped_ptr<packed_data_t> data;
  int num_samples = 0, sample_rate = 0, num_channels = 0;
  encoding data_encoding = FORMAT_UNDEFINED;

  // Read sub-chunks.
  while (bytes_left > 0) {
    int chunk_id = read_uint32(f);
    int chunk_size = read_uint32(f);
    bytes_left -= 8 + chunk_size;

    switch (chunk_id) {
      // FRMT: Format chunk (must come before the data chunk).
      case 0x544D5246: {
        if (chunk_size < 14) {
          return 0;
        }

        uint32_t format_fourcc = read_uint32(f);
        switch (format_fourcc) {
          case 0x41344444:
            data_encoding = FORMAT_DD4A;
            break;

          case 0x41384444:
            data_encoding = FORMAT_DD8A;
            break;

          default:
            return 0;
        }

        num_samples = read_uint32(f);
        num_channels = read_uint16(f);
        sample_rate = read_uint32(f);

        f.seekg(chunk_size - 14, std::ios_base::cur);
        break;
      }

      // DATA: Data chunk.
      case 0x41544144: {
        if (data_encoding == FORMAT_UNDEFINED) {
          // We don't have the data definition yet.
          return 0;
        }

        if (chunk_size > 0) {
          // Read the data...
          uint8_t *buffer = new uint8_t[chunk_size];
          f.read(reinterpret_cast<char*>(buffer), chunk_size);

          // Create the packed data container (transfers ownership of the data buffer).
          data.reset(new packed_data_t(buffer, chunk_size, num_samples, num_channels, sample_rate, data_encoding));
        }
        break;
      }

      // Any other chunk: skip.
      default: {
        f.seekg(chunk_size, std::ios_base::cur);
        break;
      }
    }
  }

  return data.release();
}

} // namespace libsac