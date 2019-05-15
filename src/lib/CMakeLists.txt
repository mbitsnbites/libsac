# -*- Mode: cmake; tab-width: 2; indent-tabs-mode: nil -*-
#-----------------------------------------------------------------------------
# Copyright (c) 2019 Marcus Geelnard
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
#     1. The origin of this software must not be misrepresented; you must not
#     claim that you wrote the original software. If you use this software
#     in a product, an acknowledgment in the product documentation would be
#     appreciated but is not required.
#
#     2. Altered source versions must be plainly marked as such, and must not
#     be misrepresented as being the original software.
#
#     3. This notice may not be removed or altered from any source
#     distribution.
#-----------------------------------------------------------------------------

set(LIBSAC_SRC
    saver.cpp
    loader.cpp
    encoder/encode.cpp
    encoder/encode_dd4a.cpp
    encoder/analyzer.cpp
    encoder/encode_dd8a.cpp
    packed_data.cpp
    decoder/decode_dd8a.cpp
    decoder/decode_dd4a.cpp
    decoder/decode.cpp
    quant_lut_dd4a.cpp
    quant_lut_dd8a.cpp
   )

add_library(libsac ${LIBSAC_SRC})
target_include_directories(libsac PRIVATE .)
target_include_directories(libsac PUBLIC ../include)

# We use OpenMP whenever we can.
find_package(OpenMP)
if(OPENMP_FOUND)
  target_link_libraries(libsac PUBLIC OpenMP::OpenMP_CXX)
  target_compile_definitions(libsac PRIVATE LIBSAC_USE_OPENMP)
endif()

