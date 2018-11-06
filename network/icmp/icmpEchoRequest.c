/**
 * @file icmpEchoRequest.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <icmp.h>
#include <stdlib.h>
#include <clock.h>
#include <CriticalSection.h>

/**
 * @ingroup icmp
 *
 * Send an ICMP Echo (Ping) Request.
 * @param dst destination address
 * @param id  ping stream identifier
 * @param seq sequence number
 * @return OK if packet was sent, otherwise SYSERR
 */
xinu_syscall icmpEchoRequest(struct netaddr *dst, unsigned short id, unsigned short seq)
{
    struct packet *pkt;
    struct icmpEcho *echo;
    int result;
    struct netaddr src;

    ICMP_TRACE("echo request(%d, %d)", id, seq);
    pkt = netGetbuf();
    if (SYSERR == (int)pkt)
    {
        ICMP_TRACE("Failed to acquire packet buffer");
        return SYSERR;
    }

    pkt->len = sizeof(struct icmpEcho);
    pkt->curr -= pkt->len;

    echo = (struct icmpEcho *)pkt->curr;
    echo->id = hs2net(id);
    echo->seq = hs2net(seq);
    /* Our optional data payload includes room for the departure */
    /*  and arrival timestamps, in seconds, milliseconds, and    */
    /*  clock cycles.                                            */
	ENTER_KERNEL_CRITICAL_SECTION();
    echo->timecyc = hl2net(clkcount());
    echo->timetic = hl2net(clkticks);
    echo->timesec = hl2net(clktime);
	EXIT_KERNEL_CRITICAL_SECTION();
    echo->arrivcyc = 0;
    echo->arrivtic = 0;
    echo->arrivsec = 0;

    ICMP_TRACE("Sending Echo Request id = %d, seq = %d, time = %lu.%lu",
               net2hs(echo->id), net2hs(echo->seq),
               net2hl(echo->timesec), net2hl(echo->timetic));

    src.type = 0;

    result = icmpSend(pkt, ICMP_ECHO, 0, sizeof(struct icmpEcho), &src, dst);
    netFreebuf(pkt);
    return result;
}
