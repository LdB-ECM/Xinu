/**
 * @file memory.h
 * Definitions for kernel memory allocator and maintenance.
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdint.h>
#include <xinu.h>

/* roundmb - round address up to size of memblock  */
#define roundmb(x)  (uintptr_t)( (7 + (unsigned long)(x)) & ~0x07 )
/* truncmb - truncate address down to size of memblock */
#define truncmb(x)  (uintptr_t)( ((unsigned long)(x)) & ~0x07 )

/**
 * @ingroup memory_mgmt
 *
 * Frees memory allocated with stkget().
 *
 * @param p
 *      Pointer to the topmost (highest address) word of the allocated stack (as
 *      returned by stkget()).
 * @param len
 *      Size of the allocated stack, in bytes.  (Same value passed to stkget().)
 */
#define stkfree(p, len) memfree((void *)((unsigned long)(p)         \
                                - (unsigned long)roundmb(len)       \
                                + (unsigned long)sizeof(unsigned long)),    \
                                (unsigned long)roundmb(len))


/**
 * Structure for a block of memory.
 */
struct memblock
{
    struct memblock *next;          /**< pointer to next memory block       */
    unsigned int length;            /**< size of memory block (with struct) */
};

extern struct memblock memlist;     /**< head of free memory list           */

/* Other memory data */

extern void *_end;              /**< linker provides end of image           */
extern void *_etext;            /**< linker provides end of text segment    */
extern void *_start;            /**< linker provides start of code segment  */
extern uintptr_t memheap;       /**< bottom of heap                         */

/* Memory function prototypes */
void *memget(unsigned int);
xinu_syscall memfree(void *, unsigned int);
void *stkget(unsigned int);

#endif                          /* _MEMORY_H_ */
