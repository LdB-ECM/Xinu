/**
 * @file icmpRedirect.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <ipv4.h>
#include <icmp.h>
#include <string.h>
#include <route.h>

/**
 * @ingroup icmp
 *
 * Compose ICMP Redirect message.
 * @param redir packet that should have gone somewhere else
 * @param code  ICMP redirect code number
 * @param route new gateway for packet
 * @return OK if packet was sent, otherwise SYSERR
 */
xinu_syscall icmpRedirect(struct packet *redir, unsigned char code,
                     struct rtEntry *route)
{
    struct packet *pkt;
    struct ipv4Pkt *ip;
    struct netaddr dst;
    int result;
    int ihl;
    struct netaddr src;

    ICMP_TRACE("ICMP redirect, code(%d)", code);
    pkt = netGetbuf();
    if (SYSERR == (int)pkt)
    {
        ICMP_TRACE("Failed to acquire packet buffer");
        return SYSERR;
    }

    ip = (struct ipv4Pkt *)redir->nethdr;
    dst.type = NETADDR_IPv4;
    dst.len = IPv4_ADDR_LEN;
    /* Send error message back to original source.                */
	memcpy(dst.addr, ip->src, dst.len);
    ihl = (ip->ver_ihl & IPv4_IHL) * 4;

    /* Message will contain at least ICMP_DEF_DATALEN             */
    /*  of packet in question, as per RFC 792.                    */
    pkt->len = ihl + ICMP_DEF_DATALEN;
    pkt->curr -= pkt->len;

	memcpy(pkt->curr, ip, ihl + ICMP_DEF_DATALEN);
    /* First four octets of payload are gateway.                  */
    pkt->curr -= IPv4_ADDR_LEN;
    pkt->len += IPv4_ADDR_LEN;

	memcpy(pkt->curr, route->gateway.addr, IPv4_ADDR_LEN);

    src.type = 0;
    result = icmpSend(pkt, ICMP_REDIRECT, code, pkt->len, &src, &dst);

    netFreebuf(pkt);
    return result;
}
