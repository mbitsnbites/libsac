# libsac

This is a reference implementation of the Simple Audio Compression (SAC)
compression algorithms (4-bit and 8-bit DDPCM).

A simple conversion tool is included that enables conversion between SAC and
WAVE format files.

The library is written in C++, but the API is plain C.


## IMPORTANT!

This is work-in-progress code! The file and data formats as well as the
library API will most likely change!


## Building

To build the library, run `make` from the lib folder.

To build to conversion tool, run `make` from the tools folder.


## A note on OpenMP

The default is for libsac to use OpenMP, which requires that you enable OpenMP
when linking to the library (e.g. using `-fopenmp` for gcc/g++), otherwise you
may get linking errors such as `undefined reference to 'omp_get_num_threads'`.

To disable the use of OpenMP, build the library using `make USE_OPENMP=NO`.

Please note that encoding will typically be much slower when OpenMP is
disabled.


## License

This library is released under the zlib/libpng license:

```
 Copyright (c) 2014 Marcus Geelnard

 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not
    be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
```

