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

#ifndef TOOLS_FILE_IO_H_
#define TOOLS_FILE_IO_H_

#include <string>

#include <libsac.h>

namespace tools {

class sound_t;

sound_t *load_wave(const std::string& file_name);
void save_wave(const std::string &file_name, const sound_t *sound);

sound_t *load_sac(const std::string &file_name);
void save_sac(const std::string &file_name, const sound_t *sound, sac_encoding_t encoding);

} // namespace tools

#endif // TOOLS_FILE_IO_H_
