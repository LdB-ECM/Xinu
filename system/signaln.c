/**
 * @file signaln.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <thread.h>
#include <CriticalSection.h>

/**
 * @ingroup semaphores
 *
 * Signal a semaphore @p count times, releasing @p count waiting threads.
 *
 * signaln() may reschedule the currently running thread.  As a result,
 * signaln() should not be called from non-reentrant interrupt handlers unless
 * ::resdefer is set to a positive value at the start of the interrupt handler.
 *
 * @param sem
 *      Semaphore to signal.
 * @param count
 *      Number of times to signal, which must be positive.
 *
 * @return
 *      ::OK on success, ::SYSERR on failure.  This function can only fail if @p
 *      sem did not specify a valid semaphore of if @p count was not positive.
 */
xinu_syscall signaln(semaphore sem, int count)
{
    struct sement *semptr;

	ENTER_KERNEL_CRITICAL_SECTION();								// Disable scheduler we are playing with sem table entries
    if (isbadsem(sem) || (count <= 0))								// Check the semaphore id is valid and count not already negative
    {
		EXIT_KERNEL_CRITICAL_SECTION();								// We are going to exit so enable switcher again
		return SYSERR;												// One of the above error conditions met so return system error
    }
	semptr = &semtab[sem];											// Fetch semaphore pointer
    for (; count > 0; count--)										// Repeat for count
    {
        if ((semptr->count++) < 0)									// If semaphore count has gone negative
        {
			tid_typ tid;
			tid = dequeue(semptr->queue);							// Dequeue a thread waiting on semaphore
			if (isbadtid(tid) == false)								// Check thread id is not bad .. aka thread id still valid
			{
				struct thrent *thrptr;
				thrptr = &thrtab[tid];								// Fetch thread from id
				thrptr->state = THRREADY;							// Set it to ready state
				insert(tid, readylist, thrptr->prio);				// Insert it into ready list
			}
        }
    }
	EXIT_KERNEL_CRITICAL_SECTION();									// Ok to allow scheduler to run now
    resched();														// Call reschedule
	return OK;														// Return OK
}
