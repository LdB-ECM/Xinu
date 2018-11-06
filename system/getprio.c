/**
 * @file getprio.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <thread.h>
#include <CriticalSection.h>

/**
 * @ingroup threads
 *
 * Return the scheduling priority of a thread
 * @param tid thread ID
 * @return priority of thread on success, SYSERR on failure
 */
xinu_syscall getprio (tid_typ tid)
{
    int prio;

	ENTER_KERNEL_CRITICAL_SECTION();								// Need to disable pre-emptive scheduler
    if (isbadtid(tid))												// Check if thread id is bad 
    {
		EXIT_KERNEL_CRITICAL_SECTION();								// We are going to exit so enable scheduler again
		return SYSERR;												// Return system error as thread id is bad
    }

    prio = thrtab[tid].prio;										// Fetch the thread priority
	EXIT_KERNEL_CRITICAL_SECTION();									// We are exiting allow pre-emptive scheduler to operate
    return prio;													// Return the priority
}
