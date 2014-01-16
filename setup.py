#!/usr/bin/env python
# -*- coding: utf-8 -*-

from distutils.core import setup, Extension
from glob import glob

setup(name='foo',
      version='1.0',
      ext_modules=[Extension('foo', ['foo.c'])],
      )


from Cython.Distutils import build_ext


if __name__ == '__main__':
    src_files = glob('src/*.c')
    extensions = [Extension("suffix_array",
                            src_files,
                            language='c')]
    setup(name='suffix_array',
          cmdclass={'build_ext': build_ext},
          ext_modules=extensions)
