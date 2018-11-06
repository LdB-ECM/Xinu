/**
 * @file rtClear.c
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
 * Removes all entries for a network interface from the routing table. 
 * @param nif network interface 
 * @return OK if entries are removed successfully, otherwise SYSERR
 */
xinu_syscall rtClear(struct netif *nif)
{
    int i;

    /* Error check pointers */
    if (NULL == nif)
    {
        return SYSERR;
    }

	ENTER_KERNEL_CRITICAL_SECTION();
    for (i = 0; i < RT_NENTRY; i++)
    {
        if ((RT_USED == rttab[i].state) && (nif == rttab[i].nif))
        {
            rttab[i].state = RT_FREE;
            rttab[i].nif = NULL;
        }
    }
	EXIT_KERNEL_CRITICAL_SECTION();
    return OK;
}
