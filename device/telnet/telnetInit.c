/**
 * @file telnetInit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <telnet.h>
#include <stdlib.h>

struct telnet telnettab[NTELNET];

/**
 * @ingroup telnet
 *
 * Initialize TELNET structures.
 * @param devptr TELNET device table entry
 * @return OK if device is initialized
 */
xinu_devcall telnetInit(device *devptr)
{
    struct telnet *tntptr;

    tntptr = &telnettab[devptr->minor];
    bzero(tntptr, sizeof(struct telnet));
    if (0 == devptr->minor)
    {
        tntptr->killswitch = semcreate(0);
    }
    else
    {
        tntptr->killswitch = telnettab[0].killswitch;
    }

    return OK;
}
