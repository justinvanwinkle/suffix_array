_THIS_FIXES_CYTHON_BUG = 'wtf'

from libcpp.vector cimport vector
from libcpp.string cimport string

cdef extern from "divsufsort.h":
    int divsufsort(const unsigned char *T, int *SA, int n) nogil

    int binarysearch_lower(const int *A, int size, int value) nogil
    int binary_cmov(const int *arr, int n, int key)

    int divbwt(const unsigned char *T, unsigned char *U, int *A, int n) nogil

    const char* divsufsort_version() nogil

    int bw_transform(const unsigned char *T,
                         unsigned char *U,
                         int *SA, # /* can NULL */
                         int n,
                         int *idx) nogil

    int inverse_bw_transform(const unsigned char *T,
                                 unsigned char *U,
                                 int *A, # /* can NULL */
                                 int n,
                                 int idx) nogil

    int sufcheck(const unsigned char *T, const int *SA, int n, int verbose) nogil

    int sa_search(const unsigned char *T,
                      int Tsize,
                      const unsigned char *P,
                      int Psize,
                      const int *SA,
                      int SAsize,
                      int *left) nogil

    int sa_simplesearch(const unsigned char *T,
                            int Tsize,
                            const int *SA,
                            int SAsize,
                            int c,
                            int *left) nogil


cdef extern from "repeats.hpp":
    cdef cppclass RepeatFinder:
        RepeatFinder(vector[string], int) except +
        int match_length
        vector[int] matches
        void go_rstr()
