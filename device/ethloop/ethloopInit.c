/**
 * @file     ethloopInit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <stdlib.h>
#include <device.h>
#include <ethloop.h>

struct ethloop elooptab[NETHLOOP] = { 0 };

/**
 * @ingroup ethloop
 *
 * Initialize ethloop device
 * @param devptr 
 * @return OK on open
 */
xinu_devcall ethloopInit (device *devptr)
{
    struct ethloop *elpptr;

    elpptr = &elooptab[devptr->minor];
    //bzero(elpptr, sizeof(struct ethloop));  /* cant do that install has set function ptrs */
    elpptr->state = ELOOP_STATE_FREE;

    return OK;
}
