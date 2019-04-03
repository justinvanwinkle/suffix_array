# cython: language_level=3
_THIS_FIXES_CYTHON_BUG = 'wtf'

from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp.pair cimport pair
from libcpp.map cimport map as cpp_map


ctypedef pair[int, int] int_pair
ctypedef cpp_map[int_pair, int_pair] repeat_map


cdef extern from "repeats.hpp" namespace "RepeatFinding":
    cdef cppclass SuffixArray:
        SuffixArray()
        SuffixArray(vector[string])
        SuffixArray(SuffixArray, vector[pair[int, int]])
        vector[int] suffix_array
        vector[int] lcp
        vector[int] rank
        string s
        int s_len
        unsigned int num_texts
        int text_at(int)
        int text_index_at(int, int)

    cdef cppclass RepeatFinder:
        RepeatFinder(vector[string])
        RepeatFinder(SuffixArray) except +
        RepeatFinderResult rstr()
        vector[string] all_repeats()
        string LCS()
        SuffixArray sa

    cdef cppclass RepeatFinderResult:
        int match_length
        int matching
        vector[int] matches

cdef extern from "nodes.hpp" namespace "Nodes":
    cdef double bisect_distance(string, string)
