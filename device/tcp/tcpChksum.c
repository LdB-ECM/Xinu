/**
 * @file tcpChksum.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <ipv4.h>
#include <network.h>
#include <tcp.h>

/**
 * @ingroup tcp
 */
unsigned short tcpChksum(struct packet *pkt, unsigned short len, struct netaddr *src,
                 struct netaddr *dst)
{
    struct tcpPseudo *pseu;
    unsigned char buf[TCP_PSEUDO_LEN];
    unsigned short sum;

    /* Store current data before TCP header in temporary buffer */
    pseu = (struct tcpPseudo *)(pkt->curr - TCP_PSEUDO_LEN);
	memcpy(buf, pseu, TCP_PSEUDO_LEN);

    /* Generate TCP psuedo header */
	memcpy(pseu->srcIp, src->addr, IPv4_ADDR_LEN);
	memcpy(pseu->dstIp, dst->addr, IPv4_ADDR_LEN);
    pseu->zero = 0;
    pseu->proto = IPv4_PROTO_TCP;
    pseu->len = hs2net(len);

    sum = netChksum(pseu, len + TCP_PSEUDO_LEN);

    /* Restore data before TCP header from temporary buffer */
	memcpy(pseu, buf, TCP_PSEUDO_LEN);

    return sum;
}
