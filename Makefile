OS := $(shell uname)

PYTHON_INCLUDES=$(shell python-config --includes)
INCLUDE = $(PYTHON_INCLUDES)

CFLAGS = -fno-strict-aliasing
CFLAGS += -fno-common
CFLAGS += -dynamic
CFLAGS += -g
CFLAGS += -O3
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes
CFLAGS += -pipe

CPPFLAGS = $(CFLAGS)
CPPFLAGS += -std=c++11
CPPFLAGS += -stdlib=libc++

CYTHON = cython


CYTHON_FLAGS = -Wextra
CC = cc
CPP = clang++

LFLAGS = -fno-strict-aliasing -Wall -Wextra -pedantic -O3 -undefined dynamic_lookup

ifeq ($(OS), Darwin)
LFLAGS += -bundle
endif

LINK =  $(LFLAGS) -Wl,-F. src/*.o -o suffix_array.so

.PHONY: test

default: suffix_array.so

src/%.o: src/%.c Makefile src/divsufsort.h
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

src/%.o: src/%.cpp
	$(CPP) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

src/suffix_array.cpp: cython_src/suffix_array.pyx cython_src/suffix_array.pxd
	cython $(CYTHON_FLAGS) --cplus cython_src/suffix_array.pyx -o src/suffix_array.cpp

suffix_array.so: src/suffix_array.o src/divsufsort.o src/*
	$(CC) $(LINK)

test: suffix_array.so test/test_basics.py
	py.test -- test/test_basics.py

install: suffix_array.so
	python setup.py install

clean:
	rm -rf suffix_array.so src/*.o build/ src/suffix_array.cpp

check-syntax:
	cc -fsyntax-only -fno-color-diagnostics -Wall -Wextra $(INCLUDE) -pedantic -c ${CHK_SOURCES}
