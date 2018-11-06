/**
 * @file rtSend.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <network.h>
#include <route.h>
#include <ipv4.h>
#include <icmp.h>

/**
 * @ingroup route
 *
 * Attempt to route a packet.
 * @param pkt incoming packet to route
 * @return OK if packet is routed successfully, otherwise SYSERR
 */
xinu_syscall rtSend(struct packet *pkt)
{
    struct ipv4Pkt *ip;
    struct netaddr dst;
    struct rtEntry *route;
    struct netaddr *nxthop;

    /* Error check pointers */
    if (NULL == pkt)
    {
        return SYSERR;
    }

    ip = (struct ipv4Pkt *)pkt->nethdr;
    dst.type = NETADDR_IPv4;
    dst.len = IPv4_ADDR_LEN;
	memcpy(dst.addr, ip->dst, dst.len);

    route = rtLookup(&dst);

    if ((SYSERR == (unsigned long)route) || (NULL == (unsigned long)route))
    {
        RT_TRACE("Routed packet: Network unreachable.");
        icmpDestUnreach(pkt, ICMP_NET_UNR);
        return SYSERR;
    }

    if (route->nif == pkt->nif)
    {
        // If outgoing interface is same as incoming, send ICMP redirect
        if (NULL == route->gateway.type)
        {
            icmpRedirect(pkt, ICMP_RHST, route);
        }
        else
        {
            icmpRedirect(pkt, ICMP_RNET, route);
        }
    }

    /* Update IP header */
    ip->ttl--;
    if (0 == ip->ttl)
    {
        // 11 - Time Exceeded
        icmpTimeExceeded(pkt, ICMP_TTL_EXC);
        return SYSERR;
    }
    ip->chksum = 0;
    ip->chksum = netChksum((unsigned char *)ip, IPv4_HDR_LEN);

    /* Change packet to new network interface */
    pkt->nif = route->nif;

    /* Determine if packet should be send to destination or gateway */
    if (NULL == route->gateway.type)
    {
        nxthop = &dst;
    }
    else
    {
        nxthop = &route->gateway;
    }

    if (SYSERR == ipv4SendFrag(pkt, nxthop))
    {
        RT_TRACE("Routed packet: Host unreachable.");
        icmpDestUnreach(pkt, ICMP_HST_UNR);
        return SYSERR;
    }

    RT_TRACE("Routed packet was sent.");

    return OK;
}
