/*
 * sssort.c for libdivsufsort
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


/*- Private Functions -*/

static const int32_t lg_table[256]= {
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
int32_t
ss_ilg(int32_t n) {
    return (n & (int32_t)0xffff0000) ?
	((n & (int32_t)0xff000000) ?
	 24 + lg_table[(n >> 24) & 0xff] :
	 16 + lg_table[(n >> 16) & 0xff]) :
	((n & 0x0000ff00) ?
	 8 + lg_table[(n >>  8) & 0xff] :
	 0 + lg_table[(n >>  0) & 0xff]);
}


/*---------------------------------------------------------------------------*/

/* Compares two suffixes. */
static
int32_t
ss_compare(const uint8_t *T,
           const int32_t *p1, const int32_t *p2,
           int32_t depth) {
    const uint8_t *U1, *U2, *U1n, *U2n;

    for(U1 = T + depth + *p1,
	    U2 = T + depth + *p2,
	    U1n = T + *(p1 + 1) + 2,
	    U2n = T + *(p2 + 1) + 2;
        (U1 < U1n) && (U2 < U2n) && (*U1 == *U2);
        ++U1, ++U2) {
    }

    return U1 < U1n ?
	(U2 < U2n ? *U1 - *U2 : 1) :
	(U2 < U2n ? -1 : 0);
}


/*---------------------------------------------------------------------------*/

/* Insertionsort for small size groups */
static
void
ss_insertionsort(const uint8_t *T, const int32_t *PA,
                 int32_t *first, int32_t *last, int32_t depth) {
    int32_t *i, *j;
    int32_t t;
    int32_t r;

    for(i = last - 2; first <= i; --i) {
        for(t = *i, j = i + 1; 0 < (r = ss_compare(T, PA + t, PA + *j, depth));) {
            do { *(j - 1) = *j; } while((++j < last) && (*j < 0));
            if(last <= j) { break; }
        }
        if(r == 0) { *j = ~*j; }
        *(j - 1) = t;
    }
}


/*---------------------------------------------------------------------------*/

static
void
ss_fixdown(const uint8_t *Td, const int32_t *PA,
           int32_t *SA, long i, long size) {
    long j, k;
    int32_t v;
    int32_t c, d, e;

    for(v = SA[i], c = Td[PA[v]]; (j = 2 * i + 1) < size; SA[i] = SA[k], i = k) {
        d = Td[PA[SA[k = j++]]];
        if(d < (e = Td[PA[SA[j]]])) { k = j; d = e; }
        if(d <= c) { break; }
    }
    SA[i] = v;
}

/* Simple top-down heapsort. */
static
void
ss_heapsort(const uint8_t *Td, const int32_t *PA, int32_t *SA, long size) {
    long i, m;
    int32_t t;

    m = size;
    if((size % 2) == 0) {
        m--;
        if(Td[PA[SA[m / 2]]] < Td[PA[SA[m]]]) { SWAP(SA[m], SA[m / 2]); }
    }

    for(i = m / 2 - 1; 0 <= i; --i) { ss_fixdown(Td, PA, SA, i, m); }
    if((size % 2) == 0) { SWAP(SA[0], SA[m]); ss_fixdown(Td, PA, SA, 0, m); }
    for(i = m - 1; 0 < i; --i) {
        t = SA[0], SA[0] = SA[i];
        ss_fixdown(Td, PA, SA, 0, i);
        SA[i] = t;
    }
}


/*---------------------------------------------------------------------------*/

/* Returns the median of three elements. */
static
int32_t *
ss_median3(const uint8_t *Td, const int32_t *PA,
           int32_t *v1, int32_t *v2, int32_t *v3) {
    if(Td[PA[*v1]] > Td[PA[*v2]]) { SWAP(v1, v2); }
    if(Td[PA[*v2]] > Td[PA[*v3]]) {
        if(Td[PA[*v1]] > Td[PA[*v3]]) { return v1; }
        else { return v3; }
    }
    return v2;
}

/* Returns the median of five elements. */
static
int32_t *
ss_median5(const uint8_t *Td, const int32_t *PA,
           int32_t *v1, int32_t *v2, int32_t *v3, int32_t *v4, int32_t *v5) {
    if(Td[PA[*v2]] > Td[PA[*v3]]) { SWAP(v2, v3); }
    if(Td[PA[*v4]] > Td[PA[*v5]]) { SWAP(v4, v5); }
    if(Td[PA[*v2]] > Td[PA[*v4]]) { SWAP(v2, v4); SWAP(v3, v5); }
    if(Td[PA[*v1]] > Td[PA[*v3]]) { SWAP(v1, v3); }
    if(Td[PA[*v1]] > Td[PA[*v4]]) { SWAP(v1, v4); SWAP(v3, v5); }
    if(Td[PA[*v3]] > Td[PA[*v4]]) { return v4; }
    return v3;
}

/* Returns the pivot element. */
static
int32_t *
ss_pivot(const uint8_t *Td, const int32_t *PA, int32_t *first, int32_t *last) {
    int32_t *middle;
    long t;

    t = last - first;
    middle = first + t / 2;

    if(t <= 512) {
        if(t <= 32) {
            return ss_median3(Td, PA, first, middle, last - 1);
        } else {
            t >>= 2;
            return ss_median5(Td, PA, first, first + t, middle, last - 1 - t, last - 1);
        }
    }
    t >>= 3;
    first  = ss_median3(Td, PA, first, first + t, first + (t << 1));
    middle = ss_median3(Td, PA, middle - t, middle, middle + t);
    last   = ss_median3(Td, PA, last - 1 - (t << 1), last - 1 - t, last - 1);
    return ss_median3(Td, PA, first, middle, last);
}


/*---------------------------------------------------------------------------*/

/* Binary partition for substrings. */
static
int32_t *
ss_partition(const int32_t *PA,
             int32_t *first, int32_t *last, int32_t depth) {
    int32_t *a, *b;
    int32_t t;
    for(a = first - 1, b = last;;) {
        for(; (++a < b) && ((PA[*a] + depth) >= (PA[*a + 1] + 1));) { *a = ~*a; }
        for(; (a < --b) && ((PA[*b] + depth) <  (PA[*b + 1] + 1));) { }
        if(b <= a) { break; }
        t = ~*b;
        *b = *a;
        *a = t;
    }
    if(first < a) { *first = ~*first; }
    return a;
}

/* Multikey introsort for medium size groups. */
static
void
ss_mintrosort(const uint8_t *T,	const int32_t *PA, int32_t* first, int32_t* last, int32_t depth) {
    struct {
        int32_t *a, *b, c;
        int32_t d;
    } stack[STACK_SIZE];
    const uint8_t *Td;
    int32_t *a, *b, *c, *d, *e, *f;
    int32_t s;
    int32_t t;
    int32_t ssize;
    long limit;
    int32_t v, x = 0;

    for(ssize = 0, limit = ss_ilg((int32_t)(last - first));;) {

        if((last - first) <= SS_INSERTIONSORT_THRESHOLD) {
            if(1 < (last - first))
                ss_insertionsort(T, PA, first, last, depth);
            STACK_POP(first, last, depth, limit);
            continue;
        }

        Td = T + depth;
        if(limit-- == 0)
            ss_heapsort(Td, PA, first, last - first);
        if(limit < 0) {
            for(a = first + 1, v = Td[PA[*first]]; a < last; ++a) {
                if((x = Td[PA[*a]]) != v) {
                    if(1 < (a - first)) { break; }
                    v = x;
                    first = a;
                }
            }
            if(Td[PA[*first] - 1] < v) {
                first = ss_partition(PA, first, a, depth);
            }
            if((a - first) <= (last - a)) {
                if(1 < (a - first)) {
                    STACK_PUSH(a, last, depth, -1);
                    last = a;
                    depth += 1;
                    limit = ss_ilg((int32_t)(a - first));
                } else {
                    first = a, limit = -1;
                }
            } else {
                if(1 < (last - a)) {
                    STACK_PUSH(first, a, depth + 1, ss_ilg((int32_t)(a - first)));
                    first = a, limit = -1;
                } else {
                    last = a;
                    depth += 1;
                    limit = ss_ilg((int32_t)(a - first));
                }
            }
            continue;
        }

        /* choose pivot */
        a = ss_pivot(Td, PA, first, last);
        v = Td[PA[*a]];
        SWAP(*first, *a);

        /* partition */
        for(b = first; (++b < last) && ((x = Td[PA[*b]]) == v);) { }
        if(((a = b) < last) && (x < v)) {
            for(; (++b < last) && ((x = Td[PA[*b]]) <= v);) {
                if(x == v) { SWAP(*b, *a); ++a; }
            }
        }
        for(c = last; (b < --c) && ((x = Td[PA[*c]]) == v);) { }
        if((b < (d = c)) && (x > v)) {
            for(; (b < --c) && ((x = Td[PA[*c]]) >= v);) {
                if(x == v) { SWAP(*c, *d); --d; }
            }
        }
        for(; b < c;) {
            SWAP(*b, *c);
            for(; (++b < c) && ((x = Td[PA[*b]]) <= v);) {
                if(x == v) { SWAP(*b, *a); ++a; }
            }
            for(; (b < --c) && ((x = Td[PA[*c]]) >= v);) {
                if(x == v) { SWAP(*c, *d); --d; }
            }
        }

        if(a <= d) {
            c = b - 1;
            s = (int32_t)(a - first);
            t = (int32_t)(b - a);
            if(s > t)
                s = t;
            for(e = first, f = b - s; 0 < s; --s, ++e, ++f) { SWAP(*e, *f); }
            s = (int32_t)(d - c);
            t = (int32_t)(last - d - 1);
            if(s > t)
                s = t;
            for(e = b, f = last - s; 0 < s; --s, ++e, ++f) { SWAP(*e, *f); }

            a = first + (b - a), c = last - (d - c);
            b = (v <= Td[PA[*a] - 1]) ? a : ss_partition(PA, a, c, depth);

            if((a - first) <= (last - c)) {
                if((last - c) <= (c - b)) {
                    STACK_PUSH(b, c, depth + 1, ss_ilg((int32_t)(c - b)));
                    STACK_PUSH(c, last, depth, (int32_t)limit);
                    last = a;
                } else if((a - first) <= (c - b)) {
                    STACK_PUSH(c, last, depth, (int32_t)limit);
                    STACK_PUSH(b, c, depth + 1, ss_ilg((int32_t)(c - b)));
                    last = a;
                } else {
                    STACK_PUSH(c, last, depth, (int32_t)limit);
                    STACK_PUSH(first, a, depth, (int32_t)limit);
                    first = b, last = c, depth += 1, limit = ss_ilg((int32_t)(c - b));
                }
            } else {
                if((a - first) <= (c - b)) {
                    STACK_PUSH(b, c, depth + 1, ss_ilg((int32_t)(c - b)));
                    STACK_PUSH(first, a, depth, (int32_t)limit);
                    first = c;
                } else if((last - c) <= (c - b)) {
                    STACK_PUSH(first, a, depth, (int32_t)limit);
                    STACK_PUSH(b, c, depth + 1, ss_ilg((int32_t)(c - b)));
                    first = c;
                } else {
                    STACK_PUSH(first, a, depth, (int32_t)limit);
                    STACK_PUSH(c, last, depth, (int32_t)limit);
                    first = b;
                    last = c;
                    depth += 1;
                    limit = ss_ilg((int32_t)(c - b));
                }
            }
        } else {
            limit += 1;
            if(Td[PA[*first] - 1] < v) {
                first = ss_partition(PA, first, last, depth);
                limit = ss_ilg((int32_t)(last - first));
            }
            depth += 1;
        }
    }
}



/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/

/*- Function -*/

/* Substring sort */
void
sssort(const uint8_t *T, const int32_t *PA,
       int32_t *first, int32_t *last,
       int32_t depth, int32_t n, int32_t lastsuffix) {
    int32_t *a;
    int32_t i;

    if(lastsuffix != 0) { ++first; }

    ss_mintrosort(T, PA, first, last, depth);
    if(lastsuffix != 0) {
        /* Insert last type B* suffix. */
        int32_t PAi[2]; PAi[0] = PA[*(first - 1)], PAi[1] = n - 2;
        for(a = first, i = *(first - 1);
            (a < last) && ((*a < 0) || (0 < ss_compare(T, &(PAi[0]), PA + *a, depth)));
            ++a) {
            *(a - 1) = *a;
        }
        *(a - 1) = i;
    }
}
