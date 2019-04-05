# cython: language_level=3
# distutils: language = c++

_THIS_FIXES_CYTHON_BUG = 'wtf'

from collections import namedtuple

from suffix_array cimport RepeatFinder as RepeatFinder
from suffix_array cimport SuffixArray
from suffix_array cimport bisect_distance

from libcpp.string cimport string

##

Result = namedtuple('Result', ['match_length', 'matches'])

cdef vector[int] bytes_to_vector(bytes b):
    cdef vector[int] text
    for c in b:
        text.push_back(c)
    return text


cdef vector[vector[int]] strings_to_vectors(ss):
    cdef vector[vector[int]] texts
    cdef vector[int] text

    for s in ss:
        if isinstance(s, bytes):
            text = bytes_to_vector(s)
        else:
            text = bytes_to_vector(s.encode('utf-32'))
        texts.push_back(text)

    return texts


cdef bytes vector_to_byte(vector[int] text):
    ba = bytearray(text.size())
    for ix in range(text.size()):
        ba[ix] = text[ix]
    return bytes(ba)


cdef str vector_to_string(vec):
    return vector_to_byte(vec).decode('utf-32')

cdef list vectors_to_strings(vecs):
    ss = []
    for vec in vecs:
        ss.append(vector_to_string(vec))
    return ss


cdef list vectors_to_bytes(vecs):
    ss = []
    for vec in vecs:
        ss.append(vector_to_byte(vec))
    return ss



cdef class RepeatFinderP:
    cdef RepeatFinder *thisptr
    cdef vector[vector[int]] texts
    cdef int is_byte

    def __cinit__(self, texts):
        if len(texts) > 0:
            if isinstance(texts[0], bytes):
                self.is_byte = True
            else:
                self.is_byte = False

            for text in texts:
                if isinstance(text, bytes) != self.is_byte:
                    raise ValueError("Can't mix str and bytes")
        self.texts = strings_to_vectors(texts)
        self.thisptr = new RepeatFinder(self.texts)

    def __dealloc__(self):
        if self.thisptr is not NULL:
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

    def all_repeats(self):
        cdef vector[vector[int]] result = self.thisptr.all_repeats()
        if self.is_byte:
            return vectors_to_bytes(result)
        else:
            return vectors_to_strings(result)

    def LCS(self):
        result = self.thisptr.LCS()
        if self.is_byte:
            return vector_to_byte(result)
        else:
            return vector_to_string(result)

    def get_lcp(self):
        return self.thisptr.sa.lcp;


def rstr_max(ss):
    rstr = RepeatFinderP(ss)
    results = rstr.go_rstr()
    return results

def all_repeats(ss):
    rstr = RepeatFinderP(ss)
    all_rpts = rstr.all_repeats()
    return all_rpts


def longest_common_substring(ss):
    rstr = RepeatFinderP(ss)
    lcs = rstr.LCS()
    return lcs


# cpdef double file_bisect_distance(string fn0, string fn1):
#     s1 = open(fn0, 'rb').read()
#     s2 = open(fn1, 'rb').read()
#     return bisect_distance(vector[int](s1), vectors[int](s2))
