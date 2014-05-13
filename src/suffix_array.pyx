# distutils: language = c++

_THIS_FIXES_CYTHON_BUG = 'wtf'

from collections import namedtuple

from suffix_array cimport RepeatFinder as RepeatFinder
from suffix_array cimport CommonRepeatFinder as CommonRepeatFinder
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


cdef class TableP:
    def __cinit__(self, bounds, record_divides):
        self.bounds = bounds
        self.record_divides = record_divides

    @classmethod
    def from_c(cls, bounds, record_divides):
        new_bounds = []
        for b in bounds:
            new_bounds.append((b.left, b.right))

        new_record_divides = []
        for divides in record_divides:
            new_divides = []
            new_record_divides.append(new_divides)
            for divide in divides:
                new_divides.append(divide)

        return cls(new_bounds, new_record_divides)


cdef class CommonRepeatFinderP:
    cdef CommonRepeatFinder *thisptr
    def __cinit__(self, texts):
        cdef vector[string] ctexts = texts
        with nogil:
            self.thisptr = new CommonRepeatFinder(ctexts)

    @property
    def tables(self):
        tables = []
        for c_table in self.thisptr.tables:
            tables.append(TableP.from_c(c_table.bounds, c_table.record_divides))
        return tables

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


def find_tables(ss):
    cstr = CommonRepeatFinderP(ss)
    cstr.go_rstr()
    tables = cstr.tables
    # print tables
    return tables



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
