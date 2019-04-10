from os.path import join as pathjoin
from glob import glob
from itertools import permutations
import pytest

# from suffix_array import SuffixArray
# from suffix_array import make_lcp
from suffix_array import rstr_max
from suffix_array import all_repeats
from suffix_array import longest_common_substring


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


testwords = [b'bububub', b'xxx', b'a', b'test', b'feed', b'Ada']


@pytest.mark.parametrize('testword', testwords)
def test_longest_common_substring_x(testword):
    lcs = longest_common_substring([testword, testword])
    assert lcs == testword


@pytest.mark.parametrize('testword', testwords)
def test_longest_common_substring(testword):
    bub = testword

    test_texts = [bub,
                  b'a' * 10 + bub,
                  bub + b'x'*10,
                  b'c' * 5 + bub + b'd' * 5]

    for text_count in range(2, len(test_texts) + 1):
        for texts in permutations(test_texts, text_count):
            lcs = longest_common_substring(texts)
            assert lcs == bub

def test_no_repeats():
    texts = ['a' * 500, 'b' * 500]
    result = rstr_max(texts)

    assert result.match_length == 0
    assert result.matches == tuple()

def test_simple_rstr():
    texts = [b'a::',
             b'b::']

    result = rstr_max(texts)

    assert result.match_length == 2
    assert result.matches == (1, 1)

    texts = [b'::a',
             b'::b']
    result = rstr_max(texts)

    assert result.match_length == 2
    assert result.matches == (0, 0)


def test_all_repeats():
    s1 = b'1yyyy0'
    s2 = b'2yyyy3yyyy4'

    result = all_repeats([s1, s2])

    assert set(result) == set([b'yyyy', b'yyy', b'yy', b'y'])


def test_rstr_max():
    s1 = b'1yyyy0'
    s2 = b'2yyyy3yyyy4'

    result = rstr_max([s1, s2])
    assert result.match_length == 0
    assert len(result.matches) == 0


def _read_files(path):
    fns = glob(pathjoin(path, '*'))
    fns.sort()
    content = []
    for fn in fns:
        content.append(open(fn, 'rb').read())
    return content


# def test_rstr_max_big():
#     ss = _read_files('test/data/html')

#     result = rstr_max(ss)
#     assert result.match_length == 27661
#     assert result.matches == (
#         6831, 7495, 6739, 6574, 6932, 6831, 6379, 6794,
#         7176, 6920, 7007, 6921, 6561, 7742, 6757, 6843,
#         6508, 6761, 6815)


def test_rstr_max_logic():
    texts = [b'1368226","dpci":"246-02-0788',
             b'4104910","dpci":"068-07-0065',
             b'3350721","dpci":"248-63-1547',
             b'1843493","dpci":"246-04-5372',
             b'3247206","dpci":"248-60-0627',
             b'4255101","dpci":"243-30-6501',
             b'4375553","dpci":"040-03-0872',
             b'3567775","dpci":"070-05-0341',
             b'4170911","dpci":"086-04-0259',
             b'4307328","dpci":"248-81-4755',
             b'2237077","dpci":"246-05-1835',
             b'4358735","dpci":"074-00-0030',
             b'3775398","dpci":"244-22-3012',
             b'3763893","dpci":"242-17-5065',
             b'2870702","dpci":"248-52-0775',
             b'3221691","dpci":"244-20-6656',
             b'1354607","dpci":"244-00-6031',
             b'4077605","dpci":"060-03-0312',
             b'3804229","dpci":"244-22-4137',
             b'3890157","dpci":"248-78-1622',
             b'1691132","dpci":"246-04-3258',
             b'4420449","dpci":"026-01-1827',
             b'1567749","dpci":"248-09-2062',
             b'4362542","dpci":"241-27-3133',
             b'3715382","dpci":"248-73-0576',
             b'4157298","dpci":"243-29-1957',
             b'3104684","dpci":"243-19-8713']
    result = rstr_max(texts)

    assert result.match_length == 10
    assert result.matches == tuple([7] * 27)


def test_rstr_max_end():
    texts = [b'abaxxx',
             b'cbcxxx',
             b'dddxxx',
             b'fgfxxx',
             b'nonxxx']
    result = rstr_max(texts)

    assert result.matches == tuple([3] * 5)
    assert result.match_length == 3


def test_rstr_max_begin():
    texts = [b"xxxaba",
             b"xxxcbc",
             b"xxxddd",
             b"xxxfgf",
             b"xxxnon"]
    result = rstr_max(texts)

    assert result.match_length == 3
    assert result.matches == tuple([0] * 5)


def test_rstr_max_wraparound():
    texts = [b'aaabaxxxx',
             b'aaabfcxxxx',
             b'aaadddxxxx',
             b'aaafgfxxxx',
             b'aaanonxxxx']
    result = rstr_max(texts)

    assert result.match_length == 4
    assert result.matches == (5, 6, 6, 6, 6)


def test_rstr_max_doubled():
    texts = [b'aabb', b'aabbaabb']

    result = rstr_max(texts)

    assert result.match_length == 0
    assert result.matches == tuple()


def test_found_bug_1():
    texts = [b"3032704'); ",
             b"3035841'); ",
             b"3714022'); ",
             b"3484658'); ",
             b"4699727'); ",
             b"3713494'); ",
             b"4621762'); ",
             b"1921322'); ",
             b"3914006'); ",
             b"2025732'); ",
             b"4252542'); ",
             b"4066915'); ",
             b"3997210'); ",
             b"3753929'); ",
             b"4600999'); ",
             b"4328259'); ",
             b"3236879'); ",
             b"4243508'); ",
             b"2832063'); ",
             b"4787939'); ",
             b"3546141'); ",
             b"3789634'); ",
             b"4558638'); ",
             b"3827191'); ",
             b"4595830'); ",
             b"4592632'); "]
    result = rstr_max(texts)

    assert result.match_length == 4
    assert result.matches == tuple([7] * len(texts))


def test_found_bug_2():
    texts = [b'bug', b'_bug_', b'_bug_']
    result = rstr_max(texts)

    assert result.match_length == 3
    assert result.matches == (0, 1, 1)


def test_found_bug_3():
    texts = [b'3032704"/>',
             b'3035841"/>',
             b'3714022"/>',
             b'3484658"/>',
             b'4699727"/>',
             b'3713494"/>',
             b'4621762"/>',
             b'1921322"/>',
             b'3914006"/>',
             b'2025732"/>',
             b'4252542"/>',
             b'4066915"/>',
             b'3997210"/>',
             b'3753929"/>',
             b'4600999"/>',
             b'4328259"/>',
             b'1345800"/>',
             b'4915032"/>',
             b'2070789"/>',
             b'5030283"/>',
             b'2044818"/>',
             b'4436078"/>',
             b'2028926"/>',
             b'4853197"/>',
             b'3827671"/>',
             b'4269357"/>',
             b'1420340"/>',
             b'3846564"/>',
             b'4385824"/>',
             b'4225028"/>',
             b'3236879"/>',
             b'4243508"/>',
             b'2832063"/>',
             b'4787939"/>',
             b'3546141"/>',
             b'3789634"/>',
             b'4558638"/>',
             b'3827191"/>',
             b'4595830"/>',
             b'4592632"/>']

    result = rstr_max(texts)

    assert result.matches == tuple([7] * len(texts))
    assert result.match_length == 3


def test_timestamp():
    texts = [b'18:12:44',
             b'18:46:45',
             b'18:12:33',
             b'18:48:07',
             b'23:35:27',
             b'23:36:56',
             b'18:11:57',
             b'23:38:27',
             b'23:44:14',
             b'18:13:08',
             b'18:30:29',
             b'18:13:44',
             b'18:13:27',
             b'23:51:44',
             b'18:10:56',
             b'23:50:55',
             b'23:47:54',
             b'18:11:48',
             b'18:11:21',
             b'22:18:18',
             b'18:48:31',
             b'18:13:56',
             b'23:41:17',
             b'23:35:15',
             b'18:02:59',
             b'18:12:51',
             b'18:13:09',
             b'18:11:41',
             b'18:13:47',
             b'18:31:20',
             b'18:57:28',
             b'18:48:30',
             b'13:22:25',
             b'18:12:12',
             b'23:48:54',
             b'23:45:44',
             b'18:29:23',
             b'18:10:41',
             b'18:13:38',
             b'23:57:25']

    result = rstr_max(texts)

    assert result.match_length == 0
    assert result.matches == tuple()


if __name__ == '__main__':
    test_found_bug_2()
