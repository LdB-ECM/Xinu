/**
 * @file recvtime.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <thread.h>
#include <clock.h>
#include <CriticalSection.h>

/**
 * @ingroup threads
 *
 * wait to receive a message or timeout and return result
 * @param  maxwait ticks to wait before timeout
 * @return msg if becomes available, TIMEOUT if no message
 */
xinu_message recvtime (int maxwait)
{
    struct thrent *thrptr;
    xinu_message msg;

    if (maxwait < 0)
    {
        return SYSERR;
    }
	ENTER_KERNEL_CRITICAL_SECTION();
    thrptr = &thrtab[thrcurrent];
    if (FALSE == thrptr->hasmsg)
    {
#if RTCLOCK == TRUE
        if (SYSERR == insertd(thrcurrent, sleepq, maxwait))
        {
			EXIT_KERNEL_CRITICAL_SECTION();
            return SYSERR;
        }
        thrtab[thrcurrent].state = THRTMOUT;
		EXIT_KERNEL_CRITICAL_SECTION();
        resched();
		ENTER_KERNEL_CRITICAL_SECTION();
#else
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
#endif
    }

    if (thrptr->hasmsg)
    {
        msg = thrptr->msg;      /* retrieve message              */
        thrptr->hasmsg = FALSE; /* reset message flag            */
    }
    else
    {
        msg = TIMEOUT;
    }
	EXIT_KERNEL_CRITICAL_SECTION();
    return msg;
}
