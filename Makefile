
CC = clang
CXX = clang++

PYTHON_INCLUDES=$(shell python-config --includes)
INCLUDE = $(PYTHON_INCLUDES)

#FLAGS = -fno-strict-aliasing
#FLAGS += -fno-common
FLAGS = -g
FLAGS += -O3
FLAGS += -Wall
FLAGS += -Wextra
FLAGS += -Wstrict-prototypes
FLAGS += -pipe
FLAGS += -pedantic
FLAGS += -ferror-limit=2

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
CPPFLAGS += -Wc++11-long-long
#CPPFLAGS += --analyze
#CPPFLAGS += -fsanitize=address

.PHONY: test

default: suffix_array.so

src/%.o: src/%.cpp src/*.hpp
	$(CXX) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

src/suffix_array.cpp: src/suffix_array.pyx src/suffix_array.pxd
	cython $(CYTHON_FLAGS) $< -o $@

suffix_array.so: src/suffix_array.o src/divsufsort.o
	$(CXX) $(CPPFLAGS) $(LFLAGS) $(INCLUDE) $^ -o $@

test: suffix_array.so test/test_basics.py
	PYTHONPATH=./ py.test -s -- test/test_basics.py

install: suffix_array.so
	python setup.py install

clean:
	rm -rf suffix_array.so src/*.o build/ src/suffix_array.cpp
