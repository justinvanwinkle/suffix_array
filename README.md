# Suffix Array

A fork of divsufsort. It does suffix arrays and related stuff.

    In [1]: s = 'ab' * 5000000

    In [2]: %timeit 'ababababababx' in s
    10 loops, best of 3: 28.3 ms per loop

    In [3]: import suffix_array

    In [4]: sa = suffix_array.SuffixArray(s)

    In [5]: %timeit sa.search('ababababababx')
    1000000 loops, best of 3: 467 ns per loop


## Setup for development:

    pip install -r requirements.txt
    python setup.py develop


## Testing:

    py.test
