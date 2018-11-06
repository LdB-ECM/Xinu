/**
 * @file telnetFlush.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <CriticalSection.h>
#include <telnet.h>

/**
 * @ingroup telnet
 *
 * Flushes the telnet server's output buffer
 * @param devptr TELNET device table entry
 * @return OK if flush is successful, SYSERR on failure
 */
xinu_devcall telnetFlush(device *devptr)
{
    struct telnet *tntptr;
    device *phw;

    tntptr = &telnettab[devptr->minor];
    phw = tntptr->phw;
	ENTER_KERNEL_CRITICAL_SECTION();

    if (NULL == phw)
    {
        return SYSERR;
    }

    if (TELNET_STATE_OPEN != tntptr->state)
    {
        return SYSERR;
    }

    if (tntptr->ostart > 0)
    {
        if (SYSERR ==
            (*phw->write) (phw, (void *)(tntptr->out), tntptr->ostart))
        {
			EXIT_KERNEL_CRITICAL_SECTION();
            return SYSERR;
        }

        tntptr->ostart = 0;
    }

	EXIT_KERNEL_CRITICAL_SECTION();

    return OK;
}
