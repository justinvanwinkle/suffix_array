# Suffix Array

A fork of divsufsort. It does suffix arrays and related stuff.

    In [1]: s = 'ab' * 1000000000

    In [2]: %timeit 'abababx' in s
    1 loops, best of 3: 5.5 s per loop

    In [3]: import suffix_array

    In [4]: %time sa = suffix_array.SuffixArray(s)
    CPU times: user 19.6 s, sys: 1.79 s, total: 21.3 s
    Wall time: 21.4 s

    In [5]: %timeit 'abababx' in sa
    1000000 loops, best of 3: 540 ns per loop

    In [6]: %time s.count('ab')
    CPU times: user 2.95 s, sys: 579 µs, total: 2.95 s
    Wall time: 2.95 s
    Out[6]: 1000000000

    In [7]: %time sa.count('ab')
    CPU times: user 15 µs, sys: 1 µs, total: 16 µs
    Wall time: 20 µs
    Out[7]: 1000000000


## Setup for development:

    pip install -r requirements.txt
    python setup.py develop


## Testing:

    py.test
