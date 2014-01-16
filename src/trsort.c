/*
 * trsort.c for libdivsufsort
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


static
inline
void
array_swap(int32_t array[], int32_t a, int32_t b) {
    int32_t t = array[a];
    array[a] = array[b];
    array[b] = t;
}


/*- Private Functions -*/

static const int32_t lg_table[256] = {
    -1,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

static
inline
int32_t
tr_ilg(int32_t n) {
    return (n & (int32_t)0xffff0000) ?
	((n & (int32_t)0xff000000) ?
	 24 + lg_table[(n >> 24) & 0xff] :
	 16 + lg_table[(n >> 16) & 0xff]) :
	((n & 0x0000ff00) ?
	 8 + lg_table[(n >>  8) & 0xff] :
	 0 + lg_table[(n >>  0) & 0xff]);
}


/*---------------------------------------------------------------------------*/

/* Simple insertionsort for small size groups. */
static
void
tr_insertionsort(const int32_t *ISAd, int32_t *first, int32_t *last) {
    int32_t *a, *b;
    int32_t t, r;

    for(a = first + 1; a < last; ++a) {
	t = *a;
	b = a - 1;
	while(0 > (r = ISAd[t] - ISAd[*b])) {
	    do {
		*(b + 1) = *b;
	    } while((first <= --b) && (*b < 0));
	    if(b < first)
		break;
	}
	if(r == 0)
	    *b = ~*b;
	*(b + 1) = t;
    }
}


/*---------------------------------------------------------------------------*/

static
void
tr_fixdown(const int32_t *ISAd, int32_t *SA, int32_t i, int32_t size) {
    int32_t j, k;
    int32_t v;
    int32_t c, d, e;

    for(v = SA[i], c = ISAd[v]; (j = 2 * i + 1) < size; SA[i] = SA[k], i = k) {
	d = ISAd[SA[k = j++]];
	if(d < (e = ISAd[SA[j]])) { k = j; d = e; }
	if(d <= c) { break; }
    }
    SA[i] = v;
}

/* Simple top-down heapsort. */
static
void
tr_heapsort(const int32_t *ISAd, int32_t *SA, int32_t size) {
    int32_t i, m;
    int32_t t;

    m = size;
    if((size % 2) == 0) {
	m--;
	if(ISAd[SA[m / 2]] < ISAd[SA[m]]) { array_swap(SA, m, m/2); }
    }

    for(i = m / 2 - 1; 0 <= i; --i) { tr_fixdown(ISAd, SA, i, m); }
    if((size % 2) == 0) {
	array_swap(SA, 0, m);
	tr_fixdown(ISAd, SA, 0, m);
    }
    for(i = m - 1; 0 < i; --i) {
	t = SA[0], SA[0] = SA[i];
	tr_fixdown(ISAd, SA, 0, i);
	SA[i] = t;
    }
}


/*---------------------------------------------------------------------------*/

/* Returns the median of three elements. */
static
int32_t *
tr_median3(const int32_t *ISAd, int32_t *v1, int32_t *v2, int32_t *v3) {
    if(ISAd[*v1] > ISAd[*v2]) { SWAP(v1, v2); }
    if(ISAd[*v2] > ISAd[*v3]) {
	if(ISAd[*v1] > ISAd[*v3]) { return v1; }
	else { return v3; }
    }
    return v2;
}

/* Returns the median of five elements. */
static
int32_t *
tr_median5(const int32_t *ISAd,
           int32_t *v1, int32_t *v2, int32_t *v3, int32_t *v4, int32_t *v5) {
    if(ISAd[*v2] > ISAd[*v3]) { SWAP(v2, v3); }
    if(ISAd[*v4] > ISAd[*v5]) { SWAP(v4, v5); }
    if(ISAd[*v2] > ISAd[*v4]) { SWAP(v2, v4); SWAP(v3, v5); }
    if(ISAd[*v1] > ISAd[*v3]) { SWAP(v1, v3); }
    if(ISAd[*v1] > ISAd[*v4]) { SWAP(v1, v4); SWAP(v3, v5); }
    if(ISAd[*v3] > ISAd[*v4]) { return v4; }
    return v3;
}

/* Returns the pivot element. */
static
int32_t *
tr_pivot(const int32_t *ISAd, int32_t *first, int32_t *last) {
    int32_t *middle;
    int32_t t;

    t = (int32_t)(last - first);
    middle = first + t / 2;

    if(t <= 512) {
	if(t <= 32) {
	    return tr_median3(ISAd, first, middle, last - 1);
	} else {
	    t >>= 2;
	    return tr_median5(ISAd, first, first + t, middle, last - 1 - t, last - 1);
	}
    }
    t >>= 3;
    first  = tr_median3(ISAd, first, first + t, first + (t << 1));
    middle = tr_median3(ISAd, middle - t, middle, middle + t);
    last   = tr_median3(ISAd, last - 1 - (t << 1), last - 1 - t, last - 1);
    return tr_median3(ISAd, first, middle, last);
}


/*---------------------------------------------------------------------------*/

typedef struct _trbudget_t trbudget_t;
struct _trbudget_t {
    int32_t chance;
    int32_t remain;
    int32_t incval;
    int32_t count;
};

static
void
trbudget_init(trbudget_t *budget, int32_t chance, int32_t incval) {
    budget->chance = chance;
    budget->remain = budget->incval = incval;
}

static
int32_t
trbudget_check(trbudget_t *budget, int32_t size) {
    if(size <= budget->remain) { budget->remain -= size; return 1; }
    if(budget->chance == 0) { budget->count += size; return 0; }
    budget->remain += budget->incval - size;
    budget->chance -= 1;
    return 1;
}


/*---------------------------------------------------------------------------*/

static
void
tr_partition(const int32_t *ISAd,
             int32_t *first, int32_t *middle, int32_t *last,
             int32_t **pa, int32_t **pb, int32_t v) {
    int32_t *a, *b, *c, *d, *e, *f;
    int32_t t, s;
    int32_t x = 0;

    for(b = middle - 1; (++b < last) && ((x = ISAd[*b]) == v);) { }
    if(((a = b) < last) && (x < v)) {
	for(; (++b < last) && ((x = ISAd[*b]) <= v);) {
	    if(x == v) { SWAP(*b, *a); ++a; }
	}
    }
    for(c = last; (b < --c) && ((x = ISAd[*c]) == v);) { }
    if((b < (d = c)) && (x > v)) {
	for(; (b < --c) && ((x = ISAd[*c]) >= v);) {
	    if(x == v) { SWAP(*c, *d); --d; }
	}
    }
    for(; b < c;) {
	SWAP(*b, *c);
	for(; (++b < c) && ((x = ISAd[*b]) <= v);) {
	    if(x == v) { SWAP(*b, *a); ++a; }
	}
	for(; (b < --c) && ((x = ISAd[*c]) >= v);) {
	    if(x == v) { SWAP(*c, *d); --d; }
	}
    }

    if(a <= d) {
	c = b - 1;
	if((s = (int32_t)(a - first)) > (t = (int32_t)(b - a))) { s = t; }
	for(e = first, f = b - s; 0 < s; --s, ++e, ++f) { SWAP(*e, *f); }
	if((s = d - c) > (t = last - d - 1)) { s = t; }
	for(e = b, f = last - s; 0 < s; --s, ++e, ++f) { SWAP(*e, *f); }
	first += (b - a), last -= (d - c);
    }
    *pa = first, *pb = last;
}

static
void
tr_copy(int32_t *ISA, const int32_t *SA,
        int32_t *first, int32_t *a, int32_t *b, int32_t *last,
        int32_t depth) {
    /* sort suffixes of middle partition
       by using sorted order of suffixes of left and right partition. */
    int32_t *c, *d, *e;
    int32_t s, v;

    v = b - SA - 1;
    for(c = first, d = a - 1; c <= d; ++c) {
	if((0 <= (s = *c - depth)) && (ISA[s] == v)) {
	    *++d = s;
	    ISA[s] = d - SA;
	}
    }
    for(c = last - 1, e = d + 1, d = b; e < d; --c) {
	if((0 <= (s = *c - depth)) && (ISA[s] == v)) {
	    *--d = s;
	    ISA[s] = d - SA;
	}
    }
}

static
void
tr_partialcopy(int32_t *ISA, const int32_t *SA,
               int32_t *first, int32_t *a, int32_t *b, int32_t *last,
               int32_t depth) {
    int32_t *c, *d, *e;
    int32_t s, v;
    int32_t rank, lastrank, newrank = -1;

    v = b - SA - 1;
    lastrank = -1;
    for(c = first, d = a - 1; c <= d; ++c) {
	if((0 <= (s = *c - depth)) && (ISA[s] == v)) {
	    *++d = s;
	    rank = ISA[s + depth];
	    if(lastrank != rank) { lastrank = rank; newrank = d - SA; }
	    ISA[s] = newrank;
	}
    }

    lastrank = -1;
    for(e = d; first <= e; --e) {
	rank = ISA[*e];
	if(lastrank != rank) { lastrank = rank; newrank = e - SA; }
	if(newrank != rank) { ISA[*e] = newrank; }
    }

    lastrank = -1;
    for(c = last - 1, e = d + 1, d = b; e < d; --c) {
	if((0 <= (s = *c - depth)) && (ISA[s] == v)) {
	    *--d = s;
	    rank = ISA[s + depth];
	    if(lastrank != rank) { lastrank = rank; newrank = d - SA; }
	    ISA[s] = newrank;
	}
    }
}

static
void
tr_introsort(int32_t *ISA, const int32_t *ISAd,
             int32_t *SA, int32_t *first, int32_t *last,
             trbudget_t *budget) {
    struct { const int32_t *a; int32_t *b, *c; int32_t d, e; }stack[TR_STACKSIZE];
    int32_t *a, *b, *c;
    int32_t v, x = 0;
    int32_t incr = (int32_t)(ISAd - ISA);
    int32_t limit, next;
    int32_t ssize, trlink = -1;

    for(ssize = 0, limit = tr_ilg((int32_t)(last - first));;) {

	if(limit < 0) {
	    if(limit == -1) {
		/* tandem repeat partition */
		tr_partition(ISAd - incr, first, first, last, &a, &b, last - SA - 1);

		/* update ranks */
		if(a < last) {
		    for(c = first, v = a - SA - 1; c < a; ++c) { ISA[*c] = v; }
		}
		if(b < last) {
		    for(c = a, v = b - SA - 1; c < b; ++c) { ISA[*c] = v; }
		}

		/* push */
		if(1 < (b - a)) {
		    STACK_PUSH5(NULL, a, b, 0, 0);
		    STACK_PUSH5(ISAd - incr, first, last, -2, trlink);
		    trlink = ssize - 2;
		}
		if((a - first) <= (last - b)) {
		    if(1 < (a - first)) {
			STACK_PUSH5(ISAd, b, last, tr_ilg(last - b), trlink);
			last = a, limit = tr_ilg(a - first);
		    } else if(1 < (last - b)) {
			first = b, limit = tr_ilg(last - b);
		    } else {
			STACK_POP5(ISAd, first, last, limit, trlink);
		    }
		} else {
		    if(1 < (last - b)) {
			STACK_PUSH5(ISAd, first, a, tr_ilg(a - first), trlink);
			first = b, limit = tr_ilg(last - b);
		    } else if(1 < (a - first)) {
			last = a, limit = tr_ilg(a - first);
		    } else {
			STACK_POP5(ISAd, first, last, limit, trlink);
		    }
		}
	    } else if(limit == -2) {
		/* tandem repeat copy */
		a = stack[--ssize].b, b = stack[ssize].c;
		if(stack[ssize].d == 0) {
		    tr_copy(ISA, SA, first, a, b, last, ISAd - ISA);
		} else {
		    if(0 <= trlink) { stack[trlink].d = -1; }
		    tr_partialcopy(ISA, SA, first, a, b, last, ISAd - ISA);
		}
		STACK_POP5(ISAd, first, last, limit, trlink);
	    } else {
		/* sorted partition */
		if(0 <= *first) {
		    a = first;
		    do { ISA[*a] = a - SA; } while((++a < last) && (0 <= *a));
		    first = a;
		}
		if(first < last) {
		    a = first; do { *a = ~*a; } while(*++a < 0);
		    next = (ISA[*a] != ISAd[*a]) ? tr_ilg(a - first + 1) : -1;
		    if(++a < last) { for(b = first, v = a - SA - 1; b < a; ++b) { ISA[*b] = v; } }

		    /* push */
		    if(trbudget_check(budget, a - first)) {
			if((a - first) <= (last - a)) {
			    STACK_PUSH5(ISAd, a, last, -3, trlink);
			    ISAd += incr, last = a, limit = next;
			} else {
			    if(1 < (last - a)) {
				STACK_PUSH5(ISAd + incr, first, a, next, trlink);
				first = a, limit = -3;
			    } else {
				ISAd += incr, last = a, limit = next;
			    }
			}
		    } else {
			if(0 <= trlink) { stack[trlink].d = -1; }
			if(1 < (last - a)) {
			    first = a, limit = -3;
			} else {
			    STACK_POP5(ISAd, first, last, limit, trlink);
			}
		    }
		} else {
		    STACK_POP5(ISAd, first, last, limit, trlink);
		}
	    }
	    continue;
	}

	if((last - first) <= TR_INSERTIONSORT_THRESHOLD) {
	    tr_insertionsort(ISAd, first, last);
	    limit = -3;
	    continue;
	}

	if(limit-- == 0) {
	    tr_heapsort(ISAd, first, (int32_t)(last - first));
	    for(a = last - 1; first < a; a = b) {
		for(x = ISAd[*a], b = a - 1; (first <= b) && (ISAd[*b] == x); --b) { *b = ~*b; }
	    }
	    limit = -3;
	    continue;
	}

	/* choose pivot */
	a = tr_pivot(ISAd, first, last);
	SWAP(*first, *a);
	v = ISAd[*first];

	/* partition */
	tr_partition(ISAd, first, first + 1, last, &a, &b, v);
	if((last - first) != (b - a)) {
	    next = (ISA[*a] != v) ? tr_ilg((int32_t)(b - a)) : -1;

	    /* update ranks */
	    for(c = first, v = a - SA - 1; c < a; ++c) { ISA[*c] = v; }
	    if(b < last) { for(c = a, v = b - SA - 1; c < b; ++c) { ISA[*c] = v; } }

	    /* push */
	    if((1 < (b - a)) && (trbudget_check(budget, (int32_t)(b - a)))) {
		if((a - first) <= (last - b)) {
		    if((last - b) <= (b - a)) {
			if(1 < (a - first)) {
			    STACK_PUSH5(ISAd + incr, a, b, next, trlink);
			    STACK_PUSH5(ISAd, b, last, limit, trlink);
			    last = a;
			} else if(1 < (last - b)) {
			    STACK_PUSH5(ISAd + incr, a, b, next, trlink);
			    first = b;
			} else {
			    ISAd += incr, first = a, last = b, limit = next;
			}
		    } else if((a - first) <= (b - a)) {
			if(1 < (a - first)) {
			    STACK_PUSH5(ISAd, b, last, limit, trlink);
			    STACK_PUSH5(ISAd + incr, a, b, next, trlink);
			    last = a;
			} else {
			    STACK_PUSH5(ISAd, b, last, limit, trlink);
			    ISAd += incr, first = a, last = b, limit = next;
			}
		    } else {
			STACK_PUSH5(ISAd, b, last, limit, trlink);
			STACK_PUSH5(ISAd, first, a, limit, trlink);
			ISAd += incr, first = a, last = b, limit = next;
		    }
		} else {
		    if((a - first) <= (b - a)) {
			if(1 < (last - b)) {
			    STACK_PUSH5(ISAd + incr, a, b, next, trlink);
			    STACK_PUSH5(ISAd, first, a, limit, trlink);
			    first = b;
			} else if(1 < (a - first)) {
			    STACK_PUSH5(ISAd + incr, a, b, next, trlink);
			    last = a;
			} else {
			    ISAd += incr, first = a, last = b, limit = next;
			}
		    } else if((last - b) <= (b - a)) {
			if(1 < (last - b)) {
			    STACK_PUSH5(ISAd, first, a, limit, trlink);
			    STACK_PUSH5(ISAd + incr, a, b, next, trlink);
			    first = b;
			} else {
			    STACK_PUSH5(ISAd, first, a, limit, trlink);
			    ISAd += incr, first = a, last = b, limit = next;
			}
		    } else {
			STACK_PUSH5(ISAd, first, a, limit, trlink);
			STACK_PUSH5(ISAd, b, last, limit, trlink);
			ISAd += incr, first = a, last = b, limit = next;
		    }
		}
	    } else {
		if((1 < (b - a)) && (0 <= trlink)) { stack[trlink].d = -1; }
		if((a - first) <= (last - b)) {
		    if(1 < (a - first)) {
			STACK_PUSH5(ISAd, b, last, limit, trlink);
			last = a;
		    } else if(1 < (last - b)) {
			first = b;
		    } else {
			STACK_POP5(ISAd, first, last, limit, trlink);
		    }
		} else {
		    if(1 < (last - b)) {
			STACK_PUSH5(ISAd, first, a, limit, trlink);
			first = b;
		    } else if(1 < (a - first)) {
			last = a;
		    } else {
			STACK_POP5(ISAd, first, last, limit, trlink);
		    }
		}
	    }
	} else {
	    if(trbudget_check(budget, (int32_t)(last - first))) {
		limit = tr_ilg((int32_t)(last - first)), ISAd += incr;
	    } else {
		if(0 <= trlink) { stack[trlink].d = -1; }
		STACK_POP5(ISAd, first, last, limit, trlink);
	    }
	}
    }
}



/*---------------------------------------------------------------------------*/

/*- Function -*/

/* Tandem repeat sort */
void
trsort(int32_t *ISA, int32_t *SA, int32_t n, int32_t depth) {
    int32_t *ISAd;
    int32_t *first, *last;
    trbudget_t budget;
    int32_t t, skip, unsorted;

    trbudget_init(&budget, tr_ilg(n) * 2 / 3, n);

    for(ISAd = ISA + depth; -n < *SA; ISAd += ISAd - ISA) {
	first = SA;
	skip = 0;
	unsorted = 0;
	do {
	    if((t = *first) < 0) { first -= t; skip += t; }
	    else {
		if(skip != 0) { *(first + skip) = skip; skip = 0; }
		last = SA + ISA[t] + 1;
		if(1 < (last - first)) {
		    budget.count = 0;
		    tr_introsort(ISA, ISAd, SA, first, last, &budget);
		    if(budget.count != 0) { unsorted += budget.count; }
		    else { skip = (int32_t)(first - last); }
		} else if((last - first) == 1) {
		    skip = -1;
		}
		first = last;
	    }
	} while(first < (SA + n));
	if(skip != 0) { *(first + skip) = skip; }
	if(unsorted == 0) { break; }
    }
}
