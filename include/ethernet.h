/*
 * @file ethernet.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#ifndef _ETHERNET_H_
#define _ETHERNET_H_

#include <stdint.h>
#include <xinu.h>

/* Ether Packet Types */
#define ETHER_TYPE_IPv4   0x0800
#define ETHER_TYPE_ARP    0x0806

#define ETH_ADDR_LEN	6
#define ETH_HDR_LEN		14

/*
 * ETHERNET HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Destination MAC                                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Destination MAC               | Source MAC                    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Source MAC                                                    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Ether Type                    | Data                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Data (46 - 1500 octets)                                       |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

/* Ethernet packet header */
struct etherPkt
{
    uint8_t dst[ETH_ADDR_LEN];  /* Destination Mac */
    uint8_t src[ETH_ADDR_LEN];  /* Source Mac      */
    uint16_t type;              /* Ether type      */
    uint8_t data[1];            /* Packet data     */
};

#endif                          /* _ETHERNET_H_ */
