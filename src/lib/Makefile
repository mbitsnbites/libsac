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

OUTDIR     ?= out
USE_OPENMP ?= YES

CPP      = g++
CPPFLAGS = -c -O3 -funroll-loops -W -Wall -Wextra
AR       = ar
ARFLAGS  = -rsc

# Enable OpenMP?
ifeq ($(USE_OPENMP), YES)
CPPFLAGS += -fopenmp -DLIBSAC_USE_OPENMP
endif

# Automatically generate dependency files
CPPFLAGS += -MMD -MP

OBJS = $(OUTDIR)/decode.o \
       $(OUTDIR)/decode_dd4a.o \
       $(OUTDIR)/decode_dd8a.o \
       $(OUTDIR)/encode.o \
       $(OUTDIR)/encode_dd4a.o \
       $(OUTDIR)/encode_dd8a.o \
       $(OUTDIR)/loader.o \
       $(OUTDIR)/packed_data.o \
       $(OUTDIR)/saver.o \
       $(OUTDIR)/util.o \
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
	$(CPP) $(CPPFLAGS) -o $@ $<

# These dependency files are generated automatically when building the object
# files.
-include $(OBJS:.o=.d)

