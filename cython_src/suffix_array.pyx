# #cython: profile=True

_THIS_FIXES_CYTHON_BUG = 'wtf'

cimport cython
from cpython cimport PyMem_Malloc, PyMem_Realloc, PyMem_Free

from libc.stdint cimport int32_t
from libc.stdint cimport uint8_t
from libc.string cimport memset
from suffix_array cimport divsufsort
from suffix_array cimport binarysearch_lower
from suffix_array cimport sa_search


cdef int bsearch_left(const int32_t *array, int size, int q):
    cdef int pos = binarysearch_lower(array, size, q)
    return pos


cdef class Int32Array:
    cdef int32_t* _array
    cdef int length

    def __cinit__(Int32Array self, int32_t length):
        cdef int32_t* _array = <int32_t *>PyMem_Malloc(length * sizeof(int32_t))
        if _array == NULL:
            raise MemoryError
        self._array = _array
        self.length = length

    cpdef Int32Array zero(Int32Array self):
        return self.populate(0)

    cpdef Int32Array populate(Int32Array self, int v):
        cdef int i
        for i in range(self.length):
            self._array[i] = v
        return self

    @cython.profile(False)
    cdef inline int get(Int32Array self, int i):
        return self._array[i]

    cpdef int bsearch(Int32Array self, int q):
        cdef int pos = bsearch_left(self._array, self.length, q)
        if self.get(pos) == q:
            return pos
        return pos - 1

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
        int _sarray_len
        int _alphabet_size
        bytes s

    def __cinit__(self, s):
        cdef Int32Array SA = Int32Array(len(s))
        divsufsort(<uint8_t *><char *>s, SA._array, len(s))
        self.SA = SA

    def __init__(self, s):
        self.s = bytes(s)
        self._sarray_len = len(s)

    cpdef find(SuffixArray self, q):
        # TODO: start and end optional params, see str.find() documentation
        # slice notation might make it more complicated than it sounds...
        cdef int32_t idx = 0

        matches = <int32_t>sa_search(<uint8_t *><char *>self.s,
                                     len(self.s),
                                     <uint8_t *><char *>q,
                                     len(q),
                                     self.SA._array,
                                     self._sarray_len,
                                     &idx)
        if matches == 0:
            return -1
        else:
            return idx

    cpdef count(SuffixArray self, q):
        matches = <int32_t>sa_search(<uint8_t *><char *>self.s,
                                     len(self.s),
                                     <uint8_t *><char *>q,
                                     len(q),
                                     self.SA._array,
                                     self._sarray_len,
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
        for i in range(self._sarray_len):
            yield self.get(i)

    cdef _eq(SuffixArray self, other):
        cdef int i
        for i in range(self._sarray_len):
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
        cdef uint8_t* bwt_str = <uint8_t *>PyMem_Malloc(len(self.s) * sizeof(uint8_t))
        cdef int32_t idx = 0

        if bwt_str == NULL:
            raise MemoryError('Unable to allocate memory')
        try:
            res = bw_transform(self.s, bwt_str, self.SA._array, self._sarray_len, &idx)
            if res != 0:
                raise Exception('Problem')
            return ((<char *>bwt_str)[:self._sarray_len], idx)
        finally:
            PyMem_Free(bwt_str)

    cpdef inverse_bwt(self, s, idx):
        cdef uint8_t* bwt_str = <uint8_t *>PyMem_Malloc(len(self.s) * sizeof(uint8_t))

        if bwt_str == NULL:
            raise MemoryError('Unable to allocate memory')
        try:
            res = inverse_bw_transform(
                s, bwt_str, NULL, self._sarray_len, idx)
            if res != 0:
                raise Exception('Problem')
            return (<char *>bwt_str)[:self._sarray_len]
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

    def __cinit__(Stack self) :
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


from collections import deque

cpdef common_substrings(strs):
    counts = [0] * len(strs)
    big_s = chr(2).join(strs)
    positions = []
    pos = 0
    for s in strs:
        positions.append(pos)
        pos += len(s) + 1

    sa = SuffixArray(big_s)
    a = 1
    b = 0




cdef class Rstr_max:
    char_frontier = chr(2)
    cdef:
        tuple texts
        int num_texts
        Int32Array text_positions
        bytes combined_texts
        SuffixArray sa

    def __init__(self, texts):
        self.texts = texts
        self.num_texts = len(texts)
        self.text_positions = Int32Array(self.num_texts)

        cdef int pos = 0
        for i, text in enumerate(texts):
            self.text_positions[i] = pos
            pos += len(text)
            pos += 1

    def get_repeat(self, id_str, offset, length) :
        return self.texts[id_str][offset:offset+length]

    cdef int text_index_at(Rstr_max self, int i):
        cdef int text_position = self.text_positions[self.text_at(i)]
        return i - text_position

    cdef int text_at(Rstr_max self, int i):
        return self.text_positions.bsearch(i)

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
            int previous_lcp_len = 0

        self.combined_texts = self.char_frontier.join(self.texts)
        sa = SuffixArray(self.char_frontier.join(self.texts))
        self.sa = sa
        lcp = LCP(sa)
        len_lcp = len(lcp) - 1
        if len(sa) == 0:
            return {}

        pos1 = sa.get(0)
        for i in range(len_lcp):
            current_lcp = lcp.get(i + 1)
            pos2 = sa.get(i + 1)
            end_ix = intmax(pos1, pos2) + current_lcp
            n = previous_lcp_len - current_lcp
            if n < 0:
                stack.push((-n, i, end_ix))
                stack._top += -n
            elif n > 0:
                self.remove_many(stack, results, n, i)
            elif stack._top > 0 and end_ix > stack.last()[2]:
                Xn, Xi, _ = stack.pop()
                stack.push((Xn, Xi, end_ix))

            previous_lcp_len = current_lcp
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
            dict r = self.rstr()
            int nb
            int o
            int offset
            int offset_end
            int offset_global
            int start_plage
            list results = []
            list sub_results

        for (offset_end, nb), (match_len, start_ix) in r.iteritems():
            sub_results = []
            results.append(sub_results)
            for o in range(start_ix, start_ix + nb):
                offset_global = self.sa.get(o)
                offset = self.text_index_at(offset_global)
                id_str = self.text_at(offset_global)
                sub_results.append((match_len, offset, id_str))
        return results


def rstr_max(*ss):
    rstr = Rstr_max(ss)

    return rstr.go_rstr()


def rstr_pretty(*ss):
    rstr = Rstr_max(ss)

    results = rstr.go_rstr()


    for result_set in results:
        first = True
        for length, start_offset, filenumber in result_set:
            if first:
                print repr(ss[filenumber][start_offset:start_offset + length])
                first = False
            print ' ' * 4,
            print filenumber, start_offset
