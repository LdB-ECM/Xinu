/**
 * file netGetbuf.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <bufpool.h>
#include <network.h>

/**
 * @ingroup network
 *
 * Frees a buffer for storing a packet.
 * @return OK if successful, SYSERR if an error occured
 */
xinu_syscall netFreebuf(struct packet *pkt)
{
    return buffree(pkt);
}
