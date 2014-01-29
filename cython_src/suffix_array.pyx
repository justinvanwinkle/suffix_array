# distutils: language = c++

_THIS_FIXES_CYTHON_BUG = 'wtf'

from suffix_array cimport RepeatFinder as RepeatFinder


cdef class RepeatFinderP:
    cdef RepeatFinder *thisptr
    def __cinit__(self, texts, min_matching=None):
        if min_matching is None:
            min_matching = len(texts)
        self.thisptr = new RepeatFinder(texts, min_matching)
    def __dealloc__(self):
        del self.thisptr
    def go_rstr(self):
        self.thisptr.go_rstr()
        matches = []
        for match in self.thisptr.matches:
            if match == -1:
                matches.append(None)
            else:
                matches.append(match)
        return (self.thisptr.match_length, tuple(matches))


def rstr_max(ss, min_matching=None):
    rstr = RepeatFinderP(ss, min_matching)
    return rstr.go_rstr()
