/**
 * @file send.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <thread.h>
#include <CriticalSection.h>

/**
 * @ingroup threads
 *
 * Send a message to another thread
 * @param tid thread id of recipient
 * @param msg contents of message
 * @return OK on success, SYSERR on failure
 */
xinu_syscall send (tid_typ tid, xinu_message msg)
{
    register struct thrent *thrptr;

	ENTER_KERNEL_CRITICAL_SECTION();								// Need to disable scheduler we are using thread table
    if (isbadtid(tid))												// Check if thread id is bad 
    {
		EXIT_KERNEL_CRITICAL_SECTION();								// We are going to exit so enable scheduler again
        return SYSERR;												// Return system error as thread id is bad
    }
    thrptr = &thrtab[tid];											// Fetch the thread id
    if ((THRFREE == thrptr->state) || thrptr->hasmsg)				// Check thread isn't free and does not already have message
    {
		EXIT_KERNEL_CRITICAL_SECTION();								// We are going to exit so enable scheduler again
        return SYSERR;												// Return system error as thread is free or has message already
    }
    thrptr->msg = msg;												// Deposit message
    thrptr->hasmsg = TRUE;											// Raise message flag

    if (THRRECV == thrptr->state)
    {
		thrptr->state = THRREADY;									// Set thread state to ready
		insert(tid, readylist, thrptr->prio);						// Insert thread in ready list
		EXIT_KERNEL_CRITICAL_SECTION();								// Ok to allow scheduler to operate						
		resched();													// Reshedule
    }
    else if (THRTMOUT == thrptr->state)								// Thread has timeout we need to unsleep it
    {
		tid_typ next;

		next = quetab[tid].next;									// Fetch next  thread
		if (next < NTHREAD)											// If next id is valid
		{
			quetab[next].key += quetab[tid].key;					// Set next key to our key
		}
		getitem(tid);												// Remove us from list
		thrptr->state = THRREADY;									// Set thread state to ready
		insert(tid, readylist, thrptr->prio);						// Insert thread in ready list
		EXIT_KERNEL_CRITICAL_SECTION();								// Ok to allow scheduler to operate						
		resched();													// Reshedule
	}
	else {
		EXIT_KERNEL_CRITICAL_SECTION();								// We are exiting allow scheduler to operate
	}
    return OK;														// Return OK status
}
