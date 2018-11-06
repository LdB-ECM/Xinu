/**
 * @file stdlib.h
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#ifndef _STDLIB_H_
#define _STDLIB_H_

#define __need_size_t
#define __need_wchar_t
#define __need_NULL
#include <stddef.h>

#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e)*1234; }))

/* &a[0] degrades to a pointer: a different type from an array */
#define __must_be_array(a)  BUILD_BUG_ON_ZERO(__builtin_types_compatible_p(typeof(a), typeof(&a[0])))

#define _countof(arr) ((sizeof(arr) / sizeof(*arr)) + __must_be_array(arr))

int abs(int j);
long labs(long j);
int atoi(const char *nptr);
long atol(const char *nptr);
void bzero(void *s, size_t n);
void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void*));
int rand(void);
void srand(unsigned int seed);
void *malloc(size_t size);
void free(void *ptr);

#define RAND_MAX 32767

#endif /* _STDLIB_H_ */
