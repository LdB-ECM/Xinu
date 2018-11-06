/**
 * @file unsleep.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <kernel.h>
#include <xinu.h>
#include <CriticalSection.h>
#include <thread.h>
#include <queue.h>
#include <clock.h>

/**
 * @ingroup threads
 *
 * Remove thread from the sleep queue prematurely
 * @param tid  target thread
 * @return OK if thread removed, else SYSERR
 */
xinu_syscall unsleep (tid_typ tid)
{
    struct thrent *thrptr;
    tid_typ next = 0;

	ENTER_KERNEL_CRITICAL_SECTION();

    if (isbadtid(tid))
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    thrptr = &thrtab[tid];
    if ((thrptr->state != THRSLEEP) && (thrptr->state != THRTMOUT))
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    next = quetab[tid].next;
    if (next < NTHREAD)
    {
        quetab[next].key += quetab[tid].key;
    }

    getitem(tid);
	EXIT_KERNEL_CRITICAL_SECTION();
    return OK;
}
