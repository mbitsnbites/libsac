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

#include <libsac.h>

#include <iostream>
#include <string>

#include "file_io.h"
#include "scoped_ptr.h"
#include "sound.h"

int main(int argc, char** argv) {
  // Parse arguments.
  sac_encoding_t encoding = SAC_FORMAT_DD8A;
  std::string in_file;
  std::string out_file;
  bool bad_arg = false;
  for (int a = 1; a < argc; ++a) {
    std::string arg(argv[a]);
    if (arg == "-4") {
      encoding = SAC_FORMAT_DD4A;
    } else if (arg == "-8") {
      encoding = SAC_FORMAT_DD8A;
    } else if (arg[0] == '-') {
      std::cerr << "Invalid option: " << arg << std::endl;
      bad_arg = true;
      break;
    } else if (in_file.empty()) {
      in_file = arg;
    } else if (out_file.empty()) {
      out_file = arg;
    } else {
      std::cerr << "Too many arguments" << std::endl;
      bad_arg = true;
      break;
    }
  }

  // Show usage if necessary.
  if (bad_arg || in_file.empty() || out_file.empty()) {
    std::cout << "Usage: " << argv[0] << " [options] infile outfile" << std::endl;
    std::cout << std::endl;
    std::cout << " infile   The input file (either 16-bit PCM WAVE or SAC)" << std::endl;
    std::cout << " outfile  The output file (for WAVE input, the output is SAC, and vice versa)" << std::endl;
    std::cout << std::endl;
    std::cout << "Options (only used for SAC output):" << std::endl;
    std::cout << " -4       Use 4-bit DD4A encoding" << std::endl;
    std::cout << " -8       Use 8-bit DD8A encoding (default)" << std::endl;
    return 0;
  }

  tools::scoped_ptr<tools::sound_t> sound;

  // Try loading a WAVE input file.
  sound.reset(tools::load_wave(in_file));
  if (sound.get()) {
    // Save as SAC file.
    tools::save_sac(out_file, sound.get(), encoding);
    return 0;
  }

  // Try loading a SAC input file.
  sound.reset(tools::load_sac(in_file));
  if (sound.get()) {
    // Save as WAVE file.
    tools::save_wave(out_file, sound.get());
    return 0;
  }

  std::cerr << "Unable to load input file " << in_file << std::endl;

  return 0;
}
