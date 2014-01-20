_THIS_FIXES_CYTHON_BUG = 'wtf'

cdef extern from "divsufsort.h":
    int divsufsort(const unsigned char *T, int *SA, int n)

    int binarysearch_lower(const int *A, int size, int value)

    int divbwt(const unsigned char *T, unsigned char *U, int *A, int n)

    const char* divsufsort_version()

    int bw_transform(const unsigned char *T,
                         unsigned char *U,
                         int *SA, # /* can NULL */
                         int n,
                         int *idx)

    int inverse_bw_transform(const unsigned char *T,
                                 unsigned char *U,
                                 int *A, # /* can NULL */
                                 int n,
                                 int idx)

    int sufcheck(const unsigned char *T, const int *SA, int n, int verbose)

    int sa_search(const unsigned char *T,
                      int Tsize,
                      const unsigned char *P,
                      int Psize,
                      const int *SA,
                      int SAsize,
                      int *left)

    int sa_simplesearch(const unsigned char *T,
                            int Tsize,
                            const int *SA,
                            int SAsize,
                            int c,
                            int *left)
