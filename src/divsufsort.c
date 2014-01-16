/*
 * divsufsort.c for libdivsufsort
 * Copyright (c) 2003-2008 Yuta Mori All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "divsufsort_private.h"

/* for divsufsort.c */
#define BUCKET_A(_c0) bucket_A[(_c0)]
#define BUCKET_B(_c0, _c1) (bucket_B[(_c1) * ALPHABET_SIZE + (_c0)])
#define BUCKET_BSTAR(_c0, _c1) (bucket_B[(_c0) * ALPHABET_SIZE + (_c1)])


int32_t
binarysearch_lower(const int32_t *A, int32_t size, int32_t value) {
    int32_t half, i;
    i = 0;
    half = size >> 1;
    do {
	if(A[i + half] < value) {
	    i += half + 1;
	    half -= (size & 1) ^ 1;
	}
	size = half;
	half >>= 1;
    } while (0 < size);

    return i;
}


static
inline
int32_t
int32_t_min(int32_t a, int32_t b) {
    return a < b ? a : b;
}


/* Sorts suffixes of type B*. */
static
int32_t
sort_typeBstar(const uint8_t *T, int32_t *SA,
               int32_t *bucket_A, int32_t *bucket_B,
               int32_t n) {
    int32_t *PAb, *ISAb;
    int32_t i, j, k, t, m;
    int32_t c0, c1;

    /* Initialize bucket arrays. */
    for(i = 0; i < BUCKET_A_SIZE; ++i)
	bucket_A[i] = 0;

    for(i = 0; i < BUCKET_B_SIZE; ++i)
	bucket_B[i] = 0;

    /* Count the number of occurrences of the first one or two characters of each
       type A, B and B* suffix. Moreover, store the beginning position of all
       type B* suffixes into the array SA. */
    for(i = n - 1, m = n, c0 = T[n - 1]; 0 <= i;) {
        /* type A suffix. */
        do {
            ++BUCKET_A(c1 = c0);
        } while((0 <= --i) && ((c0 = T[i]) >= c1));
        if(0 <= i) {
            /* type B* suffix. */
            ++BUCKET_BSTAR(c0, c1);
            SA[--m] = i;
            /* type B suffix. */
	    --i;
	    c1 = c0;
	    while((0 <= i) && ((c0 = T[i]) <= c1)) {
		++BUCKET_B(c0, c1);
		--i;
		c1 = c0;
	    }
        }
    }
    m = n - m;
    /*
      note:
      A type B* suffix is lexicographically smaller than a type B suffix that
      begins with the same first two characters.
    */

    /* Calculate the index of start/end point of each bucket. */
    i = 0;
    j = 0;
    for(c0 = 0; c0 < ALPHABET_SIZE; ++c0) {
        t = i + BUCKET_A(c0);
        BUCKET_A(c0) = i + j; /* start point */
        i = t + BUCKET_B(c0, c0);
        for(c1 = c0 + 1; c1 < ALPHABET_SIZE; ++c1) {
            j += BUCKET_BSTAR(c0, c1);
            BUCKET_BSTAR(c0, c1) = j; /* end point */
            i += BUCKET_B(c0, c1);
        }
    }

    if(0 < m) {
        /* Sort the type B* suffixes by their first two characters. */
        PAb = SA + n - m; ISAb = SA + m;
        for(i = m - 2; 0 <= i; --i) {
            t = PAb[i], c0 = T[t], c1 = T[t + 1];
            SA[--BUCKET_BSTAR(c0, c1)] = i;
        }
        t = PAb[m - 1], c0 = T[t], c1 = T[t + 1];
        SA[--BUCKET_BSTAR(c0, c1)] = m - 1;

        /* Sort the type B* substrings using sssort. */
	j = m;
        for(c0 = ALPHABET_SIZE - 2; 0 < j; --c0) {
            for(c1 = ALPHABET_SIZE - 1; c0 < c1; j = i, --c1) {
                i = BUCKET_BSTAR(c0, c1);
                if(1 < (j - i)) {
                    sssort(T, PAb, SA + i, SA + j, 2, n, *(SA + i) == (m - 1));
                }
            }
        }

        /* Compute ranks of type B* substrings. */
        for(i = m - 1; 0 <= i; --i) {
            if(0 <= SA[i]) {
                j = i;
                do {
		    ISAb[SA[i]] = i;
		} while((0 <= --i) && (0 <= SA[i]));
                SA[i + 1] = i - j;
                if(i <= 0)
		    break;
            }
            j = i;
            do {
		ISAb[SA[i] = ~SA[i]] = j;
	    } while(SA[--i] < 0);
            ISAb[SA[i]] = j;
        }

        /* Construct the inverse suffix array of type B* suffixes using trsort. */
        trsort(ISAb, SA, m, 1);

        /* Set the sorted order of tyoe B* suffixes. */
        for(i = n - 1, j = m, c0 = T[n - 1]; 0 <= i;) {
            for(--i, c1 = c0; (0 <= i) && ((c0 = T[i]) >= c1); --i, c1 = c0) { }
            if(0 <= i) {
                t = i;
		do {
		    --i;
		    c1 = c0;
		    c0 = T[i];
		} while (c0 <= c1);

                SA[ISAb[--j]] = ((t == 0) || (1 < (t - i))) ? t : ~t;
            }
        }

        /* Calculate the index of start/end point of each bucket. */
        BUCKET_B(ALPHABET_SIZE - 1, ALPHABET_SIZE - 1) = n; /* end point */
        for(c0 = ALPHABET_SIZE - 2, k = m - 1; 0 <= c0; --c0) {
            i = BUCKET_A(c0 + 1) - 1;
            for(c1 = ALPHABET_SIZE - 1; c0 < c1; --c1) {
                t = i - BUCKET_B(c0, c1);
                BUCKET_B(c0, c1) = i; /* end point */

                /* Move all type B* suffixes to the correct position. */
                for(i = t, j = BUCKET_BSTAR(c0, c1);
                    j <= k;
                    --i, --k) { SA[i] = SA[k]; }
            }
            BUCKET_BSTAR(c0, c0 + 1) = i - BUCKET_B(c0, c0) + 1; /* start point */
            BUCKET_B(c0, c0) = i; /* end point */
        }
    }

    return m;
}

/* Constructs the suffix array by using the sorted order of type B* suffixes. */
static
void
construct_SA(const uint8_t *T, int32_t *SA,
             int32_t *bucket_A, int32_t *bucket_B,
             int32_t n, int32_t m) {
    int32_t i, j, k;
    int32_t s;
    int32_t c0, c1, c2;

    if(0 < m) {
        /* Construct the sorted order of type B suffixes by using
	   the sorted order of type B* suffixes. */
        for(c1 = ALPHABET_SIZE - 2; 0 <= c1; --c1) {
            /* Scan the suffix array from right to left. */
            i = BUCKET_BSTAR(c1, c1 + 1);
            k = 0;
            c2 = -1;
            for(j = BUCKET_A(c1 + 1) - 1; i <= j; --j) {
                s = SA[j];
                if(s > 0) {
                    //assert(T[s] == c1);
                    //assert(((s + 1) < n) && (T[s] <= T[s + 1]));
                    //assert(T[s - 1] <= T[s]);
                    SA[j] = ~s;
                    c0 = T[--s];
                    if((0 < s) && (T[s - 1] > c0))
                        s = ~s;
                    if(c0 != c2) {
                        if(c2 >= 0)
                            BUCKET_B(c2, c1) = k;
                        c2 = c0;
                        k = BUCKET_B(c2, c1);
                    }
                    //assert(k < j);
                    SA[k--] = s;
                } else {
                    //assert(((s == 0) && (T[s] == c1)) || (s < 0));
                    SA[j] = ~s;
                }
            }
        }
    }

    /* Construct the suffix array by using
       the sorted order of type B suffixes. */
    k = BUCKET_A(c2 = T[n - 1]);
    SA[k++] = (T[n - 2] < c2) ? ~(n - 1) : (n - 1);
    /* Scan the suffix array from left to right. */
    for(i = 0, j = n; i < j; ++i) {
        s = SA[i];
        if(s > 0) {
            //assert(T[s - 1] >= T[s]);
            c0 = T[--s];
            if((s == 0) || (T[s - 1] < c0)) { s = ~s; }
            if(c0 != c2) {
                BUCKET_A(c2) = k;
                k = BUCKET_A(c2 = c0);
            }
            //assert(i < k);
            SA[k++] = s;
        } else {
            //assert(s < 0);
            SA[i] = ~s;
        }
    }
}

/* Constructs the burrows-wheeler transformed string directly
   by using the sorted order of type B* suffixes. */
static
int32_t
construct_BWT(const uint8_t *T, int32_t *SA,
              int32_t *bucket_A, int32_t *bucket_B,
              int32_t n, int32_t m) {
    int32_t i, j, k, orig;
    int32_t s;
    int32_t c0, c1, c2;

    if(0 < m) {
        /* Construct the sorted order of type B suffixes by using
	   the sorted order of type B* suffixes. */
        for(c1 = ALPHABET_SIZE - 2; 0 <= c1; --c1) {
            /* Scan the suffix array from right to left. */
            i = BUCKET_BSTAR(c1, c1 + 1);
            k = 0;
            c2 = -1;
            for(j = BUCKET_A(c1 + 1) - 1; i <= j; --j) {
                s = SA[j];
                if(s > 0) {
                    //assert(T[s] == c1);
                    //assert(((s + 1) < n) && (T[s] <= T[s + 1]));
                    //assert(T[s - 1] <= T[s]);
                    c0 = T[--s];
                    SA[j] = ~((int32_t)c0);
                    if((0 < s) && (T[s - 1] > c0)) { s = ~s; }
                    if(c0 != c2) {
                        if(0 <= c2)
                            BUCKET_B(c2, c1) = k;
                        k = BUCKET_B(c2 = c0, c1);
                    }
                    //assert(k < j);
                    SA[k--] = s;
                } else if(s != 0) {
                    SA[j] = ~s;
                }
            }
        }
    }

    /* Construct the BWTed string by using
       the sorted order of type B suffixes. */
    k = BUCKET_A(c2 = T[n - 1]);
    SA[k++] = (T[n - 2] < c2) ? ~((int32_t)T[n - 2]) : (n - 1);
    /* Scan the suffix array from left to right. */
    j = n;
    orig = 0;
    for(i = 0; i < j; ++i) {
        s = SA[i];
        if(s > 0) {
            //assert(T[s - 1] >= T[s]);
            c0 = T[--s];
            SA[i] = c0;

            if((0 < s) && (T[s - 1] < c0))
		s = ~((int32_t)T[s - 1]);

            if(c0 != c2) {
                BUCKET_A(c2) = k;
                k = BUCKET_A(c2 = c0);
            }
            //assert(i < k);
            SA[k++] = s;
        } else if(s != 0) {
            SA[i] = ~s;
        } else {
            orig = i;
        }
    }

    return orig;
}


/*---------------------------------------------------------------------------*/

/*- Function -*/

int32_t
divsufsort(const uint8_t *T, int32_t *SA, int32_t n) {
    int32_t *bucket_A, *bucket_B;
    int32_t m;
    int32_t err = 0;

    /* Check arguments. */
    if((T == NULL) || (SA == NULL) || (n < 0)) {
	return -1;
    } else if(n == 0) {
        return 0;
    } else if(n == 1) {
        SA[0] = 0;
        return 0;
    } else if(n == 2) {
        m = (T[0] < T[1]);
        SA[m ^ 1] = 0;
        SA[m] = 1;
        return 0;
    }

    bucket_A = (int32_t *)PyMem_Malloc(BUCKET_A_SIZE * sizeof(int32_t));
    bucket_B = (int32_t *)PyMem_Malloc(BUCKET_B_SIZE * sizeof(int32_t));

    /* Suffixsort. */
    if((bucket_A != NULL) && (bucket_B != NULL)) {
        m = sort_typeBstar(T, SA, bucket_A, bucket_B, n);
        construct_SA(T, SA, bucket_A, bucket_B, n, m);
    } else {
        err = -2;
    }

    PyMem_Free(bucket_B);
    PyMem_Free(bucket_A);

    return err;
}

int32_t
divbwt(const uint8_t *T, uint8_t *U, int32_t *A, int32_t n) {
    int32_t *B;
    int32_t *bucket_A, *bucket_B;
    int32_t m, pidx, i;

    /* Check arguments. */
    if((T == NULL) || (U == NULL) || (n < 0)) {
        return -1;
    } else if(n <= 1) {
        if(n == 1) {
            U[0] = T[0];
        }
        return n;
    }

    if((B = A) == NULL)
        B = (int32_t *)PyMem_Malloc((size_t)(n + 1) * sizeof(int32_t));
    bucket_A = (int32_t *)PyMem_Malloc(BUCKET_A_SIZE * sizeof(int32_t));
    bucket_B = (int32_t *)PyMem_Malloc(BUCKET_B_SIZE * sizeof(int32_t));

    /* Burrows-Wheeler Transform. */
    if((B != NULL) && (bucket_A != NULL) && (bucket_B != NULL)) {
        m = sort_typeBstar(T, B, bucket_A, bucket_B, n);
        pidx = construct_BWT(T, B, bucket_A, bucket_B, n, m);

        /* Copy to output string. */
        U[0] = T[n - 1];
        for(i = 0; i < pidx; ++i)
            U[i + 1] = (uint8_t)B[i];
        for(i += 1; i < n; ++i)
            U[i] = (uint8_t)B[i];
        pidx += 1;
    } else {
        pidx = -2;
    }

    PyMem_Free(bucket_B);
    PyMem_Free(bucket_A);
    if(A == NULL)
        PyMem_Free(B);

    return pidx;
}

static
int
_compare(const uint8_t *T, int32_t Tsize,
         const uint8_t *P, int32_t Psize,
         int32_t suf, int32_t *match) {
    int32_t i, j;
    int32_t r;
    i = suf + *match;
    j = *match;
    r = 0;
    while((i < Tsize) && (j < Psize) && ((r = T[i] - P[j]) == 0)) {
	++i;
	++j;
    }
    *match = j;
    return (r == 0) ? -(j != Psize) : r;
}

/* Search for the pattern P in the string T. */
int32_t
sa_search(const uint8_t *T, int32_t Tsize,
          const uint8_t *P, int32_t Psize,
          const int32_t *SA, int32_t SAsize,
          int32_t *idx) {
    int32_t size, lsize, rsize, half;
    int32_t match, lmatch, rmatch;
    int32_t llmatch, lrmatch, rlmatch, rrmatch;
    int32_t i, j, k;
    int32_t r;

    if(idx != NULL)
	*idx = -1;

    if((T == NULL) || (P == NULL) || (SA == NULL) ||
       (Tsize < 0) || (Psize < 0) || (SAsize < 0)) {
	return -1;
    }

    if((Tsize == 0) || (SAsize == 0))
	return 0;

    if(Psize == 0) {
	if(idx != NULL)
	    *idx = 0;
	return SAsize;
    }

    i = 0;
    j = 0;
    k = 0;
    lmatch = 0;
    rmatch = 0;
    size = SAsize;
    half = size >> 1;
    while(0 < size) {
	match = int32_t_min(lmatch, rmatch);
	r = _compare(T, Tsize, P, Psize, SA[i + half], &match);
	if(r < 0) {
	    i += half + 1;
	    half -= (size & 1) ^ 1;
	    lmatch = match;
	} else if(r > 0) {
	    rmatch = match;
	} else {
	    lsize = half;
	    j = i;
	    rsize = size - half - 1;
	    k = i + half + 1;

	    llmatch = lmatch;
	    lrmatch = match;
	    half = lsize >> 1;
            /* left part */
	    while(0 < lsize) {
		lmatch = int32_t_min(llmatch, lrmatch);
		r = _compare(T, Tsize, P, Psize, SA[j + half], &lmatch);
		if(r < 0) {
		    j += half + 1;
		    half -= (lsize & 1) ^ 1;
		    llmatch = lmatch;
		} else {
		    lrmatch = lmatch;
		}
		lsize = half;
		half >>= 1;
	    }

	    rlmatch = match;
	    rrmatch = rmatch;
	    half = rsize >> 1;
	    /* right part */
	    while(0 < rsize) {
		rmatch = int32_t_min(rlmatch, rrmatch);
		r = _compare(T, Tsize, P, Psize, SA[k + half], &rmatch);
		if(r <= 0) {
		    k += half + 1;
		    half -= (rsize & 1) ^ 1;
		    rlmatch = rmatch;
		} else {
		    rrmatch = rmatch;
		}

		rsize = half;
		half >>= 1;
	    }

	    break;
	}
	size = half;
	half >>= 1;
    }

    if(idx != NULL)
	*idx = (0 < (k - j)) ? j : i;

    return k - j;
}

const char *
divsufsort_version(void) {
    return "1.0";
}
