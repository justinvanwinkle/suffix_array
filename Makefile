
CC = clang-7
CXX = clang++-7

PYTHON_INCLUDES=$(shell python3.7-config --includes)
INCLUDE = $(PYTHON_INCLUDES)

#FLAGS = -fno-strict-aliasing
#FLAGS += -fno-common
FLAGS = -g
FLAGS += -O3
FLAGS += -Wall
FLAGS += -Wextra
FLAGS += -pipe
FLAGS += -pedantic
FLAGS += -ferror-limit=2
FLAGS += -Wunused-command-line-argument
#FLAGS += -fprofile-use
#FLAGS += -fprofile-generate
#FLAGS += -fprofile-dir=/tmp/gcc-prof

CYTHON = cython
CYTHON_FLAGS = -Wextra
CYTHON_FLAGS += --cplus
CYTHON_FLAGS += -3


ifeq ($(shell uname), Darwin)
LFLAGS += -bundle
LFLAGS += -undefined dynamic_lookup
LFLAGS += -arch x86_64
LFLAGS += -Wl,-F.
else
LFLAGS += -shared
LFLAGS += -g
LFLAGS += -O3
#LFLAGS += -pthread
LFLAGS += -Wl,-O0
# LFLAGS += -Wl,-Bsymbolic-functions
# LFLAGS += -Wl,-z,relro
FLAGS += -fPIC
endif

CPPFLAGS = $(FLAGS)
CPPFLAGS += -std=c++2a
#CPPFLAGS += -stdlib=libc++
#CPPFLAGS += --analyze

#LFLAGS += -lasan
#LFLAGS += -lubsan
#CPPFLAGS += -fsanitize=address
#CPPFLAGS += -O0
#CPPFLAGS += -fno-omit-frame-pointer
#CYTHON_FLAGS += --gdb
#2CYTHON_FLAGS += --gdb-outdir .


.PHONY: test

default: suffix_array.so

src/%.o: src/%.cpp src/*.hpp Makefile
	$(CXX) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

src/suffix_array.cpp: src/suffix_array.pyx src/suffix_array.pxd
	cython $(CYTHON_FLAGS) $< -o $@

suffix_array.so: src/suffix_array.o
	$(shell mkdir -p build)
	$(CXX) $(CPPFLAGS) $(LFLAGS) $(INCLUDE) $^ -o $@

test: suffix_array.so test/test_basics.py
	PYTHONPATH=./build/ py.test -s -- test/test_basics.py

install: suffix_array.so
	cp suffix_array.so $(shell python -c 'import suffix_array; print(suffix_array.__file__)')

clean:
	rm -rf suffix_array.so src/*.o build/ src/suffix_array.cpp

analyze:
	/opt/local/libexec/llvm-3.5/libexec/scan-build/scan-build -enable-checker alpha -analyze-headers -V make -j
