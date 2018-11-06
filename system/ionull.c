/**
 * @file ionull.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup devcalls
 *
 * Do nothing (use for irrelevant entries in devtab).
 *
 * @return ::OK
 */
xinu_devcall ionull(void)
{
    return OK;
}
