/**
 * @file rtAlloc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <CriticalSection.h>
#include <route.h>

/**
 * @ingroup route
 *
 * Allocates an entry from the route table.
 * @return entry in route table, NULL if all used, SYSERR if error occurs
 */
struct rtEntry *rtAlloc(void)
{

    RT_TRACE("Allocating route entry");

	ENTER_KERNEL_CRITICAL_SECTION();
    for (int i = 0; i < RT_NENTRY; i++)
    {
        /* If entry is free, return entry */
        if (RT_FREE == rttab[i].state)
        {
            rttab[i].state = RT_PEND;
            RT_TRACE("Free entry %d", i);
			EXIT_KERNEL_CRITICAL_SECTION();
            return &rttab[i];
        }
    }

	EXIT_KERNEL_CRITICAL_SECTION();
    RT_TRACE("No free entry");
    return NULL;
}
