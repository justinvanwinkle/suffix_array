# distutils: language = c++

_THIS_FIXES_CYTHON_BUG = 'wtf'

from suffix_array cimport RepeatFinder as RepeatFinder
from suffix_array cimport get_entropy as c_entropy
from suffix_array cimport SuffixArray


cpdef make_sa_lcp(s):
    cdef SuffixArray* c_sa
    cdef vector[int] *c_lcp

    try:
        c_sa = new SuffixArray(s, len(s))
        c_lcp = c_sa.lcp()

        sa = []
        lcp = []
        for i in range(len(s)):
            sa.append(c_sa.suffix_array[i])
            lcp.append(c_lcp.at(i))

        return sa, lcp

    finally:
        del c_lcp


cdef class RepeatFinderP:
    cdef RepeatFinder *thisptr
    def __cinit__(self, texts, min_matching=None):
        cdef vector[string] ctexts = texts
        with nogil:
            self.thisptr = new RepeatFinder(ctexts)

    def __dealloc__(self):
        del self.thisptr

    def go_rstr(self):
        result = self.thisptr.rstr()
        try:
            matches = []
            for match in result.matches:
                if match == -1:
                    matches.append(None)
                else:
                    matches.append(match)
            return (result.match_length, tuple(matches), result.t_entropy)
        finally:
            del result


def rstr_max(ss, min_matching=None):
    rstr = RepeatFinderP(ss, min_matching)
    results = rstr.go_rstr()
    del rstr
    return results


def entropy(s):
    return c_entropy(s, len(s))
