#include "lcp.hpp"

LCP::LCP(const int *sa, int


/*

cdef class LCP:
    cdef Int32Array lcp
    cdef int length

    def __cinit__(LCP self, SuffixArray SA):
        self.lcp = Int32Array(len(SA))
        self.length = len(SA)
        self._build_lcp(SA)

    cdef _build_lcp(LCP self, SuffixArray SA):
        sa_len = len(SA)
        cdef int i, j, j2, l = 0
        cdef Int32Array rank = Int32Array(sa_len)
        for i in range(sa_len):
            rank[SA.get(i)] = i
        self.lcp[0] = -1
        for j in range(self.length):
            if l != 0:
                l = l - 1
            i = rank[j]
            j2 = SA.get(i - 1)
            if i:
                while (l + j < sa_len and
                       l + j2 < sa_len and
                       SA.s[j + l] == SA.s[j2 + l]):
                    l += 1
                self.lcp[i] = l
            else:
                l = 0

    @cython.profile(False)
    cdef inline int get(LCP self, int i):
        return self.lcp.get(i)

    def __getitem__(LCP self, int i):
        return self.get(i)

    def __len__(LCP self):
        return self.length

    def __iter__(self):
        for i in range(self.length):
            yield self.get(i)

    cdef _eq(LCP self, other):
        cdef int i
        for i in range(self.length):
            if self.lcp.get(i) != other[i]:
                return False
        return True

    def __richcmp__(LCP self, other, int op):
        if op == 2:
            if len(self) != len(other):
                return False
            return self._eq(other)
        raise NotImplemented

*/
