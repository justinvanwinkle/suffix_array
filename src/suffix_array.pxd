_THIS_FIXES_CYTHON_BUG = 'wtf'

from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp.pair cimport pair

# cdef extern from "divsufsort.h":
#     int divsufsort(const unsigned char *T, int *SA, int n) nogil

#     int binarysearch_lower(const int *A, int size, int value) nogil
#     int binary_cmov(const int *arr, int n, int key)

#     int divbwt(const unsigned char *T, unsigned char *U, int *A, int n) nogil

#     const char* divsufsort_version() nogil

#     int bw_transform(const unsigned char *T,
#                          unsigned char *U,
#                          int *SA, # /* can NULL */
#                          int n,
#                          int *idx) nogil

#     int inverse_bw_transform(const unsigned char *T,
#                                  unsigned char *U,
#                                  int *A, # /* can NULL */
#                                  int n,
#                                  int idx) nogil

#     int sufcheck(const unsigned char *T, const int *SA, int n, int verbose) nogil

#     int sa_search(const unsigned char *T,
#                       int Tsize,
#                       const unsigned char *P,
#                       int Psize,
#                       const int *SA,
#                       int SAsize,
#                       int *left) nogil

#     int sa_simplesearch(const unsigned char *T,
#                         int Tsize,
#                         const int *SA,
#                         int SAsize,
#                         int c,
#                         int *left) nogil


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
        SuffixArray sa

    cdef cppclass CommonRepeatFinder:
        CommonRepeatFinder(vector[string])
        CommonRepeatFinder(SuffixArray) except +
        RepeatFinderResult rstr()
        SuffixArray sa
        int match_tables()
        vector[Table] tables

    cdef cppclass RepeatFinderResult:
        int match_length
        int matching
        vector[int] matches

    cdef cppclass Table:
        int left_match_length
        int right_match_length
        vector[vector[int]] left_extendables
        vector[vector[int]] right_extendables

    cdef SuffixArray narrow_sa(SuffixArray, vector[pair[int, int]])
