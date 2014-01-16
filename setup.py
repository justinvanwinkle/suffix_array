#!/usr/bin/env python
# -*- coding: utf-8 -*-

from glob import glob

from setuptools import setup
from distutils.extension import Extension

from Cython.Distutils import build_ext


if __name__ == '__main__':
    src_files = ["src/suffix_array.pyx"] + glob('src/*.c')
    extensions = [Extension("suffix_array",
                            src_files,
                            language='c')]
    setup(name='suffix_array',
          cmdclass={'build_ext': build_ext},
          ext_modules=extensions)
