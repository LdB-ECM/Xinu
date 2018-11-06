/**
 * @file stkget.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <stddef.h>				// Needed for NULL
#include <stdint.h>				// needed for uintptr_t
#include <xinu.h>				// needed for SYSERR, OK
#include <memory.h>
#include <CriticalSection.h>
#include <platform.h>

/**
 * @ingroup memory_mgmt
 *
 * Allocate stack memory.
 *
 * @param nbytes
 *      Number of bytes requested.
 *
 * @return
 *      ::SYSERR if @p nbytes was 0 or there is no memory to satisfy the
 *      request; otherwise returns a pointer to the <b>topmost (highest address)
 *      word</b> of the allocated memory region.  The intention is that this is
 *      the base of a stack growing down.  Free the stack with stkfree() when
 *      done with it.
 */
void* stkget (unsigned int nbytes)
{
    struct memblock *prev, *next, *fits, *fitsprev;

    if (0 == nbytes)
    {
        return (void *)SYSERR;
    }

    /* round to multiple of memblock size   */
    nbytes = (unsigned int)roundmb(nbytes);

	ENTER_KERNEL_CRITICAL_SECTION();

    prev = &memlist;
    next = memlist.next;
    fits = NULL;
    fitsprev = NULL;

    /* scan list for highest block that fits */
    while (next != NULL)
    {
        if (next->length >= nbytes)
        {
            fits = next;
            fitsprev = prev;
        }
        prev = next;
        next = next->next;
    }

    if (NULL == fits)
    {
        /* no block big enough */
		EXIT_KERNEL_CRITICAL_SECTION();
        return (void *)SYSERR;
    }

    if (nbytes == fits->length)
    {
        fitsprev->next = fits->next;
    }
    else
    {
        /* take top portion */
        fits->length -= nbytes;
        fits = (struct memblock *)((uintptr_t)fits + fits->length);
    }

    memlist.length -= nbytes;
	EXIT_KERNEL_CRITICAL_SECTION();
    return (void *)((uintptr_t)fits + nbytes - sizeof(int));
}
