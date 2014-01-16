#include "divsufsort_private.h"


int32_t
bw_transform(const uint8_t *T,
	     uint8_t *U,
	     int32_t *SA,
             int32_t n,
	     int32_t *idx) {
    int32_t *A, i, j, p, t;
    int32_t c;

    /* Check arguments. */
    if((T == NULL) || (U == NULL) || (n < 0) || (idx == NULL)) { return -1; }
    if(n <= 1) {
	if(n == 1) { U[0] = T[0]; }
	*idx = n;
	return 0;
    }

    if((A = SA) == NULL) {
	i = divbwt(T, U, NULL, n);
	if(0 <= i) { *idx = i; i = 0; }
	return (int32_t)i;
    }

    /* BW transform. */
    if(T == U) {
	t = n;
	for(i = 0, j = 0; i < n; ++i) {
	    p = t - 1;
	    t = A[i];
	    if(0 <= p) {
		c = T[j];
		U[j] = (j <= p) ? T[p] : (uint8_t)A[p];
		A[j] = c;
		j++;
	    } else {
		*idx = i;
	    }
	}
	p = t - 1;
	if(0 <= p) {
	    c = T[j];
	    U[j] = (j <= p) ? T[p] : (uint8_t)A[p];
	    A[j] = c;
	} else {
	    *idx = i;
	}
    } else {
	U[0] = T[n - 1];
	for(i = 0; A[i] != 0; ++i) { U[i + 1] = T[A[i] - 1]; }
	*idx = i + 1;
	for(++i; i < n; ++i) { U[i] = T[A[i] - 1]; }
    }

    if(SA == NULL) {
	/* Deallocate memory. */
	PyMem_Free(A);
    }

    return 0;
}

/* Inverse Burrows-Wheeler transform. */
int32_t
inverse_bw_transform(const uint8_t *T, uint8_t *U, int32_t *A,
                     int32_t n, int32_t idx) {
    int32_t C[ALPHABET_SIZE];
    uint8_t D[ALPHABET_SIZE];
    int32_t *B;
    int32_t i, p;
    int32_t c, d;

    /* Check arguments. */
    if((T == NULL) || (U == NULL) || (n < 0) || (idx < 0) ||
       (n < idx) || ((0 < n) && (idx == 0))) {
	return -1;
    }
    if(n <= 1) { return 0; }

    if((B = A) == NULL) {
	/* Allocate n*sizeof(int32_t) bytes of memory. */
	if((B = (int32_t *)PyMem_Malloc((size_t)n * sizeof(int32_t))) == NULL) { return -2; }
    }

    /* Inverse BW transform. */
    for(c = 0; c < ALPHABET_SIZE; ++c) { C[c] = 0; }
    for(i = 0; i < n; ++i) { ++C[T[i]]; }
    for(c = 0, d = 0, i = 0; c < ALPHABET_SIZE; ++c) {
	p = C[c];
	if(0 < p) {
	    C[c] = i;
	    D[d++] = (uint8_t)c;
	    i += p;
	}
    }
    for(i = 0; i < idx; ++i) { B[C[T[i]]++] = i; }
    for( ; i < n; ++i)       { B[C[T[i]]++] = i + 1; }
    for(c = 0; c < d; ++c) { C[c] = C[D[c]]; }
    for(i = 0, p = idx; i < n; ++i) {
	U[i] = D[binarysearch_lower(C, d, p)];
	p = B[p - 1];
    }

    if(A == NULL) {
	/* Deallocate memory. */
	PyMem_Free(B);
    }

    return 0;
}
