/**
 * @file memfree.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdint.h>
#include <xinu.h>
#include <platform.h>
#include <memory.h>
#include <CriticalSection.h>

/**
 * @ingroup memory_mgmt
 *
 * Frees a block of heap-allocated memory.
 *
 * @param memptr
 *      Pointer to memory block allocated with memget().
 *
 * @param nbytes
 *      Length of memory block, in bytes.  (Same value passed to memget().)
 *
 * @return
 *      ::OK on success; ::SYSERR on failure.  This function can only fail
 *      because of memory corruption or specifying an invalid memory block.
 */
xinu_syscall memfree (void *memptr, unsigned int nbytes)
{
    struct memblock *block, *next, *prev;
    uintptr_t top;

    /* make sure block is in heap */
    if ((0 == nbytes)
        || ((uintptr_t)memptr < (uintptr_t)memheap)
        || ((uintptr_t)memptr > (uintptr_t)platform.maxaddr))
    {
        return SYSERR;
    }

    block = (struct memblock *)memptr;
    nbytes = (unsigned int)roundmb(nbytes);

	ENTER_KERNEL_CRITICAL_SECTION();

    prev = &memlist;
    next = memlist.next;
    while ((next != NULL) && (next < block))
    {
        prev = next;
        next = next->next;
    }

    /* find top of previous memblock */
    if (prev == &memlist)
    {
        top = NULL;
    }
    else
    {
        top = (uintptr_t)prev + prev->length;
    }

    /* make sure block is not overlapping on prev or next blocks */
    if ((top > (uintptr_t)block)
        || ((next != NULL) && ((uintptr_t)block + nbytes) > (uintptr_t)next))
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    memlist.length += nbytes;

    /* coalesce with previous block if adjacent */
    if (top == (uintptr_t)block)
    {
        prev->length += nbytes;
        block = prev;
    }
    else
    {
        block->next = next;
        block->length = nbytes;
        prev->next = block;
    }

    /* coalesce with next block if adjacent */
    if (((uintptr_t)block + block->length) == (uintptr_t)next)
    {
        block->length += next->length;
        block->next = next->next;
    }
	EXIT_KERNEL_CRITICAL_SECTION();
    return OK;
}
