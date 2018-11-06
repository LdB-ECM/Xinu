/* @file snoopOpen.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <CriticalSection.h>
#include <network.h>
#include <snoop.h>

/**
 * @ingroup snoop
 *
 * Opens a capture from a network device.
 * @param cap pointer to capture structure
 * @param name of underlying device, ALL for all network devices
 * @return OK if open was successful, otherwise SYSERR
 * @pre-condition filter settings should already be setup in cap
 */
int snoopOpen(struct snoop *cap, char *devname)
{
    int i;
    int count = 0;
    int devnum;

    /* Error check pointers */
    if ((NULL == cap) || (NULL == devname))
    {
        return SYSERR;
    }

    SNOOP_TRACE("Opening capture on %s", devname);

    /* Reset statistics */
    cap->ncap = 0;
    cap->nmatch = 0;
    cap->novrn = 0;

    /* Allocated mailbox for queue packets */
    cap->queue = mailboxAlloc(SNOOP_QLEN);
    if (SYSERR == (int)cap->queue)
    {
        SNOOP_TRACE("Failed to allocate mailbox");
        return SYSERR;
    }

    /* Attach capture to all running network interfaces for devname "ALL" */
    if (0 == strcmp(devname, "ALL"))
    {
		ENTER_KERNEL_CRITICAL_SECTION();
#if NNETIF
        for (i = 0; i < NNETIF; i++)
        {
            if (NET_ALLOC == netiftab[i].state)
            {
                netiftab[i].capture = cap;
                count++;
                SNOOP_TRACE("Attached capture to interface %d", i);
            }
        }
#endif
		EXIT_KERNEL_CRITICAL_SECTION();
        if (0 == count)
        {
            SNOOP_TRACE("Capture not attached to any interface");
            mailboxFree(cap->queue);
            return SYSERR;
        }
        return OK;
    }

    /* Determine network interface to attach capture to */
    devnum = getdev(devname);
    if (SYSERR == devnum)
    {
        SNOOP_TRACE("Invalid device");
        mailboxFree(cap->queue);
        return SYSERR;
    }
	ENTER_KERNEL_CRITICAL_SECTION();
#if NNETIF
    for (i = 0; i < NNETIF; i++)
    {
        if ((NET_ALLOC == netiftab[i].state)
            && (netiftab[i].dev == devnum))
        {
            netiftab[i].capture = cap;
			EXIT_KERNEL_CRITICAL_SECTION();
            SNOOP_TRACE("Attached capture to interface %d", i);
            return OK;
        }
    }
#endif

    /* No network interface found */
	EXIT_KERNEL_CRITICAL_SECTION();
    SNOOP_TRACE("No network interface found");
    mailboxFree(cap->queue);
    return SYSERR;
}
