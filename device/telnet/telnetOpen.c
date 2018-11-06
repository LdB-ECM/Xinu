/**
 * @file telnetOpen.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <telnet.h>
#include <device.h>
#include <Criticalsection.h>
#include <stdarg.h>

#include <stdio.h>

/**
 * @ingroup telnet
 *
 * Associate TELNET with a hardware device.
 * @param devptr TELNET device table entry
 * @param ap 2nd argument is the device number for the hardware device
 * @return OK if TELNET is opened properly, otherwise SYSERR
 */
xinu_devcall telnetOpen(device *devptr, va_list ap)
{

    struct telnet *tntptr = NULL;
    int dvnum = 0;

	ENTER_KERNEL_CRITICAL_SECTION();

    /* Second arg should be device index for physical hardware */
    dvnum = va_arg(ap, int);
    TELNET_TRACE("Open(%d) dvnum = %d", devptr->minor, dvnum);
    if (isbaddev(dvnum))
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    /* Setup pointer to telnet */
    tntptr = &telnettab[devptr->minor];

    /* Check if TELNET is already open */
    if ((TELNET_STATE_FREE != tntptr->state) &&
        (TELNET_STATE_ALLOC != tntptr->state))
    {
        TELNET_TRACE("state = %d", tntptr->state);
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }
    tntptr->state = TELNET_STATE_OPEN;

    /* Initialize input buffer */
    tntptr->istart = 0;
    tntptr->icount = 0;
    tntptr->idelim = FALSE;

    /* Initialize output buffer */
    tntptr->ocount = 0;
    tntptr->ostart = 0;

    /* Initialize flags */
    tntptr->flags = 0;
    tntptr->ieof = FALSE;
    tntptr->phw = (device *)&devtab[dvnum];

    /* Initialize states and mutex semaphoress */
    tntptr->echoState = TELNET_ECHO_SENT_WILL;
    tntptr->isem = semcreate(1);
    tntptr->osem = semcreate(1);
    /* Restore interrupts after making changes to telnet device structure */
	EXIT_KERNEL_CRITICAL_SECTION();
    return OK;
}
