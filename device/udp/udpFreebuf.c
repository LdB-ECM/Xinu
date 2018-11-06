/**
 * @file     udpFreebuf.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <bufpool.h>
#include <network.h>
#include <udp.h>

/**
 * @ingroup udpinternal
 */
xinu_syscall udpFreebuf(struct udpPkt *udppkt)
{
    return buffree(udppkt);
}
