/**
 * @file dhcpc.h
 *
 * DHCP client interface
 */
/* Embedded Xinu, Copyright (C) 2013, 2018.  All rights reserved. */

#ifndef _DHCPC_H_
#define _DHCPC_H_

#include <xinu.h>
#include <network.h>

/** DHCP transfer data */
struct dhcpData
{
    /* DHCP client output */
    struct netaddr ip;
    struct netaddr gateway;
    struct netaddr mask;
    char bootfile[128];
    struct netaddr next_server;

    /* DHCP client internal variables */
    int state;
    unsigned int cxid;
    unsigned int starttime;
    int recvStatus;
    unsigned short discoverSecs;
    unsigned int offeredIpv4Addr;
    unsigned int clientIpv4Addr;
    unsigned int serverIpv4Addr;
    unsigned char clientHwAddr[ETH_ADDR_LEN];
    unsigned char serverHwAddr[ETH_ADDR_LEN];
};

xinu_syscall dhcpClient(int descrp, unsigned int timeout, struct dhcpData *data);

#endif /* _DHCPC_H_ */
