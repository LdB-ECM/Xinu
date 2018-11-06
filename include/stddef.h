/**
 * @file stddef.h
 *
 * This file is in the process of being turned back to what it
 * is supposed to do under C standard. Much of what was in here
 * has been moved into xinu.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#ifndef _STDDEF_H_
#define _STDDEF_H_
#define _STDDEF_H

/* macro to get offset to struct members */
#ifdef __GNUC__								// GCC already has an inbuilt offsetof function
#define offsetof(TYPE, MEMBER) __builtin_offsetof (TYPE, MEMBER)
#else										// If not on GCC compiler drop thru to old definintions
#define offsetof(type, member) ((size_t)&(((type *)0)->member))
#endif

/* Base type definitions */
#ifdef __GNUC__								// GCC already has an inbuilt typedef for these
typedef __SIZE_TYPE__ size_t;				/**< size_t type definition		*/
typedef __PTRDIFF_TYPE__ ptrdiff_t;			/**< ptrdiff_t type definition	*/
typedef __WCHAR_TYPE__ wchar_t;				/**< wchar_t type definition	*/
#else										// If not on GCC compiler drop thru to old definintions
typedef unsigned long size_t;				/**< size_t type definition		*/
typedef long ptrdiff_t;						/**< ptrdiff_t type definition	*/
typedef unsigned short wchar_t;				/**< wchar_t type definition		*/
#endif

#if defined (_STDDEF_H) || defined (__need_NULL)
#undef NULL		/* in case <stdio.h> has defined it. */
#ifdef __cplusplus
#define NULL ((void *)0)
#else 
#define NULL 0
#endif 
#endif
#undef	__need_NULL

/* Boolean type and constants */
#define FALSE        0          /**< boolean false                      */
#define TRUE         1          /**< boolean true                       */

/* Universal return constants */
#define OK        1             /**< system call ok                     */
#define SYSERR   (-1)           /**< system call failed                 */
#define EOF      (-2)           /**< End-of-file (usually from read)    */
#define TIMEOUT  (-3)           /**< system call timed out              */
#define NOMSG    (-4)           /**< no message to receive              */

#endif                          /* _STDDEF_H_ */
