/**
 * @file rtRemove.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <CriticalSection.h>
#include <network.h>
#include <route.h>

/**
 * @ingroup route
 *
 * Removes any entry based on its destination.
 * @param dst destination IP address
 * @return OK if the entry is removed successfully, otherwise SYSERR
 */
xinu_syscall rtRemove(const struct netaddr *dst)
{
    int i;

    /* Error check pointers */
    if (NULL == dst)
    {
        return SYSERR;
    }

	ENTER_KERNEL_CRITICAL_SECTION();
    for (i = 0; i < RT_NENTRY; i++)
    {
        if ((RT_USED == rttab[i].state)
            && netaddrequal(dst, &rttab[i].dst))
        {
            rttab[i].state = RT_FREE;
            rttab[i].nif = NULL;
        }
    }
	EXIT_KERNEL_CRITICAL_SECTION();
    return OK;
}
