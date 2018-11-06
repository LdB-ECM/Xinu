/**
 * @file rtRecv.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <CriticalSection.h>
#include <mailbox.h>
#include <network.h>
#include <route.h>

/**
 * @ingroup route
 */
xinu_syscall rtRecv(struct packet *pkt)
{

    /* Error check pointers */
    if (NULL == pkt)
    {
        return SYSERR;
    }

    /* If route queue is full, then drop packet */
	ENTER_KERNEL_CRITICAL_SECTION();
    if (mailboxCount(rtqueue) >= RT_NQUEUE)
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        RT_TRACE("Route queue full");
        netFreebuf(pkt);
        return OK;
    }

    /* Place packet in queue */
    if (SYSERR == mailboxSend(rtqueue, (int)pkt))
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        RT_TRACE("Failed to enqueue packet");
        netFreebuf(pkt);
        return SYSERR;
    }

	EXIT_KERNEL_CRITICAL_SECTION();
    RT_TRACE("Enqueued packet for routing");
    return OK;
}
