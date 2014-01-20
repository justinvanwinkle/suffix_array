#!/usr/bin/env python
# -*- coding: utf-8 -*-

from distutils.core import setup, Extension
from glob import glob

VERSION = '0.9.13'

if __name__ == '__main__':
    src_files = glob('src/*.c')
    extensions = [Extension("suffix_array",
                            sources=src_files,
                            language='c')]
    description = "Fast Suffix Array for Python"
    setup(name='suffix_array',
          version=VERSION,
          author='Justin Van Winkle',
          author_email='justin.vanwinkle@gmail.com',
          url='https://github.com/justinvanwinkle/suffix_array',
          license="BSD",
          description=description,
          ext_modules=extensions)
