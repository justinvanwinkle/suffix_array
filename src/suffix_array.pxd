# cython: language_level=3
_THIS_FIXES_CYTHON_BUG = 'wtf'

from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp.pair cimport pair
from libcpp.map cimport map as cpp_map


cdef extern from "repeats.hpp" namespace "RepeatFinding":
    cdef cppclass SuffixArray:
        SuffixArray()
        SuffixArray(vector[vector[int]])
        vector[int] suffix_array
        vector[int] lcp
        vector[int] rank
        vector[int] s
        int s_len
        int num_texts
        int text_at(int)
        int text_index_at(int, int)

    cdef cppclass RepeatFinder:
        RepeatFinder()
        RepeatFinder(vector[vector[int]])
        RepeatFinder(SuffixArray) except +
        RepeatFinderResult rstr()
        vector[vector[int]] all_repeats()
        vector[int] LCS()
        SuffixArray sa

    cdef cppclass RepeatFinderResult:
        int match_length
        int matching
        vector[int] matches

cdef extern from "nodes.hpp" namespace "Nodes":
    cdef double bisect_distance(vector[int], vector[int])
