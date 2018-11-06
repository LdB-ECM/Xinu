/**
 * @file semfree.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <stdbool.h>
#include <xinu.h>
#include <thread.h>
#include <CriticalSection.h>

/**
 * @ingroup semaphores
 *
 * Frees a semaphore.  This can be done even if threads are waiting on it, in
 * which case they will be released and able to run.  However, be careful, since
 * such threads will return from wait()ing on a semaphore that no longer exists
 * and there may be assumptions that no longer hold.
 *
 * @param sem
 *      Semaphore to free (allocated by semcreate()).
 *
 * @return
 *      ::SYSERR if @p sem did not specify a valid semaphore; otherwise ::OK.
 */
xinu_syscall semfree(semaphore sem)
{
    struct sement *semptr;

	ENTER_KERNEL_CRITICAL_SECTION();								// Disable scheduler we are playing with sem table entries
    if (isbadsem(sem))												// Check the semaphore id is valid
    {
		EXIT_KERNEL_CRITICAL_SECTION();								// We are going to exit so enable switcher again
        return SYSERR;												// Sem not valid return system error
    }
    semptr = &semtab[sem];											// Fetch semaphore pointer
    while (nonempty(semptr->queue))
    {
		tid_typ tid;
		tid = dequeue(semptr->queue);								// Free waiting thread
		if (isbadtid(tid) == false)									// Check thread id is not bad .. aka thread id still valid
		{
			struct thrent *thrptr;
			thrptr = &thrtab[tid];									// Fetch thread from id
			thrptr->state = THRREADY;								// Set it to ready state
			insert(tid, readylist, thrptr->prio);					// Insert it into ready list
		}
    }
    semptr->count = 0;												// Set sem count to zero
    semptr->state = SFREE;											// Set the semaphore state to free
	EXIT_KERNEL_CRITICAL_SECTION();									// Ok to allow scheduler to operate
    return OK;
}
