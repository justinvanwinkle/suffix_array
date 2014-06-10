# distutils: language = c++

_THIS_FIXES_CYTHON_BUG = 'wtf'

from collections import namedtuple

from suffix_array cimport RepeatFinder as RepeatFinder
from suffix_array cimport CommonRepeatFinder as CommonRepeatFinder
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
        matches = []
        for match in result.matches:
            if match == -1:
                matches.append(None)
            else:
                matches.append(match)
        return Result(result.match_length, tuple(matches))


class TableP:
    def __init__(self,
                 strings,
                 left_match_length,
                 right_match_length,
                 left_extendables,
                 right_extendables):
        self.strings = strings
        self.left_match_length = left_match_length
        self.right_match_length = right_match_length
        self.left_extendables = left_extendables
        self.right_extendables = right_extendables

    @property
    def seperators(self):
        left_sep = self.strings[0][self.left_extendables[0][0]:self.left_extendables[0][0] + self.left_match_length]
        right_sep = self.strings[0][self.right_extendables[0][0]: self.right_extendables[0][0] + self.right_match_length]
        return left_sep, right_sep

    def __repr__(self):
        return '<Table(left_match_length=%s, right_match_length=%s, len=%s, seps=%s>' % (
            self.left_match_length,
            self.right_match_length,
            len(self.left_extendables),
            self.seperators)

    def start_offsets(self):
        return [doc_offsets[0] for doc_offsets in self.left_extendables]

    def end_offsets(self):
        return [doc_offsets[-1] for doc_offsets in self.right_extendables]

    def spans(self):
        starts = self.start_offsets()
        ends = self.end_offsets()
        for start, end in zip(starts, ends):
            yield start, end - start

    def total_span(self):
        total = 0
        for start_group, end_group in zip(self.left_extendables,
                                          self.right_extendables):
            for start, end in zip(start_group, end_group):
                total += end - start

        return total

cdef class CommonRepeatFinderP:
    cdef CommonRepeatFinder *thisptr
    cdef vector[string] texts
    def __cinit__(self, texts):
        self.texts = texts
        with nogil:
            self.thisptr = new CommonRepeatFinder(self.texts)

    cdef unbake_vecs(self, vecs):
        offsets = []
        for vec in vecs:
            offsets.append([
                self.thisptr.text_index_at(ix, self.thisptr.text_at(ix))
                for ix in vec])

        return offsets

    @property
    def tables(self):
        self.thisptr.match_tables(1)
        tables = []
        for c_table in self.thisptr.tables:
            tables.append(TableP(
                self.texts,
                c_table.left_match_length,
                c_table.right_match_length,
                self.unbake_vecs(c_table.left_extendables),
                self.unbake_vecs(c_table.right_extendables)))
        return tables

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


def rstr_max(ss, min_matching=None):
    rstr = RepeatFinderP(ss)
    results = rstr.go_rstr()
    return results


def unbake_table(offset_set, match_length, ss):
    unbaked = []
    for offsets, s in zip(offset_set, ss):
        unbaked.append([s[offset: offset + match_length] for offset in offsets])
    return unbaked



def find_tables(ss):
    cstr = CommonRepeatFinderP(ss)
    result = cstr.go_rstr()
    tables = cstr.tables

    # lefts = []
    # rights = []

    # matches = []
    # for table in tables:
    #     if table.left_extendables:
    #         lefts.append(table.left_extendables)
    #     if table.right_extendables:
    #         rights.append(table.right_extendables)

    # for left_table in [t for t in tables if t.left_extendables]:
    #     for right_table in [t for t in tables if t.right_extendables]:
    #         match = True
    #         for doc_left, doc_right in zip(left_table.left_extendables,
    #                                        right_table.right_extendables):
    #             if len(doc_left) == len(doc_right):
    #                 offset_delta = None
    #                 for offset_l, offset_r in zip(doc_left[1:], doc_right[:-1]):
    #                     if offset_l - offset_r < 0:
    #                         match = False
    #                         break
    #                     elif offset_l - offset_r > left_table.left_match_length:
    #                         match = False
    #                         break

    #                     if offset_delta is None:
    #                         offset_delta = offset_l - offset_r
    #                     else:
    #                         if offset_delta != offset_l - offset_r:
    #                             match = False
    #                             break
    #             else:
    #                 match = False
    #         if match:
    #             matches.append((left_table, right_table))

    # for table in tables:
    #     print '*' * 200
    #     print map(len, unbake_table(table.left_extendables, table.left_match_length, ss))
    #     print table.left_extendables, table.left_match_length
    #     print table.right_extendables, table.right_match_length
    #     print unbake_table(table.left_extendables, table.left_match_length, ss)
    #     print unbake_table(table.right_extendables, table.right_match_length, ss)
    #     print '******'
    return tables
