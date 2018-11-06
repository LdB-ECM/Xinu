/**
 * @file     netLookup.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <raw.h>

/**
 * @ingroup network
 *
 * Lookup a network interface, given an underlying device.
 * @param devnum device number to find netif for.
 * @return network interface, NULL if none exists
 */
struct netif *netLookup(int devnum)
{
#if NNETIF
    for (int i = 0; i < NNETIF; i++)
    {
        /* Check if network interface is allocated and device matches */
        if ((NET_ALLOC == netiftab[i].state)
            && (netiftab[i].dev == devnum))
        {
            return &netiftab[i];
        }
    }
#endif
    return NULL;
}
