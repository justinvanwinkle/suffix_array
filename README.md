# Suffix Array

A fork of divsufsort. It does suffix arrays and related stuff.

```python
In [1]: s = 'ab' * 1000000000

In [2]: %timeit 'abababx' in s
1 loops, best of 3: 5.5 s per loop

In [3]: import suffix_array

In [4]: %time sa = suffix_array.SuffixArray(s)
CPU times: user 19.6 s, sys: 1.79 s, total: 21.3 s
Wall time: 21.4 s

In [5]: %timeit 'abababx' in sa
1000000 loops, best of 3: 540 ns per loop

In [6]: s.count('ab')
Out[6]: 1000000000

In [7]: sa.count('ab')
Out[7]: 1000000000

In [8]: %timeit s.count('ab')
1 loops, best of 3: 2.83 s per loop

In [9]: %timeit sa.count('ab')
1000000 loops, best of 3: 577 ns per loop

In [10]: s.count('x')
Out[10]: 0

In [11]: sa.count('x')
Out[11]: 0

In [12]: %timeit s.count('x')
1 loops, best of 3: 1.1 s per loop

In [13]: %timeit sa.count('x')
1000000 loops, best of 3: 309 ns per loop
```

This library counts overlapping matches, whereas string.count finds nonoverlapping.

```python
In [14]: s.count('ab' * 50)
Out[14]: 20000000

In [15]: sa.count('ab' * 50)
Out[15]: 999999951

In [16]: %timeit s.count('ab' * 50)
1 loops, best of 3: 1.32 s per loop

In [17]: %timeit sa.count('ab' * 50)
100000 loops, best of 3: 6.02 µs per loop
```

## Setup for development:

    pip install -r requirements.txt
    python setup.py develop


## Testing:

    py.test
