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
    for i in range(1000):
        s = 'ab' * i
        assert SuffixArray(s).count('ab') == i

    for i in range(1000):
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

    result = rstr_max(s1, s2)
    print result
    assert result == [[(4, 1, 0), (4, 1, 1), (4, 6, 1)]]


def test_rstr_max_big():
    s1 = open('test/data/html/0c314').read()
    s2 = open('test/data/html/1bf1b').read()
    s3 = open('test/data/html/e1038').read()
    s4 = open('test/data/html/ef4ba').read()

    length = len(rstr_max(s1, s2, s3, s4))
    assert length == 26511
