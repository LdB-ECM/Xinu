/**
 * @file netDown.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <CriticalSection.h>
#include <network.h>
#include <route.h>
#include <thread.h>

/**
 * @ingroup network
 *
 * Bring down a network interface.
 *
 * @param descrp
 *      Device descriptor for the underlying network device of the interface.
 *
 * @return
 *      ::OK if the interface was successfully brought down; ::SYSERR otherwise.
 *      Currently, ::SYSERR is only returned if no network interface is running
 *      on the device.
 */
xinu_syscall netDown(int descrp)
{
    struct netif *netptr;
    unsigned int i;

	ENTER_KERNEL_CRITICAL_SECTION();

    /* Determine which network interface is running on the underlying device.
     * Return SYSERR if there is none.  */
    netptr = netLookup(descrp);
    if (NULL == netptr)
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    NET_TRACE("Stopping netif %u on device %d", netptr - netiftab, descrp);

    /* Kill receiver threads.  TODO: There is a known bug here: this can kill
     * the receiver threads at inopportune times and leak resources (such as
     * packet buffers allocated with netGetbuf()).  */
    for (i = 0; i < NET_NTHR; i++)
    {
        kill(netptr->recvthr[i]);
    }

    /* Clear all entries in the route table for this network interface.  */
    rtClear(netptr);

    /* Mark interface as free, restore interrupts, and return success.  */
    netptr->state = NET_FREE;
	EXIT_KERNEL_CRITICAL_SECTION();
    return OK;
}
