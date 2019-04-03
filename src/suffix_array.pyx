# cython: language_level=3
# distutils: language = c++

_THIS_FIXES_CYTHON_BUG = 'wtf'

from collections import namedtuple

from suffix_array cimport RepeatFinder as RepeatFinder
from suffix_array cimport SuffixArray
from suffix_array cimport bisect_distance

from libcpp.string cimport string

##

Result = namedtuple('Result', ['match_length', 'matches'])


cdef class RepeatFinderP:
    cdef RepeatFinder *thisptr
    cdef vector[string] texts

    def __cinit__(self, texts):
        self.texts = texts
        self.thisptr = new RepeatFinder(self.texts)

    def __dealloc__(self):
        del self.thisptr

    def go_rstr(self):
        result = self.thisptr.rstr()
        matches = []
        for match in result.matches:
            if match == -1:
                matches.append(None)
            else:
                matches.append(match)
        return Result(result.match_length, tuple(matches))

    cdef unbake_vecs(self, vecs):
        offsets = []
        for vec in vecs:
            offsets.append([
                self.thisptr.sa.text_index_at(ix, self.thisptr.sa.text_at(ix))
                for ix in vec])

        return offsets

    def all_repeats(self):
        result = self.thisptr.all_repeats()
        l = []
        for x in result:
            l.append(x)
        return l

    def LCS(self):
        return self.thisptr.LCS()

    def get_lcp(self):
        return self.thisptr.sa.lcp;


def rstr_max(ss):
    rstr = RepeatFinderP(ss)
    results = rstr.go_rstr()
    return results

def all_repeats(ss):
    rstr = RepeatFinderP(ss)
    all_rpts = rstr.all_repeats()
    return all_rpts


def longest_common_substring(ss):
    rstr = RepeatFinderP(ss)
    lcs = rstr.LCS()
    return lcs


cpdef double file_bisect_distance(string fn0, string fn1):
    return bisect_distance(fn0, fn1)
