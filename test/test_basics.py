from os.path import join as pathjoin
from glob import glob

from suffix_array import SuffixArray
from suffix_array import LCP
from suffix_array import rstr_max


def test_basic_sort():
    assert SuffixArray('abcdefg') == range(7)
    assert SuffixArray('gfedcba') == range(6, -1, -1)
    assert list(SuffixArray('banana')) == [5, 3, 1, 0, 4, 2]


def test_find():
    s = 'aaabbbcccddd'
    assert SuffixArray(s).find('a') == 0
    assert SuffixArray(s).find('aaa') == 0
    assert SuffixArray(s).find('ab') == 2
    assert SuffixArray(s).find('dddd') == -1
    assert SuffixArray(s).find('') == 0


def test_count():
    for i in range(100):
        s = 'ab' * i
        assert SuffixArray(s).count('ab') == i

    for i in range(100):
        s = 'abxx' * i
        assert SuffixArray(s).count('ab') == i

    assert SuffixArray('aabbxxxxyyyy').count('xxx') == 2
    assert SuffixArray('aabbxxxyyyyxxxdfdjk').count('xxx') == 2


def test___contains__():
    sa = SuffixArray('ab')
    assert ('a' in sa) is True
    assert ('b' in sa) is True
    assert ('c' in sa) is False


def test_lcp_array():
    assert LCP(SuffixArray('abcdefg')) == [-1] + [0] * 6
    assert LCP(SuffixArray('banana')) == [-1, 1, 3, 0, 0, 2]


def test_rstr_max():
    s1 = '1yyyy0'
    s2 = '2yyyy3yyyy4'

    result = rstr_max([s1, s2])
    print result
    assert result == (4, (1, 6))


def _read_files(path):
    fns = glob(pathjoin(path, '*'))
    content = []
    for fn in fns:
        content.append(open(fn).read())
    return content


def test_rstr_max_big():
    ss = _read_files('test/data/html')

    best = rstr_max(ss)
    assert best == (27661, (6831, 7495, 6739, 6574, 6932, 6831, 6379, 6794, 7176, 6920, 7007, 6921, 6561, 7742, 6757, 6843, 6508, 6761, 6815))
