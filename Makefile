
CC = clang
CXX = clang++

PYTHON_INCLUDES=$(shell python-config --includes)
INCLUDE = $(PYTHON_INCLUDES)

CFLAGS = -fno-strict-aliasing
CFLAGS += -fno-common
CFLAGS += -g
CFLAGS += -O3
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wstrict-prototypes
CFLAGS += -pipe
CFLAGS += -pthread
CFLAGS += -fwrapv

CYTHON = cython
CYTHON_FLAGS = -Wextra

LFLAGS = -pedantic

ifeq ($(shell uname), Darwin)
LFLAGS += -bundle
LFLAGS += -undefined dynamic_lookup
LFLAGS += -arch x86_64
LFLAGS += -Wl,-F.
else
LFLAGS += -shared
LFLAGS += -pthread
LFLAGS += -Wl,-O3
LFLAGS += -Wl,-Bsymbolic-functions
LFLAGS += -Wl,-z,relro
CFLAGS += -fPIC
CFLAGS += -fno-common
endif

CPPFLAGS = $(CFLAGS)

CPPFLAGS += -std=c++11
CPPFLAGS += -stdlib=libc++

.PHONY: test

default: suffix_array.so

src/%.o: src/%.cpp src/*.hpp
	$(CXX) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

src/suffix_array.cpp: cython_src/suffix_array.pyx cython_src/suffix_array.pxd
	cython $(CYTHON_FLAGS) --cplus cython_src/suffix_array.pyx -o src/suffix_array.cpp

suffix_array.so: src/suffix_array.o src/divsufsort.o
	$(CXX) $(CPPFLAGS) $(LFLAGS) src/suffix_array.o src/divsufsort.o -o suffix_array.so

test: suffix_array.so test/test_basics.py
	py.test -- test/test_basics.py

a.out: src/test.cpp src/suffix_array.o src/divsufsort.o
	$(CXX) $(CPPFLAGS) -c src/test.cpp -o src/test.o
	$(CXX) src/divsufsort.o src/test.o

install: suffix_array.so
	python setup.py install

clean:
	rm -rf suffix_array.so src/*.o build/ src/suffix_array.cpp

check-syntax:
	$(CXX) ${CPPFLAGS} -fsyntax-only -fno-color-diagnostics -Wall -Wextra $(INCLUDE) -pedantic -c ${CHK_SOURCES}
