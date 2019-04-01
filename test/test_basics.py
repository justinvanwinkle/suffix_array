from os.path import join as pathjoin
from glob import glob

# from suffix_array import SuffixArray
# from suffix_array import make_lcp
from suffix_array import rstr_max
from suffix_array import find_tables
from suffix_array import SuffixArray


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


def test_lcp_array():
    assert LCP(SuffixArray('abcdefg')) == [-1] + [0] * 6
    assert LCP(SuffixArray('banana')) == [-1, 1, 3, 0, 0, 2]


def test_simple_rstr():
    l = ['a::',
         'b::']

    result = rstr_max(l)
    ##print result
    assert result.match_length == 2
    assert result.matches == (1, 1)

    l = ['::a',
         '::b']
    result = rstr_max(l)
    #print result
    assert result.match_length == 2
    assert result.matches == (0, 0)


def test_rstr_max():
    s1 = '1yyyy0'
    s2 = '2yyyy3yyyy4'

    result = rstr_max([s1, s2])
    assert result.match_length == 0
    assert len(result.matches) == 0


def _read_files(path):
    fns = glob(pathjoin(path, '*'))
    fns.sort()
    content = []
    for fn in fns:
        content.append(open(fn).read())
    return content


def test_rstr_max_big():
    ss = _read_files('test/data/html')

    result = rstr_max(ss)
    assert result.match_length == 27661
    assert result.matches == (6831, 7495, 6739, 6574, 6932, 6831, 6379, 6794,
                              7176, 6920, 7007, 6921, 6561, 7742, 6757, 6843,
                              6508, 6761, 6815)


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
    result = rstr_max(l)
    #print result
    assert result.match_length == 10
    assert result.matches == tuple([7] * 27)

def test_rstr_max_end():
    l = ['abaxxx',
         'cbcxxx',
         'dddxxx',
         'fgfxxx',
         'nonxxx']
    result = rstr_max(l)
    #print result
    assert result.matches == tuple([3] * 5)
    assert result.match_length == 3


def test_rstr_max_begin():
    l = ["xxxaba",
         "xxxcbc",
         "xxxddd",
         "xxxfgf",
         "xxxnon"]
    result = rstr_max(l)
    #print result
    assert result.match_length == 3
    assert result.matches == tuple([0] * 5)


def test_rstr_max_wraparound():
    l = ['aaabaxxxx',
         'aaabfcxxxx',
         'aaadddxxxx',
         'aaafgfxxxx',
         'aaanonxxxx']
    result = rstr_max(l)
    #print result
    assert result.match_length == 4
    assert result.matches == (5, 6, 6, 6, 6)



def test_found_bug_1():
    l = ["3032704'); ",
         "3035841'); ",
         "3714022'); ",
         "3484658'); ",
         "4699727'); ",
         "3713494'); ",
         "4621762'); ",
         "1921322'); ",
         "3914006'); ",
         "2025732'); ",
         "4252542'); ",
         "4066915'); ",
         "3997210'); ",
         "3753929'); ",
         "4600999'); ",
         "4328259'); ",
         "3236879'); ",
         "4243508'); ",
         "2832063'); ",
         "4787939'); ",
         "3546141'); ",
         "3789634'); ",
         "4558638'); ",
         "3827191'); ",
         "4595830'); ",
         "4592632'); "]
    result = rstr_max(l)
    #print result
    assert result.match_length == 4
    assert result.matches == tuple([7] * len(l))


def test_found_bug_2():
    l = ['bug', '_bug_', '_bug_']
    result = rstr_max(l)
    #print result
    assert result.match_length == 3
    assert result.matches == (0, 1, 1)



def test_found_bug_3():
    l = ['3032704"/>',
         '3035841"/>',
         '3714022"/>',
         '3484658"/>',
         '4699727"/>',
         '3713494"/>',
         '4621762"/>',
         '1921322"/>',
         '3914006"/>',
         '2025732"/>',
         '4252542"/>',
         '4066915"/>',
         '3997210"/>',
         '3753929"/>',
         '4600999"/>',
         '4328259"/>',
         '1345800"/>',
         '4915032"/>',
         '2070789"/>',
         '5030283"/>',
         '2044818"/>',
         '4436078"/>',
         '2028926"/>',
         '4853197"/>',
         '3827671"/>',
         '4269357"/>',
         '1420340"/>',
         '3846564"/>',
         '4385824"/>',
         '4225028"/>',
         '3236879"/>',
         '4243508"/>',
         '2832063"/>',
         '4787939"/>',
         '3546141"/>',
         '3789634"/>',
         '4558638"/>',
         '3827191"/>',
         '4595830"/>',
         '4592632"/>']

    result = rstr_max(l)
    #print result
    assert result.matches == tuple([7] * len(l))
    assert result.match_length == 3


def test_timestamp():
    l = ['18:12:44',
         '18:46:45',
         '18:12:33',
         '18:48:07',
         '23:35:27',
         '23:36:56',
         '18:11:57',
         '23:38:27',
         '23:44:14',
         '18:13:08',
         '18:30:29',
         '18:13:44',
         '18:13:27',
         '23:51:44',
         '18:10:56',
         '23:50:55',
         '23:47:54',
         '18:11:48',
         '18:11:21',
         '22:18:18',
         '18:48:31',
         '18:13:56',
         '23:41:17',
         '23:35:15',
         '18:02:59',
         '18:12:51',
         '18:13:09',
         '18:11:41',
         '18:13:47',
         '18:31:20',
         '18:57:28',
         '18:48:30',
         '13:22:25',
         '18:12:12',
         '23:48:54',
         '23:45:44',
         '18:29:23',
         '18:10:41',
         '18:13:38',
         '23:57:25']

    result = rstr_max(l)
    #print result
    assert result.match_length == 0
    assert result.matches == tuple()


# def test_tables():
#     l = ['<ul> <li>this</li> <li>is</li> <li>a</li> <li>fat</li> <li>list</li></ul>',
#          '<ul> <li>of crap</li> <li>that</li></ul>',
#          '<ul> <li>that</li> <li>is</li></ul>',
#          '<ul> <li>dumb</li></ul>']

#     tables = find_tables(l)
#     assert len(tables) == 1

    # fns = glob('/Users/jvanwink/scratch/dpreview/specs/*')[:20]
    # l = [open(fn).read() for fn in fns]

    # tables = find_tables(l)
    # assert len(tables) == 50





if __name__ == '__main__':
    test_found_bug_2()
