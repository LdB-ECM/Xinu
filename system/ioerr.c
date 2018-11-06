/**
 * @file ioerr.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup devcalls
 *
 * Unconditionally return an error (used for "error" entries in devtab).
 *
 * @return ::SYSERR
 */
xinu_devcall ioerr (void)
{
    return SYSERR;
}
