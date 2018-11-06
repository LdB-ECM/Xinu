/**
 * @file     netRecv.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <arp.h>
#include <device.h>
#include <ethernet.h>
#include <network.h>
#include <ipv4.h>
#include <snoop.h>
#include <stdlib.h>
#include <string.h>
#include <thread.h>

/**
 * @ingroup network
 *
 * Receive thread to handle one incoming packet at a time
 *
 * @param netptr
 *      network interface device to open netRecv on
 *
 * @return
 *      This thread never returns.
 */
thread netRecv(struct netif *netptr)
{
    unsigned int maxlen;                            /**< maximum packet length */
    maxlen = netptr->linkhdrlen + netptr->mtu;
    struct packet *pkt;
    struct etherPkt *ether;
    struct netaddr dst;

    /* Processing incoming packets */
    while (TRUE)
    {
        int len;

        /* Get a buffer for incoming packet */
        pkt = netGetbuf();
        if (SYSERR == (int)pkt)
        {
            continue;
        }

        /* Read in packet from the underlying network device.
         * This thread will wait until there is a packet to read.
         * It is the responsibility of the network driver to tell this
         * thread to run, signifying that there is a packet to read
         */
        len = read(netptr->dev, pkt->data, maxlen);
        if (ETH_HDR_LEN > len || SYSERR == len)
        {
            netFreebuf(pkt);
            continue;
        }

        pkt->len = len;
        pkt->curr = pkt->data;
        pkt->nif = netptr;
        netptr->nin++;

        /* Point to packet location in the incoming packet buffer */
        pkt->linkhdr = pkt->curr;
        ether = (struct etherPkt *)pkt->curr;

        /* Snoop if we are in promiscuous mode */
        if (netptr->capture != NULL)
        {
            snoopCapture(netptr->capture, pkt);
        }

        /* Obtain destination hardware address */
        dst.type = NETADDR_ETHERNET;
        dst.len = ETH_ADDR_LEN;
		memcpy(dst.addr, ether->dst, ETH_ADDR_LEN);

#ifdef TRACE_NET
        char str[20];
        NET_TRACE("Read packet len %d", pkt->len);
        netaddrsprintf(str, &dst);
        NET_TRACE("\tPacket dst %s", str);
        NET_TRACE("\tPacket proto 0x%04X", net2hs(ether->type));
#endif

        /* Verify that packet belongs to our mac or is broadcast mac */
        if ((netaddrequal(&dst, &netptr->hwaddr))
            || (netaddrequal(&dst, &netptr->hwbrc)))
        {
            /* Move current pointer to network level header */
            pkt->curr = pkt->data + netptr->linkhdrlen;

            /* Call necessary routine based on packet type */
            switch (net2hs(ether->type))
            {
                /* IP Packet */
            case ETHER_TYPE_IPv4:
                ipv4Recv(pkt);
                netptr->nproc++;
                break;

                /* ARP Packet */
            case ETHER_TYPE_ARP:
                arpRecv(pkt);
                netptr->nproc++;
                break;

                /* Unknown ether packet type */
            default:
                netFreebuf(pkt);
                break;
            }

        }
        else
        {
            netFreebuf(pkt);
        }
    }

    return SYSERR;

}
