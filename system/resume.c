/**
 * @file resume.c
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
 * Unsuspend a thread, making it ready
 * @param tid target thread
 * @return priority
 */
xinu_syscall resume (tid_typ tid)
{
    struct thrent *thrptr;     /* thread control block  */
    int prio;

	ENTER_KERNEL_CRITICAL_SECTION();								// We are using thread block so must disable scheduler
    thrptr = &thrtab[tid];											// Fetch thread id refers to
    if (isbadtid(tid) || (thrptr->state != THRSUSP))				// Check the therad ID is still valid
    {
		EXIT_KERNEL_CRITICAL_SECTION();								// We are going to exit so enable switcher again
        return SYSERR;												// Return SYSERR as thread is no longer valid
    }
	thrptr->state = THRREADY;										// Set the thread to ready state
    prio = thrptr->prio;											// Grad the thread priority fro return
	insert(tid, readylist, prio);									// Insert thread into ready list 
	EXIT_KERNEL_CRITICAL_SECTION();									// Ok to allow scheduler to operate
	resched();														// Reschedule tasks
    return prio;													// Return the thread priority
}
