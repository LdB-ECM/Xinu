/**
 * @file udpInit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <stdlib.h>
#include <udp.h>

struct udp udptab[NUDP];

/**
 * @ingroup udpexternal
 *
 * Set aside some space for a UDP device to be opened on
 * @param devptr UDP device table entry
 * @return OK
 */
xinu_devcall udpInit(device *devptr)
{
    struct udp *udpptr;

    udpptr = &udptab[devptr->minor];
    bzero(udpptr, sizeof(struct udp));

    return OK;
}
