/**
 * @file kernel.h 
 *
 * The base include file for the Xinu kernel. Defines symbolic constants,
 * universal return constants, intialization constants, machine size
 * definitions, inline utility functions, and include types
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <xinu.h>
#include <conf.h>
#include <stdarg.h>
#include <compiler.h>

/* Kernel function prototypes */
void nulluser(void);

/* Kernel function prototypes */
xinu_syscall kprintf(const char *fmt, ...);
xinu_syscall kputc (char);
xinu_syscall kgetc(void);
xinu_syscall kvprintf(const char *fmt, va_list va);

/** Type-independent macro to calculate the minimum of 2 values.  */
#define min(a, b) ({ typeof(a) _a = (a); typeof(b) _b = (b); \
                            (_a < _b) ? _a : _b; })

/** Type-independent macro to calculate the maximum of 2 values.  */
#define max(a, b) ({ typeof(a) _a = (a); typeof(b) _b = (b); \
                            (_a > _b) ? _a : _b; })

/** Perform integer division, rounding up the quotient.  */
#define DIV_ROUND_UP(num, denom) (((num) + (denom) - 1) / (denom))

/** Get the number of elements in an array (not dynamically allocated)  */
#define ARRAY_LEN(array) (sizeof(array) / sizeof((array)[0]))

#endif                          /* _KERNEL_H_ */
