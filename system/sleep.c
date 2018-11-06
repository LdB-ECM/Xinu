/**
 * @file sleep.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <kernel.h>
#include <xinu.h>
#include <CriticalSection.h>
#include <thread.h>
#include <queue.h>
#include <clock.h>

/**
 * @ingroup threads
 *
 * Yields the processor for the specified number of milliseconds, allowing other
 * threads to be scheduled.
 *
 * @param ms number of milliseconds to sleep
 *
 * @return
 *      If successful, the thread will sleep for the specified number of
 *      milliseconds, then return ::OK.  Otherwise, ::SYSERR will be returned.
 *      If a system timer is not supported, ::SYSERR will always returned.
 */
xinu_syscall sleep (unsigned int ms)
{
#if RTCLOCK == TRUE
     unsigned int ticks = 0;

    ticks = (ms * CLKTICKS_PER_SEC) / 1000;

	ENTER_KERNEL_CRITICAL_SECTION();
    if (ticks > 0)
    {
        if (SYSERR == insertd(thrcurrent, sleepq, ticks))
        {
			EXIT_KERNEL_CRITICAL_SECTION();
            return SYSERR;
        }
        thrtab[thrcurrent].state = THRSLEEP;
    }
	EXIT_KERNEL_CRITICAL_SECTION();
    resched();														// Reschedule has its own critical section and we are good to go
    return OK;
#else
    return SYSERR;
#endif
}
