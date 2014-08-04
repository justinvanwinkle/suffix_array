_THIS_FIXES_CYTHON_BUG = 'wtf'

from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp.pair cimport pair


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
        string LCS()
        vector[Table] find_tables()
        SuffixArray sa

    cdef cppclass RepeatFinderResult:
        int match_length
        int matching
        vector[int] matches

    cdef cppclass Table:
        int left_match_length
        int right_match_length
        vector[vector[int]] left_extendables
        vector[vector[int]] right_extendables

cdef extern from "nodes.hpp" namespace "Nodes":
    cdef double bisect_distance(string, string)
