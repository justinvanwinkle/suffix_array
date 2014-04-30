
CC = clang
CXX = clang++

PYTHON_INCLUDES=$(shell python-config --includes)
INCLUDE = $(PYTHON_INCLUDES)

FLAGS = -fno-strict-aliasing
FLAGS += -fno-common
FLAGS += -g
FLAGS += -O2
FLAGS += -Wall
FLAGS += -Wextra
FLAGS += -Wstrict-prototypes
FLAGS += -pipe
FLAGS += -pthread
FLAGS += -fwrapv
FLAGS += -pedantic
FLAGS += -ferror-limit=2
FLAGS += -MMD
#FLAGS += -fstandalone-debug

CYTHON = cython
CYTHON_FLAGS = -Wextra

ifeq ($(shell uname), Darwin)
LFLAGS += -bundle
LFLAGS += -undefined dynamic_lookup
LFLAGS += -arch x86_64
LFLAGS += -Wl,-F.
else
LFLAGS += -shared
LFLAGS += -pthread
# LFLAGS += -Wl,-O3
# LFLAGS += -Wl,-Bsymbolic-functions
# LFLAGS += -Wl,-z,relro
FLAGS += -fPIC
FLAGS += -fno-common
endif

CPPFLAGS = $(FLAGS)
CPPFLAGS += -std=c++11
CPPFLAGS += -stdlib=libc++

SRCS = src/flott/flott.c src/flott/flott_simple.c

%.d : %.c
	$(CC) $(CCFLAGS) -MF"$@" -MG -MM -MP -MT"$@" -MT"$(<:.c=.o)" "$<"

DEPS = $(SRCS:.c=.d)

include $(DEPS)

.PHONY: test

default: suffix_array.so

src/flott/%.o: src/flott/%.c
	$(CC) $(FLAGS) -c -o src/flott_all.o $(FLOTT_SRC)

src/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

src/suffix_array.cpp: cython_src/suffix_array.pyx cython_src/suffix_array.pxd
	cython $(CYTHON_FLAGS) --cplus cython_src/suffix_array.pyx -o src/suffix_array.cpp

suffix_array.so: src/suffix_array.o src/divsufsort.o src/flott/f%.o
	echo
	$(CXX) $(CPPFLAGS) $(LFLAGS) $(INCLUDE) src/suffix_array.o src/divsufsort.o -o suffix_array.so

test: suffix_array.so test/test_basics.py
	PYTHONPATH=./ py.test -- test/test_basics.py

a.out: src/test.cpp src/suffix_array.o src/divsufsort.o
	$(CXX) -std=c++11 -stdlib=libc++ src/divsufsort.cpp src/test.cpp -o a.out

install: suffix_array.so
	python setup.py install

clean:
	rm -rf suffix_array.so src/*.o src/flott/*.o build/ src/suffix_array.cpp

check-syntax:
	$(CXX) ${CPPFLAGS} -fsyntax-only -fno-color-diagnostics -Wall -Wextra $(INCLUDE) -pedantic -c ${CHK_SOURCES}
