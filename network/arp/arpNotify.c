/**
 * @file arpNotify.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <arp.h>
#include <CriticalSection.h>
#include <stdlib.h>
#include <thread.h>

/**
 * @ingroup arp
 *
 * Send a message to threads waiting on resolution of an ARP table entry.
 * @param entry ARP table entry
 * @param msg message to send to waiting threads
 * @return OK if successful, SYSERR if error occurs
 */
xinu_syscall arpNotify (struct arpEntry *entry, xinu_message msg)
{
    /* Error check pointers */
    if (NULL == entry)
    {
        return SYSERR;
    }

    /* Send message to each waiting thread */
	ENTER_KERNEL_CRITICAL_SECTION();
    for (int i = 0; i < entry->count; i++)
    {
        if (SYSERR == send(entry->waiting[i], msg))
        {
			EXIT_KERNEL_CRITICAL_SECTION();
            return SYSERR;
        }
    }

    /* Clear list of waiting threads */
    entry->count = 0;
    bzero(entry->waiting, sizeof(tid_typ) * ARP_NTHRWAIT);

	EXIT_KERNEL_CRITICAL_SECTION();
    return OK;
}
