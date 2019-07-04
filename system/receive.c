/**
 * @file receive.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <thread.h>
#include <CriticalSection.h>

/**
 * @ingroup threads
 *
 * receive - wait for a message and return it
 * @return message
 */
xinu_message receive(void)
{
    struct thrent *thrptr;
    xinu_message msg;

	ENTER_KERNEL_CRITICAL_SECTION();
    thrptr = &thrtab[thrcurrent];
    if (FALSE == thrptr->hasmsg)
    {                           /* if no message, wait for one */
        thrptr->state = THRRECV;
		EXIT_KERNEL_CRITICAL_SECTION();
        resched();
		ENTER_KERNEL_CRITICAL_SECTION();
    }
    msg = thrptr->msg;          /* retrieve message                */
    thrptr->hasmsg = FALSE;     /* reset message flag              */
	EXIT_KERNEL_CRITICAL_SECTION();
    return msg;
}
