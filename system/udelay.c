/**
 * @file udelay.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#if RTCLOCK == TRUE

#include <clock.h>
#include <platform.h>

/* The below udelay() implementation is platform-independent and only depends on
 * clkcount() being implemented by the platform-specific code.  However it does
 * assume that the platform clock frequency is an even multiple of 1000000.  */

/**
 * @ingroup timer
 *
 * Busy-waits for a number of microseconds.
 * Only use this function if absolutely necessary.  Normally you should call
 * sleep() so that other threads can use the processor immediately.
 *
 * @param us
 *    Number of microseconds to wait for.
 */
void udelay (unsigned long us)
{
    /* delay = Number of timer ticks to wait for  */
    unsigned long delay = (platform.clkfreq / 1000000) * us;

    /* start = Starting tick count  */
    unsigned long start = clkcount();

    /* end = Ending tick count (may have wrapped around)  */
    unsigned long target = start + delay;

    /* temporary variable  */
    unsigned long count;

    if (target >= start)
    {
        /* Normal case:  Wait until tick count is greater than target or has
         * wrapped around to less than start.  */
        while (((count = clkcount()) < target) && (count >= start))
            ;
    }
    else
    {
        /* Wrap around case:  Wait for tick count to wrap around, then wait for
         * tick count to reach target.  */
        while ((count = clkcount()) > start)
            ;
        while ((count = clkcount()) < target)
            ;
    }
}

#endif /* RTCLOCK */
