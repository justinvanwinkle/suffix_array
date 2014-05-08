# distutils: language = c++

_THIS_FIXES_CYTHON_BUG = 'wtf'

from collections import namedtuple

from suffix_array cimport RepeatFinder as RepeatFinder
# from suffix_array cimport get_entropy as c_entropy
# from suffix_array cimport nti_distance as c_nti_distance
# from suffix_array cimport ntc_distance as c_ntc_distance
from suffix_array cimport SuffixArray


from libcpp.string cimport string

cpdef make_sa_lcp(s):
    cdef SuffixArray *c_sa = NULL
    cdef string c_s = s
    try:
        c_sa = new SuffixArray(c_s)

        sa = []
        lcp = []
        for i in range(len(s)):
            sa.append(c_sa.suffix_array[i])
            lcp.append(c_sa.lcp[i])

        return sa, lcp
    finally:
        del c_sa


Result = namedtuple('Result', ['match_length', 'matches'])


cdef class RepeatFinderP:
    cdef RepeatFinder *thisptr
    def __cinit__(self, texts):
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
            return Result(result.match_length, tuple(matches))
        finally:
            del result


def rstr_max(ss, min_matching=None):
    rstr = RepeatFinderP(ss)
    results = rstr.go_rstr()
    return results


# def entropy(s):
#     result = c_entropy(s, len(s))
#     return {'t_entropy': result.t_entropy,
#             't_information': result.t_information,
#             't_complexity': result.t_complexity,
#             'levels': result.levels}


# def nti_distance(s1, s2):
#     return c_nti_distance(s1, len(s1), s2, len(s2))

# def ntc_distance(s1, s2):
#     return c_ntc_distance(s1, len(s1), s2, len(s2))
