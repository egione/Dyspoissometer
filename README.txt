/*
Dyspoissometer
Copyright 2016-2017 Russell Leidich
http://dyspoissonism.blogspot.com

This collection of files constitutes the Dyspoissometer Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The Dyspoissometer Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Dyspoissometer Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the Dyspoissometer Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
TURN ON WORD WRAP SO YOU CAN READ THIS FILE AND ALSO SOURCE CODE COMMENTS,
ALTHOUGH DOING SO WILL RUIN TEXT ALIGNMENT.

VERSION

Version info is provided in the files starting with "flag".

ABOUT

Build Cleanup
-------------
"make clean" will delete everything in the temp folder. (This must be done manually on Windows.)

Birthday Tool
-------------
Measures the maximum index before repetition (MIBR) of a file with a given byte-granular mask size, in addition to its skew.

See also: http://dyspoissonism.blogspot.com/2016/06/mibr-randomness-metric-of-last-resort.html .

Type "make birthdaytool" to build.

Deltafy
-------
Computes discrete first and second derivatives and definite integrals of files in various formats. Used to lower entropy in order to enhance the sensitivity of Signal Sort.

Type "make deltafy" to build.

Dyspoissometer
--------------
This is a library (in the abstract sense) of functions which compute logfreedom, maximum and median logfreedom, dyspoissonism, information sparsity, random mask sets, and more. All the details are provided at the webpage linked above.

Type "make demo" inside this folder and follow the instructions from there. Alternatively, you can "make dyspoissometer_quad", "make dyspoissometer_double", or "make dyspoissometer_float" in order to produce temp/dyspoissometer.o(bj) at the indicated numerical precision with 64, 32, or 16 bits per integer, respectively. Float precision is the fastest and least accurate, suitable for data sets with less than 64Ki masks.

Dyspoissometer for Files
------------------------
Computes various statistical metrics of a file.

Type "make dyspoissofile" to build.

Signal Sort
-----------
Sorts files consisting of fixed-sized data frames by their dyspoissonism. It's particularly useful for finding deeply buried signals.

Type "make signalsort_quad", "make signalsort_double" or "make signalsort_float" to build quad, double, or float precision, with 64, 32, or 16 bits per integer, respectively.

LANGUAGES

Currently only C (adhering to C11) (dyspoissometer.c) and Mathematica (mathematica.txt) are supported, but contact us if you want to port it.

TESTED ENVIRONMENTS

Linux 32/64-bit
Mac 64-bit
Windows MinGW and MinGW-w64 32-bit

UNTESTED ENVIRONMENTS (TELL US)

ARM 32/64-bit
Windows MinGW-w64 64-bit

GETTING STARTED

Linux
-----
You just need a C compiler (either "gcc" or something aliased to "cc") and "make", both of which are probably installed by default.

Mac
---
Similar to Linux, but you'll probably be using the clang C compiler as opposed to GCC.

Windows
-------
MinGW and MinGW-w64 are the only supported Windows compilers, although porting should be easy.

For its part, MinGW-w64 (which also compiles 32-bit code) seems to require MAKE from MinGW's MSYS package, so the following Path advice applies to both compilers:

MinGW and MinGW-w64 are frustating and unintuitive. For one thing, you need to change your Path variable at Start -> Computer -> Right Click -> Properties -> Advanced system settings -> Environment Variables -> System variabes -> Path, in order to append your_mingw_folder\bin and your_mingw_folder\msys\[version]\bin (and for that matter, your_nasm_folder) . Then relauch the terminal window (Start -> Run -> cmd).
