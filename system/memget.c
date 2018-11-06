/**
 * @file memget.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009,2018.  All rights reserved. */

#include <stddef.h>
#include <stdint.h>
#include <xinu.h>
#include <CriticalSection.h>
#include <memory.h>

/**
 * @ingroup memory_mgmt
 *
 * Allocate heap memory.
 *
 * @param nbytes
 *      Number of bytes requested.
 *
 * @return
 *      ::SYSERR if @p nbytes was 0 or there is no memory to satisfy the
 *      request; otherwise returns a pointer to the allocated memory region.
 *      The returned pointer is guaranteed to be 8-byte aligned.  Free the block
 *      with memfree() when done with it.
 */
void *memget (unsigned int nbytes)
{
    struct memblock *prev, *curr, *leftover;

    if (0 == nbytes)
    {
        return (void *)SYSERR;
    }

    /* round to multiple of memblock size   */
    nbytes = (unsigned int)roundmb(nbytes);

	ENTER_KERNEL_CRITICAL_SECTION();

    prev = &memlist;
    curr = memlist.next;
    while (curr != NULL)
    {
        if (curr->length == nbytes)
        {
            prev->next = curr->next;
            memlist.length -= nbytes;

			EXIT_KERNEL_CRITICAL_SECTION();
            return (void *)(curr);
        }
        else if (curr->length > nbytes)
        {
            /* split block into two */
            leftover = (struct memblock *)((uintptr_t)curr + nbytes);
            prev->next = leftover;
            leftover->next = curr->next;
            leftover->length = curr->length - nbytes;
            memlist.length -= nbytes;

			EXIT_KERNEL_CRITICAL_SECTION();
            return (void *)(curr);
        }
        prev = curr;
        curr = curr->next;
    }
	EXIT_KERNEL_CRITICAL_SECTION();
    return (void *)SYSERR;
}
