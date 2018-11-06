/**
 * @file mdelay.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#if RTCLOCK == TRUE
#include <clock.h>

/**
 * @ingroup timer
 *
 * Busy-waits for a number of milliseconds.
 * Only use this function if absolutely necessary.  Normally you should call
 * sleep() so that other threads can use the processor immediately.
 *
 * @param ms
 *    Number of milliseconds to wait for.
 */
void mdelay (unsigned long ms)
{
    while (ms--)
    {
        udelay(1000);
    }
}

#endif /* RTCLOCK */
