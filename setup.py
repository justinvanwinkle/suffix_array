#!/usr/bin/env python
# -*- coding: utf-8 -*-

from distutils.core import setup, Extension
from glob import glob


if __name__ == '__main__':
    src_files = glob('src/*.c')
    extensions = [Extension("suffix_array",
                            src_files,
                            language='c')]
    setup(name='suffix_array',
          ext_modules=extensions)
