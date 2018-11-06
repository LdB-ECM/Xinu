/**
 * @file lock.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <monitor.h>
#include <CriticalSection.h>

/**
 * @ingroup monitors
 *
 * Lock a monitor.
 *
 * If no thread owns the monitor, its owner is set to the current thread and its
 * count is set to 1.
 *
 * If the current thread already owns the monitor, its count is incremented and
 * no further action is taken.
 *
 * If another thread owns the monitor, the current thread waits for the monitor
 * to become fully unlocked by that thread, then sets its owner to the current
 * thread and its count to 1.
 *
 * @param mon
 *      The monitor to lock.
 *
 * @return
 *      ::OK on success; ::SYSERR on failure (@p mon did not specify a valid,
 *      allocated monitor).
 */
xinu_syscall lock (monitor mon)
{
    struct monent *monptr;

	ENTER_KERNEL_CRITICAL_SECTION();
    if (isbadmon(mon))
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    monptr = &montab[mon];
	if (monptr == 0) 
	{
		EXIT_KERNEL_CRITICAL_SECTION();
		return SYSERR;
	}

    /* if no thread owns the lock, the current thread claims it */
    if (NOOWNER == monptr->owner)
    {
        monptr->owner = thrcurrent;     /* current thread now owns the lock  */
        (monptr->count)++;      /* add 1 "lock" to the monitor's count */
        wait(monptr->sem);      /* this thread owns the semaphore      */
    }
    else
    {
        /* if current thread owns the lock increase count; dont wait on sem */
        if (thrcurrent == monptr->owner)
        {
            (monptr->count)++;
        }
        /* if another thread owns the lock, wait on sem until monitor is free */
        else
        {
            wait(monptr->sem);
            monptr->owner = thrcurrent;
            (monptr->count)++;

        }
    }

	EXIT_KERNEL_CRITICAL_SECTION();
    return OK;
}
