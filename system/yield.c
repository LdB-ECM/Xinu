/**
 * @file yield.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>
#include <thread.h>

/**
 * @ingroup threads
 *
 * Yield processor.
 * @return OK when the thread is context switched back
 */
xinu_syscall yield (void)
{
    resched();
    return OK;
}
