
from setuptools import setup, Extension
from Cython.Build import cythonize


VERSION = '0.9.81'

if __name__ == '__main__':
    src_files = ['src/suffix_array.pyx']

    extensions = [Extension("suffix_array",
                            sources=src_files,
                            extra_compile_args=['-std=c++17',
                                                '-O2'])]
    description = "Fast Suffix Array for Python"
    setup(name='suffix_array',
          version=VERSION,
          author='Justin Van Winkle',
          author_email='justin.vanwinkle@gmail.com',
          url='https://github.com/justinvanwinkle/suffix_array',
          license="BSD",
          tests_require=["pytest"],
          setup_requires=["cython", "pytest-runner"],
          description=description,
          ext_modules=cythonize(extensions, nthreads=4))
