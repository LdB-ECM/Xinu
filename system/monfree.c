/**
 * @file monfree.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <monitor.h>
#include <CriticalSection.h>

/**
 * @ingroup monitors
 *
 * Free a monitor previously allocated with moncreate().
 *
 * A monitor must only be freed when no thread has it locked -- that is, either
 * the monitor is unowned, or is owned by a thread that has been killed.
 *
 * @param mon
 *      The monitor to free.
 *
 * @return
 *      ::OK on success; ::SYSERR on failure (@p mon did not specify a valid,
 *      allocated monitor).
 */
xinu_syscall monfree(monitor mon)
{
	xinu_syscall result = SYSERR;
	struct monent *monptr;

	ENTER_KERNEL_CRITICAL_SECTION();

	/* make sure the specified monitor is valid and allocated  */
	if (!isbadmon(mon))
	{
		monptr = &montab[mon];
		if (monptr)
		{
			/* free the monitor's semaphore and mark the monitor table entry as free  */
			if (monptr->sem) semfree(monptr->sem);
			monptr->state = MFREE;
			result = OK;
		}
	}

	EXIT_KERNEL_CRITICAL_SECTION();
    return result;
}
