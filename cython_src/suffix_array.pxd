_THIS_FIXES_CYTHON_BUG = 'wtf'

from libc.stdint cimport int
from libc.stdint cimport uint8_t

cdef extern from "divsufsort.h":
    int divsufsort(const uint8_t *T, int *SA, int n)

    int binarysearch_lower(const int *A, int size, int value)

    int divbwt(const uint8_t *T, uint8_t *U, int *A, int n)

    const char* divsufsort_version()

    int bw_transform(const uint8_t *T,
                         uint8_t *U,
                         int *SA, # /* can NULL */
                         int n,
                         int *idx)

    int inverse_bw_transform(const uint8_t *T,
                                 uint8_t *U,
                                 int *A, # /* can NULL */
                                 int n,
                                 int idx)

    int sufcheck(const uint8_t *T, const int *SA, int n, int verbose)

    int sa_search(const uint8_t *T,
                      int Tsize,
                      const uint8_t *P,
                      int Psize,
                      const int *SA,
                      int SAsize,
                      int *left)

    int sa_simplesearch(const uint8_t *T,
                            int Tsize,
                            const int *SA,
                            int SAsize,
                            int c,
                            int *left)
