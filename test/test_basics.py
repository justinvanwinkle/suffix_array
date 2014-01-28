from os.path import join as pathjoin
from glob import glob

#from suffix_array import SuffixArray
#from suffix_array import make_lcp
from suffix_array import rstr_max


# def test_basic_sort():
#     assert SuffixArray('abcdefg') == range(7)
#     assert SuffixArray('gfedcba') == range(6, -1, -1)
#     assert list(SuffixArray('banana')) == [5, 3, 1, 0, 4, 2]


# def test_find():
#     s = 'aaabbbcccddd'
#     assert SuffixArray(s).find('a') == 0
#     assert SuffixArray(s).find('aaa') == 0
#     assert SuffixArray(s).find('ab') == 2
#     assert SuffixArray(s).find('dddd') == -1
#     assert SuffixArray(s).find('') == 0


# def test_count():
#     for i in range(100):
#         s = 'ab' * i
#         assert SuffixArray(s).count('ab') == i

#     for i in range(100):
#         s = 'abxx' * i
#         assert SuffixArray(s).count('ab') == i

#     assert SuffixArray('aabbxxxxyyyy').count('xxx') == 2
#     assert SuffixArray('aabbxxxyyyyxxxdfdjk').count('xxx') == 2


# def test___contains__():
#     sa = SuffixArray('ab')
#     assert ('a' in sa) is True
#     assert ('b' in sa) is True
#     assert ('c' in sa) is False


# def test_lcp_array():
#     assert LCP(SuffixArray('abcdefg')) == [-1] + [0] * 6
#     assert LCP(SuffixArray('banana')) == [-1, 1, 3, 0, 0, 2]


def test_rstr_max():
    s1 = '1yyyy0'
    s2 = '2yyyy3yyyy4'

    result = rstr_max([s1, s2])
    print result
    assert result == (4, (1, 1))


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


def test_rstr_max_logic():
    l = ['1368226","dpci":"246-02-0788',
         '4104910","dpci":"068-07-0065',
         '3350721","dpci":"248-63-1547',
         '1843493","dpci":"246-04-5372',
         '3247206","dpci":"248-60-0627',
         '4255101","dpci":"243-30-6501',
         '4375553","dpci":"040-03-0872',
         '3567775","dpci":"070-05-0341',
         '4170911","dpci":"086-04-0259',
         '4307328","dpci":"248-81-4755',
         '2237077","dpci":"246-05-1835',
         '4358735","dpci":"074-00-0030',
         '3775398","dpci":"244-22-3012',
         '3763893","dpci":"242-17-5065',
         '2870702","dpci":"248-52-0775',
         '3221691","dpci":"244-20-6656',
         '1354607","dpci":"244-00-6031',
         '4077605","dpci":"060-03-0312',
         '3804229","dpci":"244-22-4137',
         '3890157","dpci":"248-78-1622',
         '1691132","dpci":"246-04-3258',
         '4420449","dpci":"026-01-1827',
         '1567749","dpci":"248-09-2062',
         '4362542","dpci":"241-27-3133',
         '3715382","dpci":"248-73-0576',
         '4157298","dpci":"243-29-1957',
         '3104684","dpci":"243-19-8713']
    for _ in xrange(10000):
        length, offsets = rstr_max(l, 3)
        assert length == 10
        assert offsets == tuple([7] * 27)
