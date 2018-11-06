/**
 * @file vlan.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#ifndef _VLAN_H_
#define _VLAN_H_

#include <xinu.h>

/*
 * ETHERNET HEADER w/ vlan
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Destination MAC (cont.)                                       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Destination MAC (cont.)       | Source MAC (cont.)            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Source MAC (cont.)                                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Tag Protocol Identifier       | PCP |C| VLAN Identifier       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Ether Type                    | Data                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Data (46 - 1500 octets)                                       |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

#define ETH_LOCAL_MAC   0x02    /**< Locally administered MAC bit       */

#define ETH_TYPE_VLAN   0x8100  /**< VLAN EtherType                     */
#define ETH_VLAN_IDMASK 0x0FFF  /**< VLAN identification mask           */

/**
 * EthernetII frame header with IEEE 802.1Q vlan tag
 */
struct vlanPkt
{
    unsigned char dst[ETH_ADDR_LEN];    /**< Destination Mac                    */
    unsigned char src[ETH_ADDR_LEN];    /**< Source Mac                         */
    unsigned short tpi;                 /**< Tag Protocol Identifier            */
    unsigned short vlanId;              /**< PCP&CFI (4bits), ID (12bits)       */
    unsigned short type;                /**< Ether type                         */
    unsigned char data[1];              /**< Packet data                        */
};

xinu_devcall vlanInit(device *);
xinu_devcall vlanOpen(device *);
xinu_devcall vlanClose(device *);

#endif                          /* _VLAN_H_ */
