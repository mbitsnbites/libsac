# -*- Mode: makefile; tab-width: 4; indent-tabs-mode: t -*-
#-----------------------------------------------------------------------------
# Copyright (c) 2014 Marcus Geelnard
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

# Note: To cross compile for Windows on a Linux host:
#  1. Install mingw-w64 (Ubuntu: sudo apt-get install mingw-w64).
#  2. Create a dedicated output directoru, e.g. out_win64.
#  3. Run make with PREFIX and OUTDIR set appropriately, e.g:
#      make PREFIX=x86_64-w64-mingw32- OUTDIR=out_win64

PREFIX     ?=
OUTDIR     ?= out
USE_OPENMP ?= YES

CXX      = $(PREFIX)g++
CXXFLAGS = -c -O3 -funroll-loops -W -Wall -Wextra -I. -I../include
AR       = $(PREFIX)ar
ARFLAGS  = -rsc

# Enable OpenMP?
ifeq ($(USE_OPENMP), YES)
CXXFLAGS += -fopenmp -DLIBSAC_USE_OPENMP
endif

# Automatically generate dependency files
CXXFLAGS += -MMD -MP

OBJS = $(OUTDIR)/analyzer.o \
       $(OUTDIR)/decode.o \
       $(OUTDIR)/decode_dd4a.o \
       $(OUTDIR)/decode_dd8a.o \
       $(OUTDIR)/encode.o \
       $(OUTDIR)/encode_dd4a.o \
       $(OUTDIR)/encode_dd8a.o \
       $(OUTDIR)/loader.o \
       $(OUTDIR)/packed_data.o \
       $(OUTDIR)/saver.o \
       $(OUTDIR)/quant_lut_dd4a.o \
       $(OUTDIR)/quant_lut_dd8a.o

LIB = $(OUTDIR)/libsac.a

.PHONY: all clean

all: $(LIB)

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) $(LIB)

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $(LIB) $(OBJS)

$(OUTDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

$(OUTDIR)/%.o: decoder/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

$(OUTDIR)/%.o: encoder/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# These dependency files are generated automatically when building the object
# files.
-include $(OBJS:.o=.d)

