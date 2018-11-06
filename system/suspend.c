/**
 * @file suspend.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <thread.h>
#include <queue.h>
#include <CriticalSection.h>

/**
 * @ingroup threads
 *
 * Suspend a thread, placing it in hibernation
 * @param tid target thread
 * @return priority or SYSERR
 */
xinu_syscall suspend(tid_typ tid)
{
    register struct thrent *thrptr;     /* thread control block  */

	ENTER_KERNEL_CRITICAL_SECTION();
    if (isbadtid(tid) || (NULLTHREAD == tid))
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }
    thrptr = &thrtab[tid];
    if ((thrptr->state != THRCURR) && (thrptr->state != THRREADY))
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    if (THRREADY == thrptr->state)
    {
        getitem(tid);					// removes from queue
        thrptr->state = THRSUSP;		// Thread now suspended
		EXIT_KERNEL_CRITICAL_SECTION();	// Thread suspended its fine to allow context switch
    }
    else
    {
        thrptr->state = THRSUSP;		// Thread now suspended
		EXIT_KERNEL_CRITICAL_SECTION();	// Reschedule has it's own critical section we must exit
        resched();						// Reschedule and move to next thread
	}
    return thrptr->prio;				// Return thread priority
}
