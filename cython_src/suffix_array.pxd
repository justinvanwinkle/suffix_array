_THIS_FIXES_CYTHON_BUG = 'wtf'

from libc.stdint cimport int32_t
from libc.stdint cimport uint8_t

cdef extern from "divsufsort.h":
    int32_t divsufsort(const uint8_t *T, int32_t *SA, int32_t n)

    int32_t binarysearch_lower(const int32_t *A, int32_t size, int32_t value)

    int32_t divbwt(const uint8_t *T, uint8_t *U, int32_t *A, int32_t n)

    const char* divsufsort_version()

    int32_t bw_transform(const uint8_t *T,
                         uint8_t *U,
                         int32_t *SA, # /* can NULL */
                         int32_t n,
                         int32_t *idx)

    int32_t inverse_bw_transform(const uint8_t *T,
                                 uint8_t *U,
                                 int32_t *A, # /* can NULL */
                                 int32_t n,
                                 int32_t idx)

    int32_t sufcheck(const uint8_t *T, const int32_t *SA, int32_t n, int32_t verbose)

    int32_t sa_search(const uint8_t *T,
                      int32_t Tsize,
                      const uint8_t *P,
                      int32_t Psize,
                      const int32_t *SA,
                      int32_t SAsize,
                      int32_t *left)

    int32_t sa_simplesearch(const uint8_t *T,
                            int32_t Tsize,
                            const int32_t *SA,
                            int32_t SAsize,
                            int32_t c,
                            int32_t *left)
