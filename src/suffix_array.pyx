# distutils: language = c++

_THIS_FIXES_CYTHON_BUG = 'wtf'

from collections import namedtuple

from suffix_array cimport RepeatFinder as RepeatFinder
from suffix_array cimport SuffixArray
from suffix_array cimport bisect_distance

from libcpp.string cimport string


Result = namedtuple('Result', ['match_length', 'matches'])


class TableP:
    def __init__(self,
                 strings,
                 left_match_length,
                 right_match_length,
                 left_extendables,
                 right_extendables):
        self.strings = tuple(strings)
        self.left_match_length = left_match_length
        self.right_match_length = right_match_length
        self.left_extendables = tuple([tuple(l) for l in left_extendables])
        self.right_extendables = tuple([tuple(l) for l in right_extendables])

    @property
    def seperators(self):
        left_sep = self.strings[0][self.left_extendables[0][0]:self.left_extendables[0][0] + self.left_match_length]
        right_sep = self.strings[0][self.right_extendables[0][0]: self.right_extendables[0][0] + self.right_match_length]
        return left_sep, right_sep

    def values(self):
        vals = []
        for lefts, rights, s in zip(self.left_extendables,
                                    self.right_extendables,
                                    self.strings):
            current_vals = []
            for start, end in zip(lefts, rights):
                end = end + self.right_match_length
                current_vals.append(s[start:end])
            vals.append(current_vals)
        return vals

    def __repr__(self):
        return '<Table(left_match_length=%s, right_match_length=%s, len=%s, seps=%s>' % (
            self.left_match_length,
            self.right_match_length,
            len(self.left_extendables),
            self.seperators)

    def tuple(self):
        return (self.strings,
                self.left_match_length,
                self.right_match_length,
                self.left_extendables,
                self.right_extendables)

    def __hash__(self):
        return hash(self.tuple())

    def __eq__(self, o):
        if hasattr(o, 'tuple'):
            return self.tuple() == o.tuple()
        return False

    def start_offsets(self):
        return [doc_offsets[0] for doc_offsets in self.left_extendables]

    def end_offsets(self):
        return [doc_offsets[-1] + self.right_match_length
                for doc_offsets in self.right_extendables]

    def spans(self):
        starts = self.start_offsets()
        ends = self.end_offsets()
        return zip(starts, ends)

    def total_span(self):
        total = 0
        for start, end in zip(self.start_offsets(), self.end_offsets()):
            total += end - start
        return total

    def intersects(self, o):
        for (my_start, my_end), (o_start, o_end) in zip(self.spans(), o.spans()):
            if ((my_start <= o_start < my_end or
                 my_start < o_end < my_end or
                 o_start <= my_start < o_end or
                 o_start < my_end < o_end)):
                return True
        return False

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

    def find_tables(self):
        cdef vector[Table] c_tables = self.thisptr.find_tables()
        cdef Table c_table
        tables = []
        for c_table in c_tables:
            tables.append(TableP(
                self.texts,
                c_table.left_match_length,
                c_table.right_match_length,
                self.unbake_vecs(c_table.left_extendables),
                self.unbake_vecs(c_table.right_extendables)))
        return tables

    def LCS(self):
        return self.thisptr.LCS()


def rstr_max(ss):
    rstr = RepeatFinderP(ss)
    results = rstr.go_rstr()
    return results


def unbake_table(offset_set, match_length, ss):
    unbaked = []
    for offsets, s in zip(offset_set, ss):
        unbaked.append([s[offset: offset + match_length] for offset in offsets])
    return unbaked


def find_tables(ss):
    cstr = RepeatFinderP(ss)
    tables = cstr.find_tables()
    return tables


def LCS(ss):
    rstr = RepeatFinderP(ss)
    lcs = rstr.LCS()
    return lcs


cpdef double file_bisect_distance(string fn0, string fn1):
    return bisect_distance(fn0, fn1)
