/**
 * @file     loopbackInit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <loopback.h>
#include <stdlib.h>

struct loopback looptab[NLOOPBACK] = { 0 };

/**
 * @ingroup loopback
 *
 * Initialize loopback device
 * @param devptr 
 * @return OK on open
 */
xinu_devcall loopbackInit(device *devptr)
{
    struct loopback *lbkptr;

    lbkptr = &looptab[devptr->minor];
    lbkptr->state = LOOP_STATE_FREE;
    lbkptr->index = 0;

    return OK;
}
