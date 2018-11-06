/**
 * @file ethloopClose.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <bufpool.h>
#include <device.h>
#include <ethloop.h>
#include <CriticalSection.h>
#include <semaphore.h>

/**
 * @ingroup ethloop
 *
 * Close a ethloop device.
 * @param devptr ethloop device table entry
 * @return OK if ethloop is closed properly, otherwise SYSERR
 */
xinu_devcall ethloopClose (device *devptr)
{
    struct ethloop *elpptr;

    elpptr = &elooptab[devptr->minor];
	ENTER_KERNEL_CRITICAL_SECTION();

    /* Make sure the ethloop is actually open */
    if (ELOOP_STATE_ALLOC != elpptr->state)
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    /* free the semaphores */
    semfree(elpptr->sem);
    semfree(elpptr->hsem);

    /* free the buffer pool */
    bfpfree(elpptr->poolid);

    /* mark as not open */
    elpptr->dev = NULL;
    elpptr->state = ELOOP_STATE_FREE;
	EXIT_KERNEL_CRITICAL_SECTION();
    return OK;
}
