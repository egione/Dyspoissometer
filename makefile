BASICFLAGS=-D_FILE_OFFSET_BITS=64 -fno-stack-protector -mtune=native
BITS=32
CC=cc
DELTEMP=rm temp$(SLASH)*
EXE=
NASM=
OBJ=.o
OPTIMIZEFLAGS=-O3
SLASH=/
TARGET=$(shell cc -dumpmachine)
WARNFLAGS=-Wall -Wextra -Wconversion -Wint-conversion
ifeq (,$(TARGET))
	CC=gcc
	OPTIMIZEFLAGS=-s -O3
	TARGET=$(shell gcc -dumpmachine)
endif
ifneq (,$(findstring 64,$(TARGET)))
	BITS=64
endif
ifneq (,$(findstring mingw,$(TARGET)))
ifneq (,$(findstring mingw32,$(TARGET)))
	BITS=32
endif
	DELTEMP=@echo clean does not work due to idiotic problems with MAKE under MSYS, but deleting everything in the temp subfolder accomplishes the same thing.
	EXE=.exe
	OBJ=.obj
	SLASH=\\
endif
ifneq (,$(findstring darwin,$(TARGET)))
	WARNFLAGS+= -Wconstant-conversion
else
ifneq (,$(findstring arm,$(TARGET)))
	WARNFLAGS+= -Wconstant-conversion
endif
endif

ascii:
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG_OFF $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)ascii$(OBJ) ascii.c

birthdaytool:
	@echo Use birthdaytool_float, birthdaytool_double, or birthdaytool_quad for
	@echo single, double, or quad precision, respectively. Quad precision is not
	@echo supported by all compilers.

birthdaytool_double:
	make ascii
	make debug
	make dyspoissometer_double
	$(CC) -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_DOUBLE $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)ascii$(OBJ) temp$(SLASH)debug$(OBJ) temp$(SLASH)dyspoissometer$(OBJ) -otemp$(SLASH)birthdaytool$(EXE) birthdaytool.c -lm
	@echo You can now run \"temp$(SLASH)birthdaytool\".

birthdaytool_float:
	make ascii
	make debug
	make dyspoissometer_float
	$(CC) -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_FLOAT $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)ascii$(OBJ) temp$(SLASH)debug$(OBJ) temp$(SLASH)dyspoissometer$(OBJ) -otemp$(SLASH)birthdaytool$(EXE) birthdaytool.c -lm
	@echo You can now run \"temp$(SLASH)birthdaytool\".

birthdaytool_quad:
	make ascii
	make debug
	make debug_quad
	make dyspoissometer_quad
	$(CC) -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_QUAD $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)ascii$(OBJ) temp$(SLASH)debug$(OBJ) temp$(SLASH)debug_quad$(OBJ) temp$(SLASH)dyspoissometer$(OBJ) -otemp$(SLASH)birthdaytool$(EXE) birthdaytool.c -lm -lquadmath
	@echo You can now run \"temp$(SLASH)birthdaytool\".

clean:
	$(DELTEMP)

debug:
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)debug$(OBJ) debug.c

debug_quad:
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)debug_quad$(OBJ) debug_quad.c

deltafy:
	make ascii
	make debug
	$(CC) -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_QUAD $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)ascii$(OBJ) temp$(SLASH)debug$(OBJ) -otemp$(SLASH)deltafy$(EXE) deltafy.c -lm
	@echo You can now run \"temp$(SLASH)deltafy\".

dyspoissometer_double:
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG_OFF -DDYSPOISSOMETER_NUMBER_DOUBLE $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)dyspoissometer$(OBJ) dyspoissometer.c

dyspoissometer_double_debug:
	make debug
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_DOUBLE $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)dyspoissometer$(OBJ) dyspoissometer.c

dyspoissometer_float:
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG_OFF -DDYSPOISSOMETER_NUMBER_FLOAT $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)dyspoissometer$(OBJ) dyspoissometer.c

dyspoissometer_float_debug:
	make debug
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_FLOAT $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)dyspoissometer$(OBJ) dyspoissometer.c

dyspoissometer_quad:
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG_OFF -DDYSPOISSOMETER_NUMBER_QUAD $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)dyspoissometer$(OBJ) dyspoissometer.c

dyspoissometer_quad_debug:
	make debug
	make debug_quad
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_QUAD $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)dyspoissometer$(OBJ) dyspoissometer.c

demo:
	@echo Try \"make demo_double\" for double precision or \"demo_quad\" for quad precision.
	@echo Learn how the demo works by following along with main\(\) in demo.c. All the
	@echo build magic resides in makefile, flag\*.h, and dyspoissometer.h. To create an
	@echo object file which can be called from external code using the interfaces
	@echo provided in dyspoissometer_xtrn.h, do:
	@echo \"make dyspoissometer_\(float\/double\/quad\)\[_debug\]\". In other words,
	@echo specify the numerical precision and whether or not to include and enable the
	@echo debugger. \"make clean\" will delete the contents of the temp folder, which is
	@echo where all object files and executables are built. Beware that the build process
	@echo overwrites them automatically.
	@echo
	@echo Quad precision is not supported by all compilers.
	@echo
	@echo In Windows, you may get some compilation warnings about cc or fpic. Ignore them.

demo_double:
	make dyspoissometer_double_debug
	$(CC) -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_DOUBLE $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)debug$(OBJ) temp$(SLASH)dyspoissometer$(OBJ) -otemp$(SLASH)demo$(EXE) demo.c -lm
	@echo
	@echo You can now run \"temp$(SLASH)demo\". Capture to a file if it scrolls too fast.

demo_float:
	make dyspoissometer_float_debug
	$(CC) -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_FLOAT $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)debug$(OBJ) temp$(SLASH)dyspoissometer$(OBJ) -otemp$(SLASH)demo$(EXE) demo.c -lm
	@echo
	@echo You can now run \"temp$(SLASH)demo\". Capture to a file if it scrolls too fast.

demo_quad:
	make dyspoissometer_quad_debug
	$(CC) -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_QUAD $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)debug$(OBJ) temp$(SLASH)debug_quad$(OBJ) temp$(SLASH)dyspoissometer$(OBJ) -otemp$(SLASH)demo$(EXE) demo.c -lm -lquadmath
	@echo You can now run \"temp$(SLASH)demo\". Capture to a file if it scrolls too fast.

dyspoissofile:
	@echo Use dyspoissofile_float, dyspoissofile_double, or dyspoissofile_quad for
	@echo single, double, or quad precision, respectively. Quad precision is not
	@echo supported by all compilers.

dyspoissofile_double:
	make ascii
	make debug
	make dyspoissometer_double
	$(CC) -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_DOUBLE $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)ascii$(OBJ) temp$(SLASH)debug$(OBJ) temp$(SLASH)dyspoissometer$(OBJ) -otemp$(SLASH)dyspoissofile$(EXE) dyspoissofile.c -lm
	@echo You can now run \"temp$(SLASH)dyspoissofile\".

dyspoissofile_float:
	make ascii
	make debug
	make dyspoissometer_float
	$(CC) -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_FLOAT $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)ascii$(OBJ) temp$(SLASH)debug$(OBJ) temp$(SLASH)dyspoissometer$(OBJ) -otemp$(SLASH)dyspoissofile$(EXE) dyspoissofile.c -lm
	@echo You can now run \"temp$(SLASH)dyspoissofile\".

dyspoissofile_quad:
	make ascii
	make debug
	make debug_quad
	make dyspoissometer_quad
	$(CC) -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_QUAD $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)ascii$(OBJ) temp$(SLASH)debug$(OBJ) temp$(SLASH)debug_quad$(OBJ) temp$(SLASH)dyspoissometer$(OBJ) -otemp$(SLASH)dyspoissofile$(EXE) dyspoissofile.c -lm -lquadmath
	@echo You can now run \"temp$(SLASH)dyspoissofile\".

signalsort:
	@echo Use signalsort_float, signalsort_double, or signalsort_quad for single
	@echo double, or quad precision, respectively. Quad precision is not
	@echo supported by all compilers.

signalsort_double:
	make ascii
	make debug
	make dyspoissometer_double
	$(CC) -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_DOUBLE $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)ascii$(OBJ) temp$(SLASH)debug$(OBJ) temp$(SLASH)dyspoissometer$(OBJ) -otemp$(SLASH)signalsort$(EXE) signalsort.c -lm
	@echo You can now run \"temp$(SLASH)signalsort\".

signalsort_float:
	make ascii
	make debug
	make dyspoissometer_float
	$(CC) -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_FLOAT $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)ascii$(OBJ) temp$(SLASH)debug$(OBJ) temp$(SLASH)dyspoissometer$(OBJ) -otemp$(SLASH)signalsort$(EXE) signalsort.c -lm
	@echo You can now run \"temp$(SLASH)signalsort\".

signalsort_quad:
	make ascii
	make debug
	make debug_quad
	make dyspoissometer_quad
	$(CC) -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_QUAD $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)ascii$(OBJ) temp$(SLASH)debug$(OBJ) temp$(SLASH)debug_quad$(OBJ) temp$(SLASH)dyspoissometer$(OBJ) -otemp$(SLASH)signalsort$(EXE) signalsort.c -lm -lquadmath
	@echo You can now run \"temp$(SLASH)signalsort\".
