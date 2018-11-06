/**
 * @file     loopbackOpen.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <loopback.h>
#include <stdlib.h>
#include <CriticalSection.h>

/**
 * @ingroup loopback
 *
 * Open a loopback device. 
 * @param devptr loopback device table entry
 * @return OK if loopback is opened properly, otherwise SYSERR
 */
xinu_devcall loopbackOpen (device *devptr, va_list ap)
{
    struct loopback *lbkptr;
 
    lbkptr = &looptab[devptr->minor];

	ENTER_KERNEL_CRITICAL_SECTION();
    /* Check if loopback is already open */
    if (LOOP_STATE_FREE != lbkptr->state)
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    /* Create new semaphore */
    lbkptr->sem = semcreate(0);

    if (SYSERR == lbkptr->sem)
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    /* Zero out the buffer */
    bzero(lbkptr->buffer, LOOP_BUFFER);

    /* Zero out flags */
    lbkptr->flags = 0;

    lbkptr->state = LOOP_STATE_ALLOC;

	EXIT_KERNEL_CRITICAL_SECTION();
    return OK;
}
