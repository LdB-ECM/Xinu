/**
 * @file recvclr.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <thread.h>
#include <CriticalSection.h>

/**
 * @ingroup threads
 *
 * Clear messages, return waiting message (if any)
 * @return msg if available, NOMSG if no message
 */
xinu_message recvclr (void)
{
    struct thrent *thrptr;
    xinu_message msg;

	ENTER_KERNEL_CRITICAL_SECTION();
    thrptr = &thrtab[thrcurrent];
    if (thrptr->hasmsg)
    {
        msg = thrptr->msg;
    }                           /* retrieve message       */
    else
    {
        msg = NOMSG;
    }
    thrptr->hasmsg = FALSE;     /* reset message flag   */
	EXIT_KERNEL_CRITICAL_SECTION();
    return msg;
}
