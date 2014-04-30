#!/usr/bin/env python
# -*- coding: utf-8 -*-

from glob import glob

from distutils.core import setup, Extension
from Cython.Build import cythonize


VERSION = '0.9.81'

if __name__ == '__main__':
    src_files = (['src/suffix_array.pyx',
                  'src/divsufsort.cpp'] +
                 glob('src/flott/*.cpp'))
    extensions = [Extension("suffix_array",
                            sources=src_files,
                            extra_compile_args=['-std=c++11',
                                                '-Wc++11-long-long',
                                                '-O3'])]
    description = "Fast Suffix Array for Python"
    setup(name='suffix_array',
          version=VERSION,
          author='Justin Van Winkle',
          author_email='justin.vanwinkle@gmail.com',
          url='https://github.com/justinvanwinkle/suffix_array',
          license="BSD",
          description=description,
          ext_modules=cythonize(extensions))
