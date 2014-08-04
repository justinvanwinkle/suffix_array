
CC = clang-mp-3.5
CXX = clang++-mp-3.5

PYTHON_INCLUDES=$(shell python-config --includes)
INCLUDE = $(PYTHON_INCLUDES)

#FLAGS = -fno-strict-aliasing
#FLAGS += -fno-common
FLAGS = -g
FLAGS += -O2
FLAGS += -Wall
FLAGS += -Wextra
FLAGS += -pipe
FLAGS += -pedantic
FLAGS += -ferror-limit=2
#FLAGS += -fprofile-use
#FLAGS += -fprofile-generate
#FLAGS += -fprofile-dir=/tmp/gcc-prof

CYTHON = cython
CYTHON_FLAGS = -Wextra
CYTHON_FLAGS += --cplus

ifeq ($(shell uname), Darwin)
LFLAGS += -bundle
LFLAGS += -undefined dynamic_lookup
LFLAGS += -arch x86_64
LFLAGS += -Wl,-F.
else
LFLAGS += -shared
#LFLAGS += -pthread
# LFLAGS += -Wl,-O3
# LFLAGS += -Wl,-Bsymbolic-functions
# LFLAGS += -Wl,-z,relro
FLAGS += -fPIC
endif

CPPFLAGS = $(FLAGS)
CPPFLAGS += -std=c++11
CPPFLAGS += -stdlib=libc++
#CPPFLAGS += --analyze
#CPPFLAGS += -fsanitize=address

.PHONY: test

default: build/suffix_array.so

src/%.o: src/%.cpp src/*.hpp Makefile
	$(CXX) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

src/suffix_array.cpp: src/suffix_array.pyx src/suffix_array.pxd
	cython $(CYTHON_FLAGS) $< -o $@

build/suffix_array.so: src/suffix_array.o src/divsufsort.o
	$(shell mkdir build)
	$(CXX) $(CPPFLAGS) $(LFLAGS) $(INCLUDE) $^ -o $@

test: build/suffix_array.so test/test_basics.py
	PYTHONPATH=./build/ py.test -s -- test/test_basics.py

install: build/suffix_array.so
	cp build/suffix_array.so $(shell python -c 'import suffix_array; print suffix_array.__file__')

clean:
	rm -rf src/*.o build/ src/suffix_array.cpp

analyze:
	/opt/local/libexec/llvm-3.5/libexec/scan-build/scan-build -enable-checker alpha -analyze-headers -V make -j
