/**
 * @file rawClose.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <raw.h>
#include <stdlib.h>
#include <CriticalSection.h>

/**
 * @ingroup raw
 *
 * Close a raw socket device.
 * @param devptr RAW device table entry
 * @return OK if raw socket is closed properly, otherwise SYSERR
 */
xinu_devcall rawClose(device *devptr)
{
    struct raw *rawptr;

    /* Setup and error check pointers to structures */
    rawptr = &rawtab[devptr->minor];
	ENTER_KERNEL_CRITICAL_SECTION();
    if (RAW_ALLOC != rawptr->state)
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    /* Free semaphore */
    if (SYSERR == semfree(rawptr->isema))
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    /* Free all pending packet buffers */
    while (rawptr->icount > 0)
    {
        netFreebuf(rawptr->in[rawptr->istart]);
        rawptr->istart = (rawptr->istart + 1) % RAW_IBLEN;
        rawptr->icount--;
    }

    bzero(rawptr, sizeof(struct raw));  /* Clear RAW structure.         */
	EXIT_KERNEL_CRITICAL_SECTION();
    return OK;
}
