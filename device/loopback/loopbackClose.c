/**
 * @file loopbackClose.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <loopback.h>
#include <stdlib.h>
#include <CriticalSection.h>

/**
 * @ingroup loopback
 *
 * Close a loopback device.
 * @param devptr loopback device table entry
 * @return OK if loopback is closed properly, otherwise SYSERR
 */
xinu_devcall loopbackClose (device *devptr)
{
    struct loopback *lbkptr;

    lbkptr = &looptab[devptr->minor];

	ENTER_KERNEL_CRITICAL_SECTION();
    if (LOOP_STATE_ALLOC != lbkptr->state)
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    /* free the semaphore */
    semfree(lbkptr->sem);
    lbkptr->state = LOOP_STATE_FREE;

	EXIT_KERNEL_CRITICAL_SECTION();
    return OK;
}
