# distutils: language = c++

_THIS_FIXES_CYTHON_BUG = 'wtf'

from suffix_array cimport RepeatFinder as RepeatFinder


cdef class RepeatFinderP:
    cdef RepeatFinder *thisptr
    def __cinit__(self, texts, min_matching=None):
        cdef vector[string] ctexts = texts
        cdef int cmin_matching
        if min_matching is None:
            cmin_matching = len(texts)
        else:
            cmin_matching = min_matching
        with nogil:
            self.thisptr = new RepeatFinder(ctexts, cmin_matching)

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
            return (result.match_length, tuple(matches))
        finally:
            del result


def rstr_max(ss, min_matching=None):
    rstr = RepeatFinderP(ss, min_matching)
    results = rstr.go_rstr()
    del rstr
    return results
