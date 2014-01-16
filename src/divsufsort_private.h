/*
 * divsufsort_private.h for libdivsufsort
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

#ifndef _DIVSUFSORT_PRIVATE_H
#define _DIVSUFSORT_PRIVATE_H 1

#include <stdlib.h>
#include <Python.h>

#include "divsufsort.h"


/*- Constants -*/
#define ALPHABET_SIZE (UINT8_MAX + 1)

/* for divsufsort.c */
#define BUCKET_A_SIZE (ALPHABET_SIZE)
#define BUCKET_B_SIZE (ALPHABET_SIZE * ALPHABET_SIZE)

/* for sssort.c */
#define SS_BLOCKSIZE (0)
#define SS_INSERTIONSORT_THRESHOLD (8)


/* for trsort.c */
#define TR_INSERTIONSORT_THRESHOLD (8)
#define TR_STACKSIZE (64)

#define STACK_SIZE 64

/*- Macros -*/
#define STACK_PUSH(_a, _b, _c, _d)				\
    do {							\
	stack[ssize].a = (_a), stack[ssize].b = (_b),		\
	    stack[ssize].c = (_c), stack[ssize++].d = (_d);	\
    } while(0)

#define STACK_PUSH5(_a, _b, _c, _d, _e)					\
    do {								\
	stack[ssize].a = (_a), stack[ssize].b = (_b),			\
	    stack[ssize].c = (_c), stack[ssize].d = (_d), stack[ssize++].e = (_e); \
    } while(0)

#define STACK_POP(_a, _b, _c, _d)				\
    do {							\
	if(ssize == 0) { return; }				\
	(_a) = stack[--ssize].a, (_b) = stack[ssize].b,		\
	    (_c) = stack[ssize].c, (_d) = stack[ssize].d;	\
    } while(0)

#define STACK_POP5(_a, _b, _c, _d, _e)					\
    do {								\
	if(ssize == 0) { return; }					\
	(_a) = stack[--ssize].a, (_b) = stack[ssize].b,			\
	    (_c) = stack[ssize].c, (_d) = stack[ssize].d, (_e) = stack[ssize].e; \
    } while(0)


#define SWAP(a,b)				\
    do {					\
	typeof(a) temp;				\
	temp = a;				\
	a = b;					\
	b = temp;				\
    } while (0)

/*- Private Prototypes -*/
/* sssort.c */
void
sssort(const uint8_t *Td, const int32_t *PA,
       int32_t *first, int32_t *last,
       int32_t depth, int32_t n, int32_t lastsuffix);

/* trsort.c */
void
trsort(int32_t *ISA, int32_t *SA, int32_t n, int32_t depth);

#endif /* _DIVSUFSORT_PRIVATE_H */
