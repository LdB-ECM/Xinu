/**
 * @file moncreate.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <xinu.h>
#include <monitor.h>
#include <CriticalSection.h>

static monitor monalloc (void);

/**
 * @ingroup monitors
 *
 * Create and initialize a new monitor.
 *
 * @return
 *      On success, returns the new monitor.  On failure (system is out of
 *      monitors or semaphores), returns ::SYSERR.
 */
monitor moncreate (void)
{
    monitor mon;
    struct monent *monptr;

    /* Disable interrupts.  */
	ENTER_KERNEL_CRITICAL_SECTION();

    /* Allocate entry in monitor table.  */
    mon = monalloc();

    if (SYSERR != mon)
    {
        monptr = &montab[mon];
		if (monptr) 
		{
			/* Monitors initially have no owner and zero count.  */
			monptr->owner = NOOWNER;
			monptr->count = 0;

			/* Initialize the monitor's semaphore with a count of 1, allowing one
			* thread to acquire the monitor.  */
			EXIT_KERNEL_CRITICAL_SECTION();
			monptr->sem = semcreate(1);
			ENTER_KERNEL_CRITICAL_SECTION();
			if (SYSERR == monptr->sem)
			{
				monptr->state = MFREE;
				mon = SYSERR;
			}
		}
		else mon = SYSERR;
    }

    /* Restore interrupts.  */
	EXIT_KERNEL_CRITICAL_SECTION();

    /* Return monitor table index or SYSERR.  */
    return mon;
}

/* Returns the index of an unused monitor table entry, or SYSERR if none are
 * available.  */
static monitor monalloc (void)
{
    static int nextmon = 0;
	/* Check all NMON slots, starting at 1 past the last slot searched.  */
	for (int i = 0; i < NMON; i++)
	{
		nextmon = (nextmon + 1);
		if (nextmon >= NMON) nextmon = 0;
		if (MFREE == montab[nextmon].state)
		{
			montab[nextmon].state = MUSED;
			return nextmon;
		}
	}
    return SYSERR;
}
