/**
 * @file ready.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <stdbool.h>		// Needed for bool
#include <thread.h>
#include <queue.h>
#include <CriticalSection.h>

/**
 * @ingroup threads
 *
 * Make a thread eligible for CPU service.
 * @param tid target thread
 * @return OK if thread has been added to readylist, else SYSERR
 */
int ready (tid_typ tid)
{
    struct thrent *thrptr;

	ENTER_KERNEL_CRITICAL_SECTION();								// Disable scheduler we are playing with thread table entries
    if (isbadtid(tid))												// Check if thread id is bad  
    {
		EXIT_KERNEL_CRITICAL_SECTION();								// We are going to exit so enable scheduler again
        return SYSERR;												// Return system error as thread id is bad
    }

    thrptr = &thrtab[tid];											// Fetch thread pointer
    thrptr->state = THRREADY;										// Set thread state to ready
    insert(tid, readylist, thrptr->prio);							// Insert thread into ready queue
	EXIT_KERNEL_CRITICAL_SECTION();									// We are exiting allow scheduler to operate again
    return OK;														// Return OK status
}
