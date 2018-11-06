/**
 * @file wakeup.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <thread.h>
#include <queue.h>
#include <clock.h>

#if RTCLOCK == TRUE

/**
 * @ingroup threads
 *
 * Wakeup and ready all threads that have no more time to sleep
 */
void wakeup (void)
{
    while (nonempty(sleepq) && (firstkey(sleepq) <= 0))
    {
        ready(dequeue(sleepq));
    }

    resched();
}

#endif /* RTCLOCK */
