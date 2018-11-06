/**
 * @file moncount.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <monitor.h>

/**
 * @ingroup monitors
 *
 * Retrieve the count of a monitor --- that is, the number of times it has been
 * locked by the owning thread, or 0 if no thread currently owns the monitor.
 *
 * This function performs no locking.  The caller must temporarily disable
 * interrupts if the specified monitor could potentially be freed, locked, or
 * unlocked concurrently.
 *
 * @param mon
 *      The monitor to retrieve the count of.
 *
 * @return
 *      If @p mon specified a valid, allocated monitor, its count is returned.
 *      Otherwise, ::SYSERR is returned.
 */
xinu_syscall moncount (monitor mon)
{
    if (isbadmon(mon))
    {
        return SYSERR;
    }

    return (montab[mon].count);
}
