/**
 * @file rawInit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <raw.h>
#include <stdlib.h>

struct raw rawtab[NRAW];

/**
 * @ingroup raw
 *
 * Initialize raw socket structures.
 * @param devptr RAW device table entry
 * @return OK if device is initialized
 */
xinu_devcall rawInit(device *devptr)
{
    struct raw *rawptr;

    rawptr = &rawtab[devptr->minor];
    bzero(rawptr, sizeof(struct raw));

    return OK;
}
