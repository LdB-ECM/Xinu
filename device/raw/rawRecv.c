/**
 * @file     rawRecv.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <network.h>
#include <icmp.h>
#include <raw.h>

/**
 * @ingroup raw
 *
 * Process an incoming protocol other than UDP or TCP.
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
xinu_syscall rawRecv(struct packet *pkt, struct netaddr *src,
                     struct netaddr *dst, unsigned short proto)
{
    struct raw *rawptr;
    unsigned int index;

    /* Error check pointers */
    if ((NULL == pkt) || (NULL == src) || (NULL == dst))
    {
        return SYSERR;
    }

    /* Locate raw socket for the packet */
    rawptr = rawDemux(src, dst, proto);
    if (NULL == rawptr)
    {
        RAW_TRACE("No matching socket");
        /* Send ICMP port unreachable message */
        icmpDestUnreach(pkt, ICMP_PORT_UNR);
        netFreebuf(pkt);
        return OK;
    }

    /* Ensure there is space */
    if (rawptr->icount >= RAW_IBLEN)
    {
        netFreebuf(pkt);
        return SYSERR;
    }

    /* Enqueue packet */
    index = (rawptr->istart + rawptr->icount) % RAW_IBLEN;
    rawptr->in[index] = pkt;
    netaddrcpy(&rawptr->src[index], src);
    rawptr->icount++;
    signal(rawptr->isema);

    RAW_TRACE("Enqueued packet");
    return OK;
}
