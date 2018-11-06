/**
 * @file ttyInit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <tty.h>
#include <stdlib.h>

struct tty ttytab[NTTY];

/**
 * @ingroup tty
 *
 * Initialize TTY structures.
 * @param devptr TTY device table entry
 * @return OK if device is initialized
 */
xinu_devcall ttyInit(device *devptr)
{
    struct tty *ttyptr;

    ttyptr = &ttytab[devptr->minor];
    bzero(ttyptr, sizeof(struct tty));

    return OK;
}
