# #cython: profile=True

_THIS_FIXES_CYTHON_BUG = 'wtf'

cimport cython
from cpython cimport PyMem_Malloc, PyMem_Realloc, PyMem_Free

from libc.string cimport memset
from suffix_array cimport divsufsort
from suffix_array cimport binarysearch_lower
from suffix_array cimport binary_cmov
from suffix_array cimport sa_search


cdef class Int32Array:
    cdef int* _array
    cdef int length

    def __cinit__(Int32Array self, int length):
        cdef int* _array = <int *>PyMem_Malloc(length * sizeof(int))
        if _array == NULL:
            raise MemoryError
        self._array = _array
        self.length = length

    cpdef Int32Array zero(Int32Array self):
        return self.populate(0)

    cpdef Int32Array populate(Int32Array self, int v):
        cdef int i
        with nogil:
            for i in range(self.length):
                self._array[i] = v
        return self

    cpdef int binary_search(Int32Array self, int i):
        return binary_cmov(self._array, self.length, i)

    @cython.profile(False)
    cdef inline int get(Int32Array self, int i):
        return self._array[i]

    def __iter__(self):
        cdef int i
        for i in range(self.length):
            yield self._array[i]

    def __len__(self):
        return self.length

    def __dealloc__(self):
        PyMem_Free(self._array)

    def __getitem__(Int32Array self, int i):
        return self.get(i)

    def __setitem__(Int32Array self, int i, int v):
        self._array[i] = v



cdef class SuffixArray:
    cdef:
        Int32Array SA
        int length
        int _alphabet_size
        bytes s

    def __cinit__(self, s):
        cdef Int32Array SA = Int32Array(len(s))
        divsufsort(<unsigned char *><char *>s, SA._array, len(s))
        self.SA = SA

    def __init__(self, s):
        self.s = bytes(s)
        self.length = len(s)

    cpdef find(SuffixArray self, q):
        # TODO: start and end optional params, see str.find() documentation
        # slice notation might make it more complicated than it sounds...
        cdef:
            int idx = 0
            int len_q = len(q)
            unsigned char *_q = <unsigned char *> q
            unsigned char *_s = <unsigned char *> self.s


        with nogil:
            matches = <int>sa_search(_s,
                                     self.length,
                                     _q,
                                     len_q,
                                     self.SA._array,
                                     self.length,
                                     &idx)
        if matches == 0:
            return -1
        else:
            return idx

    cpdef count(SuffixArray self, q):
        cdef:
            int len_q = len(q)
            unsigned char *_q = <unsigned char *> q
            unsigned char *_s = <unsigned char *> self.s
        with nogil:
            matches = <int>sa_search(_s,
                                     self.length,
                                     _q,
                                     len_q,
                                     self.SA._array,
                                     self.length,
                                     NULL)
        return matches

    @cython.profile(False)
    cdef inline int get(SuffixArray self, int i):
        return self.SA.get(i)

    def __getitem__(SuffixArray self, int i):
        return self.get(i)

    def __len__(SuffixArray self):
        return len(self.SA)

    def __contains__(SuffixArray self, q):
        return self.count(q) > 0

    def __iter__(self):
        for i in range(self.length):
            yield self.get(i)

    cdef _eq(SuffixArray self, other):
        cdef int i
        for i in range(self.length):
            if self.SA.get(i) != other[i]:
                return False
        return True

    def __richcmp__(SuffixArray self, other, int op):
        if op == 2:
            if len(self) != len(other):
                return False
            return self._eq(other)
        raise NotImplemented

    cpdef bwt(self):
        cdef unsigned char* bwt_str = <unsigned char *>PyMem_Malloc(len(self.s) * sizeof(unsigned char))
        cdef int idx = 0

        if bwt_str == NULL:
            raise MemoryError('Unable to allocate memory')
        try:
            res = bw_transform(self.s, bwt_str, self.SA._array, self.length, &idx)
            if res != 0:
                raise Exception('Problem')
            return ((<char *>bwt_str)[:self.length], idx)
        finally:
            PyMem_Free(bwt_str)

    cpdef inverse_bwt(self, s, idx):
        cdef unsigned char* bwt_str = <unsigned char *>PyMem_Malloc(len(self.s) * sizeof(unsigned char))

        if bwt_str == NULL:
            raise MemoryError('Unable to allocate memory')
        try:
            res = inverse_bw_transform(
                s, bwt_str, NULL, self.length, idx)
            if res != 0:
                raise Exception('Problem')
            return (<char *>bwt_str)[:self.length]
        finally:
            PyMem_Free(bwt_str)


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


cdef class Stack:
    cdef:
        int _top
        list _stack
        int last_index

    def __cinit__(Stack self):
        self._top = 0
        self._stack = []
        self.last_index = -1

    cdef tuple push(Stack self, tuple t):
        self._stack.append(t)
        self.last_index += 1

    cdef tuple pop(Stack self):
        if self.last_index > -1:
            self.last_index -= 1
        else:
            raise IndexError
        return self._stack.pop()

    cdef tuple last(Stack self):
        return self._stack[self.last_index]


@cython.profile(False)
cdef inline int intmax(int i, int j):
    if i > j:
        return i
    return j


# from collections import deque

# cpdef common_substrings(strs):
#     counts = [0] * len(strs)
#     big_s = chr(2).join(strs)
#     positions = []
#     pos = 0
#     for s in strs:
#         positions.append(pos)
#         pos += len(s) + 1

#     sa = SuffixArray(big_s)
#     a = 1
#     b = 0



cdef class Rstr_max:
    char_frontier = chr(2)
    cdef:
        int num_texts
        Int32Array text_positions
        bytes combined_texts
        SuffixArray sa
        int min_matching

    def __init__(self, texts, min_matching=None):
        if len(texts) == 0:
            raise ValueError('Must pass some texts')

        for text in texts:
            if len(text) == 0:
                raise ValueError('Empty texts are not allowed')

        self.num_texts = len(texts)
        self.text_positions = Int32Array(self.num_texts)
        if min_matching is None:
            min_matching = len(texts)
        self.min_matching = min_matching
        self.combined_texts = self.char_frontier.join(texts)

        cdef int pos = 0
        for i, text in enumerate(texts):
            pos += len(text)
            self.text_positions[i] = pos
            pos += 1

    cdef int text_index_at(Rstr_max self, int i):
        cdef int index_at = self.text_at(i)
        cdef int start
        if index_at == 0:
            start = 0
        else:
            start = self.text_positions.get(index_at - 1) + 1
        return i - start

    cdef int text_at(Rstr_max self, int i):
        return self.text_positions.binary_search(i)

    cdef dict rstr(Rstr_max self):
        cdef:
            LCP lcp
            Stack stack = Stack()
            SuffixArray sa
            dict results = {}
            int Xi
            int Xn
            int _
            int end_ix
            int i = 0
            int len_lcp
            int n
            int pos1
            int pos2
            int current_lcp_len = 0
            int previous_lcp_len = 0

        sa = SuffixArray(self.combined_texts)
        self.sa = sa
        lcp = LCP(sa)
        len_lcp = len(lcp) - 1
        if len(sa) == 0:
            return {}

        pos1 = sa.get(0)
        for i in range(len_lcp):
            current_lcp_len = lcp.get(i + 1)
            pos2 = sa.get(i + 1)
            end_ix = intmax(pos1, pos2) + current_lcp_len
            n = previous_lcp_len - current_lcp_len
            if n < 0:
                stack.push((-n, i, end_ix))
                stack._top -= n
            elif n > 0:
                self.remove_many(stack, results, n, i)
            elif stack._top > 0 and end_ix > stack.last()[2]:
                Xn, Xi, _ = stack.pop()
                stack.push((Xn, Xi, end_ix))

            previous_lcp_len = current_lcp_len
            pos1 = pos2

        if(stack._top > 0):
            self.remove_many(stack, results, stack._top, i + 1)

        return results

    cdef remove_many(Rstr_max self, Stack stack, dict results, int m, int end_ix):
        cdef:
            int last_start_ix = -1
            int max_end_ix = 0
            int n = 0
            int start_ix = 0

        while m > 0:
            n, start_ix, max_end_ix = stack.pop()
            if last_start_ix != start_ix:
                id_ = (max_end_ix, end_ix - start_ix + 1)
                if id_ not in results or results[id_][0] < stack._top:
                    results[id_] = (stack._top, start_ix)
                last_start_ix = start_ix
            m -= n
            stack._top -= n
        if m < 0:
            stack.push((-m, start_ix, max_end_ix - n - m))
            stack._top -= m

    cpdef go_rstr(Rstr_max self):
        cdef:
            int nb
            int o
            int offset
            int offset_end
            int offset_global
            int start_plage
            int good_match

        if self.num_texts < self.min_matching:
            return []

        r = self.rstr()

        results = []
        for (offset_end, nb), (match_len, start_ix) in r.iteritems():
            match_mask = [0] * self.num_texts
            sub_results = []
            for o in range(start_ix, start_ix + nb):
                offset_global = self.sa.get(o)
                offset = self.text_index_at(offset_global)
                id_str = self.text_at(offset_global)
                if id_str >= len(match_mask):
                    print offset_global, offset, id_str, self.num_texts
                    raise ValueError('Fuck %s %s' % (id_str, len(match_mask)))
                match_mask[id_str] = 1
                sub_results.append((match_len, offset, id_str))
            good_match = self.num_texts
            for i in range(self.num_texts):
                if match_mask[i] == 0:
                    good_match -= 1
            if good_match >= self.min_matching:
                results.append(sub_results)


        return results


def rstr_max(*ss):
    rstr = Rstr_max(ss)

    return rstr.go_rstr()
