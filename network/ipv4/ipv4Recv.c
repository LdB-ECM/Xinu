/* 
 * file ipv4Recv.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <ipv4.h>
#include <network.h>
#include <raw.h>
#include <route.h>
#include <udp.h>
#include <tcp.h>
#include <icmp.h>
#include <netemu.h>

/**
 * @ingroup ipv4
 *
 * Process an incoming IPv4 packet.
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
xinu_syscall ipv4Recv(struct packet *pkt)
{
    struct ipv4Pkt *ip;
    struct netaddr dst;
    struct netaddr src;
    unsigned short iplen;

    /* Error check pointers */
    if (NULL == pkt)
    {
        return SYSERR;
    }

    /* Setup pointer to IPv4 header */
    pkt->nethdr = pkt->curr;
    ip = (struct ipv4Pkt *)pkt->curr;

    /* Verify the IP packet is valid */
    if (FALSE == ipv4RecvValid(ip))
    {
        IPv4_TRACE("Invalid packet");
        netFreebuf(pkt);
        return SYSERR;
    }

    /* Obtain destination and source IP addresses */
    dst.type = NETADDR_IPv4;
    dst.len = IPv4_ADDR_LEN;
	memcpy(dst.addr, ip->dst, dst.len);
    src.type = NETADDR_IPv4;
    src.len = IPv4_ADDR_LEN;
	memcpy(src.addr, ip->src, src.len);

    /* If packet is not destined for one of our network interfaces,
     * then attempt to route the packet */
    if (FALSE == ipv4RecvDemux(&dst))
    {
        IPv4_TRACE("Packet sent to routing subsystem");

#if NETEMU
        /* Run the packet through the network emulator if enabled */
        return netemu(pkt);
#else
        return rtRecv(pkt);
#endif
    }

    /* Check if packet is fragmented */
    if ((IPv4_FLAG_MF & net2hs(ip->flags_froff))
        || (0 != (net2hs(ip->flags_froff) & IPv4_FROFF)))
    {
        // TODO: Handle fragmenting of packet
        // TODO: Should send icmp time exceeded
        // return ipRecvSendFrag(pkt, &dst);
        IPv4_TRACE("Packet fragmented");
        netFreebuf(pkt);
        return SYSERR;
    }

    /* The Ethernet driver pads packets less than 60 bytes in length.
     * If the packet length returned from the Ethernet driver (pkt->len)
     * does not agree with the packet headers, adjust the packet length 
     * to remove padding. */
    iplen = net2hs(ip->len);
    if ((pkt->len - pkt->nif->linkhdrlen) > iplen)
    {
        pkt->len = pkt->nif->linkhdrlen + iplen;
    }

    /* Move current pointer to application level header */
    pkt->curr += ((ip->ver_ihl & IPv4_IHL) << 2);

    IPv4_TRACE("IPv4 proto %d", ip->proto);
    /* Switch on packet protocol */
    switch (ip->proto)
    {
        /* ICMP Packet */
    case IPv4_PROTO_ICMP:
        icmpRecv(pkt);
        break;

#if NUDP
        /* UDP Packet */
    case IPv4_PROTO_UDP:
        udpRecv(pkt, &src, &dst);
        break;
#endif

#if NTCP
        /* TCP Packet */
    case IPv4_PROTO_TCP:
        tcpRecv(pkt, &src, &dst);
        break;
#endif

        /* Unknown IP packet protocol */
    default:
#if NRAW
        rawRecv(pkt, &src, &dst, ip->proto);
#else
        netFreebuf(pkt);
#endif
        break;
    }

    return OK;
}
