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


CYTHON_FLAGS = -Wextra
CC = cc

LFLAGS = -fno-strict-aliasing -Wall -Wextra -pedantic -Os -undefined dynamic_lookup

ifeq ($(OS), Darwin)
LFLAGS += -bundle
endif

LINK =  $(LFLAGS) -Wl,-F. src/*.o -o suffix_array.so

.PHONY: test

default: suffix_array.so

src/%.o: src/%.c Makefile src/divsufsort.h
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

src/suffix_array.c: cython_src/suffix_array.pyx cython_src/suffix_array.pxd
	cython $(CYTHON_FLAGS) cython_src/suffix_array.pyx -o src/suffix_array.c

suffix_array.so: src/suffix_array.o src/divsufsort.o
	$(CC) $(LINK)

test: suffix_array.so test/test_basics.py
	py.test -- test/test_basics.py

install: suffix_array.so
	python setup.py install

clean:
	rm -rf suffix_array.so src/*.o build/

check-syntax:
	cc -fsyntax-only $(CFLAGS) $(INCLUDE) -pedantic -c ${CHK_SOURCES}
