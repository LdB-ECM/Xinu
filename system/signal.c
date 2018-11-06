/**
 * @file signal.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <thread.h>
#include <CriticalSection.h>

/**
 * @ingroup semaphores
 *
 * Signal a semaphore, releasing up to one waiting thread.
 *
 * signal() may reschedule the currently running thread.  As a result, signal()
 * should not be called from non-reentrant interrupt handlers unless ::resdefer
 * is set to a positive value at the start of the interrupt handler.
 *
 * @param sem
 *      Semaphore to signal.
 *
 * @return
 *      ::OK on success, ::SYSERR on failure.  This function can only fail if @p
 *      sem did not specify a valid semaphore.
 */
xinu_syscall signal (semaphore sem)
{
    struct sement *semptr;

	ENTER_KERNEL_CRITICAL_SECTION();								// Disable scheduler we are playing with sem table entries
    if (isbadsem(sem))												// Check the semaphore id is valid
    {
		EXIT_KERNEL_CRITICAL_SECTION();								// We are going to exit so enable switcher again
		return SYSERR;												// Sem not valid return system error
    }
    semptr = &semtab[sem];											// Fetch semaphore pointer
    if ((semptr->count++) < 0)										// If count is negative
    {
		tid_typ tid;
		tid = dequeue(semptr->queue);								// Dequeue a thread waiting on semaphore
		if (isbadtid(tid) == false)									// Check thread id is not bad .. aka thread id still valid
		{
			struct thrent *thrptr;
			thrptr = &thrtab[tid];									// Fetch thread from id
			thrptr->state = THRREADY;								// Set it to ready state
			insert(tid, readylist, thrptr->prio);					// Insert it into ready list
		}
		EXIT_KERNEL_CRITICAL_SECTION();								// We have finished with critical section
		resched();													// Run reschedule now
	}
	else {
		EXIT_KERNEL_CRITICAL_SECTION();								// Exiting allow scheduler to operate
	}
    return OK;														// Return OK
}
