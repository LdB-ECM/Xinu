/**
 * @file resched.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <thread.h>
#include <clock.h>
#include <queue.h>
#include <memory.h>

extern void ctxsw(void *, void *);

int resdefer = 0;				/* >0 if rescheduling deferred */

/**
 * @ingroup threads
 *
 * Reschedule processor to highest priority ready thread.
 * Upon entry, thrcurrent gives current thread id.
 * Threadtab[thrcurrent].pstate gives correct NEXT state
 * for current thread if other than THRREADY.
 * @return OK when the thread is context switched back
 */
int resched (void)
{
    struct thrent *throld;      /* old thread entry */
    struct thrent *thrnew;      /* new thread entry */

    if (resdefer > 0)												// If reschedule deferred
    {											 
        resdefer++;													// Increment count
        return (OK);												// Return back to the thread
    }

    throld = &thrtab[thrcurrent];									// Current thread pointer is thread at the current thread id
    throld->intmask = disable();									// Save its interrupt masks
    if (THRCURR == throld->state)									// Check the thread state is current
    {
        if (nonempty(readylist) &&									// If non threads in ready list 
			(throld->prio > firstkey(readylist)))					// OR current thread priority greater than first on ready list
        {
            restore(throld->intmask);								// Restore the interrupt mask
            return OK;												// Return back to the thread
        }
        throld->state = THRREADY;									// Set the thread state to ready
        insert(thrcurrent, readylist, throld->prio);				// Insert the thread into ready list with it's current priority
    }

    /* get highest priority thread from ready list */
    thrcurrent = dequeue(readylist);								// Dequeue the thread we are switching to
    thrnew = &thrtab[thrcurrent];									// Retrieve the pointer to that new thread we are switching to 									
    thrnew->state = THRCURR;										// Set the new thread state to current

    /* change address space identifier to thread id */
   // asid = thrcurrent & 0xff;										// Address space identifier
	ctxsw(&throld->stkptr, &thrnew->stkptr);						// Call the context switch

    /* old thread returns here when resumed */
    restore(throld->intmask);										// Restore the interrupt masks
    return OK;
}
