/**
 * @file etherControl.c
 */
/* Embedded Xinu, Copyright (C) 2008, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <usb_util.h>		// needed for usb_status_t
#include <ether.h>
#include <network.h>
#include <string.h>
#include "ether.h"

/* Implementation of etherControl(), see the documentation for this function in ether.h.  */
xinu_devcall etherControl (device *devptr, int req, long arg1, long arg2)
{
    struct usb_device *udev;
    usb_status_t status;
    struct netaddr *addr;
    struct ether *ethptr;

    ethptr = &ethertab[devptr->minor];
    udev = ethptr->csr;
    if (udev == NULL)
    {
        return SYSERR;
    }

    status = USB_STATUS_SUCCESS;

    switch (req)
    {
    /* Program MAC address into device. */
    case ETH_CTRL_SET_MAC:
		if (ethptr->set_mac_address)
			status = ethptr->set_mac_address(udev, (const unsigned char*)arg1);
        break;

    /* Get MAC address from device. */
    case ETH_CTRL_GET_MAC:
		if (ethptr->get_mac_address)
			status = ethptr->get_mac_address(udev, (unsigned char*)arg1);
        break;

    /* Enable or disable loopback mode.  */
    case ETH_CTRL_SET_LOOPBK:
		if (ethptr->set_loopback_mode)
			ethptr->set_loopback_mode(udev, (unsigned int)arg1);
        break;

    /* Get link header length. */
    case NET_GET_LINKHDRLEN:
        return ETH_HDR_LEN;

    /* Get MTU. */
    case NET_GET_MTU:
        return ETH_MTU;

    /* Get hardware address.  */
    case NET_GET_HWADDR:
        addr = (struct netaddr *)arg1;
        addr->type = NETADDR_ETHERNET;
        addr->len = ETH_ADDR_LEN;
        return etherControl(devptr, ETH_CTRL_GET_MAC, (long)addr->addr, 0);

    /* Get broadcast hardware address. */
    case NET_GET_HWBRC:
        addr = (struct netaddr *)arg1;
        addr->type = NETADDR_ETHERNET;
        addr->len = ETH_ADDR_LEN;
        memset(addr->addr, 0xFF, ETH_ADDR_LEN);
        break;

    default:
        return SYSERR;
    }

    if (status != USB_STATUS_SUCCESS)
    {
        return SYSERR;
    }

    return OK;
}
