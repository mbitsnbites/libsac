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

#include "file_io.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "hires_time.h"
#include "scoped_ptr.h"
#include "sound.h"

namespace tools {

namespace {

uint16_t read_uint16(std::istream& s) {
  uint8_t buf[2];
  s.read(reinterpret_cast<char*>(buf), 2);
  return static_cast<uint32_t>(buf[0]) |
      (static_cast<uint32_t>(buf[1]) << 8);
}

uint32_t read_uint32(std::istream& s) {
  uint8_t buf[4];
  s.read(reinterpret_cast<char*>(buf), 4);
  return static_cast<uint32_t>(buf[0]) |
      (static_cast<uint32_t>(buf[1]) << 8) |
      (static_cast<uint32_t>(buf[2]) << 16) |
      (static_cast<uint32_t>(buf[3]) << 24);
}

void write_uint16(std::ostream& s, uint16_t x) {
  uint8_t buf[2];
  buf[0] = x;
  buf[1] = x >> 8;
  s.write(reinterpret_cast<char*>(buf), 2);
}

void write_uint32(std::ostream& s, uint32_t x) {
  uint8_t buf[4];
  buf[0] = x;
  buf[1] = x >> 8;
  buf[2] = x >> 16;
  buf[3] = x >> 24;
  s.write(reinterpret_cast<char*>(buf), 4);
}

} // anonymous namespace

sound_t *load_wave(const std::string& file_name) {
  std::ifstream s(file_name.c_str());

  // Read header.
  if (read_uint32(s) != 0x46464952) {
    return 0;
  }
  uint32_t file_size = read_uint32(s) + 8;
  if (read_uint32(s) != 0x45564157) {
    return 0;
  }
  file_size -= 12;

  scoped_ptr<sound_t> sound;
  int num_channels = 0, bits_per_sample = 0, sample_rate = 0;

  // Read chunks...
  while (file_size > 0) {
    uint32_t chunk_id = read_uint32(s);
    uint32_t chunk_size = read_uint32(s);
    file_size -= 8 + chunk_size;

    switch (chunk_id) {
      case 0x20746d66: {
        // "fmt "
        if (read_uint16(s) != 1) {
          // We only support PCM.
          return 0;
        }
        num_channels = read_uint16(s);
        sample_rate = read_uint32(s);
        read_uint32(s); // Byte rate
        read_uint16(s); // Block align
        bits_per_sample = read_uint16(s);
        if (bits_per_sample != 16) {
          // We only support 16-bit.
          return 0;
        }

        s.seekg(chunk_size - 16, std::ios_base::cur);
        break;
      }

      case 0x61746164: {
        // "data"
        if (num_channels < 1) {
          return 0;
        }

        // Create the sound.
        int num_samples = chunk_size / (num_channels * (bits_per_sample / 8));
        sound.reset(new sound_t(num_samples, num_channels, sample_rate));

        // Read the data...
        std::vector<int16_t> buffer(chunk_size);
        s.read(reinterpret_cast<char*>(&buffer[0]), chunk_size);

        // Convert the buffered data to a sound.
        // TODO(m): Add support for big endian machines.
        for (int k = 0; k < num_samples; ++k) {
          for (int ch = 0; ch < num_channels; ++ch) {
            sound->channel(ch)[k] = buffer[k * num_channels + ch];
          }
        }

        break;
      }

      default: {
        s.seekg(chunk_size, std::ios_base::cur);
      }
    }
  }

  return sound.release();
}

void save_wave(const std::string &file_name, const sound_t *sound) {
  std::ofstream s(file_name.c_str());

  int data_size = sound->num_samples() * sound->num_channels() * 2;
  int file_size = 12 + 24 + 8 + data_size;

  // Write header.
  write_uint32(s, 0x46464952);
  write_uint32(s, file_size - 8);
  write_uint32(s, 0x45564157);

  // Write fmt chunk.
  write_uint32(s, 0x20746d66);
  write_uint32(s, 24 - 8);
  write_uint16(s, 1);
  write_uint16(s, sound->num_channels());
  write_uint32(s, sound->sample_rate());
  write_uint32(s, sound->sample_rate() * sound->num_channels() * 2);
  write_uint16(s, sound->num_channels() * 2);
  write_uint16(s, 16);

  // Write data chunk.
  write_uint32(s, 0x61746164);
  write_uint32(s, data_size);

  // Convert the sound to a buffer.
  // TODO(m): Add support for big endian machines.
  std::vector<int16_t> buffer(data_size);
  for (int k = 0; k < sound->num_samples(); ++k) {
    for (int ch = 0; ch < sound->num_channels(); ++ch) {
      buffer[k * sound->num_channels() + ch] = sound->channel(ch)[k];
    }
  }

  s.write(reinterpret_cast<char*>(&buffer[0]), data_size);
}

sound_t *load_sac(const std::string &file_name) {
  hires_time_t time;

  // Load the packed data.
  sac_packed_data_t *packed = sac_load_file(file_name.c_str());
  if (!packed) {
    return 0;
  }

  // Decode the sound.
  scoped_ptr<sound_t> sound(new sound_t(sac_get_num_samples(packed), sac_get_num_channels(packed), sac_get_sample_rate(packed)));
  time.push();
  for (int ch = 0; ch < sound->num_channels(); ++ch) {
    sac_decode_channel(sound->channel(ch), packed, 0, sound->num_samples(), ch);
  }
  double dt = time.pop_delta();
  std::cout << "Decoded SAC in " << (dt * 1000.0) << " ms.\n";

  // Free the packedsound.
  sac_free(packed);

  return sound.release();
}

void save_sac(const std::string &file_name, const sound_t *sound, sac_encoding_t encoding) {
  hires_time_t time;

  // Encode the sound.
  time.push();
  sac_packed_data_t *packed = sac_encode(sound->num_samples(), sound->num_channels(), sound->sample_rate(), encoding, sound->channels());
  if (!packed) {
    std::cerr << "Unable to encode the sound." << std::endl;
    return;
  }
  double dt = time.pop_delta();
  std::cout << "Encoded SAC in " << (dt * 1000.0) << " ms.\n";

  // Save the SAC format file.
  sac_save_file(file_name.c_str(), packed);

  // Free the encoded sound.
  sac_free(packed);
}

} // namespace tools
