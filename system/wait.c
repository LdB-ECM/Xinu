/**
 * @file wait.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <CriticalSection.h>
#include <xinu.h>
#include <thread.h>

/**
 * @ingroup semaphores
 *
 * Wait on a semaphore.
 *
 * If the semaphore's count is positive, it will be decremented and this
 * function will return immediately.  Otherwise, the currently running thread
 * will be put to sleep until the semaphore is signaled with signal() or
 * signaln(), or freed with semfree().
 *
 * @param sem
 *      Semaphore to wait on.
 *
 * @return
 *      ::OK on success; ::SYSERR on failure.  This function can only fail if @p
 *      sem did not specify a valid semaphore.
 */
xinu_syscall wait(semaphore sem)
{
    struct sement *semptr;
    struct thrent *thrptr;

	ENTER_KERNEL_CRITICAL_SECTION();
    if (isbadsem(sem))
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }
    thrptr = &thrtab[thrcurrent];
    semptr = &semtab[sem];
    if (--(semptr->count) < 0)
    {
        thrptr->state = THRWAIT;
        thrptr->sem = sem;
        enqueue(thrcurrent, semptr->queue);
		EXIT_KERNEL_CRITICAL_SECTION();				// Reschedule has it's own crit section so we must exit
        resched();
	}
	else {
		EXIT_KERNEL_CRITICAL_SECTION();
	}
    return OK;
}
