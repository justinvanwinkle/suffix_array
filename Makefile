OS := $(shell uname)

PYTHON_INCLUDES=$(shell python-config --includes)
INCLUDE = $(PYTHON_INCLUDES)

CFLAGS = -fno-strict-aliasing
CFLAGS += -fno-common
CFLAGS += -dynamic
CFLAGS += -g
CFLAGS += -Os
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes
CFLAGS += -pipe

CYTHON = cython


CYTHON_DIRECTIVES := boundscheck=False,infer_types=True,overflowcheck=False,nonecheck=False,wraparound=False

CYTHON_FLAGS = -X $(CYTHON_DIRECTIVES) -Wextra
CC = gcc

LFLAGS = -fno-strict-aliasing -Wall -Os -undefined dynamic_lookup

ifeq ($(OS), Darwin)
LFLAGS += -bundle
endif

LINK =  $(LFLAGS) -Wl,-F. src/*.o -o suffix_array.so

.PHONY: test

default: suffix_array.so

src/%.o: src/%.c Makefile src/divsufsort_private.h src/divsufsort.h
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

src/suffix_array.c: src/suffix_array.pyx src/suffix_array.pxd
	cython $(CYTHON_FLAGS) src/suffix_array.pyx

suffix_array.so: src/suffix_array.o src/trsort.o src/bwt.o src/divsufsort.o src/sssort.o
	$(CC) $(LINK)

test: suffix_array.so
	py.test -- test/test_basics.py

clean:
	rm -f suffix_array.so src/suffix_array.c src/*.o

check-syntax:
	cc -fsyntax-only $(CFLAGS) $(INCLUDE) -pedantic -c ${CHK_SOURCES}
