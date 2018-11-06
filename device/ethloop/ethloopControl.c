/**
 * @file ethloopControl.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <bufpool.h>
#include <device.h>
#include <ethernet.h>
#include <ethloop.h>
#include <CriticalSection.h>
#include <network.h>
#include <stdlib.h>

#include <stdio.h>

/**
 * @ingroup ethloop
 *
 * Control function for ethloop devices.
 * @param devptr ethloop device table entry
 * @param func control function to execute
 * @param arg1 first argument for the control function
 * @param arg2 second argument for the control function
 * @return the result of the control function
 */
xinu_devcall ethloopControl(device *devptr, int func, long arg1, long arg2)
{
    struct ethloop *elpptr;
    struct netaddr* addr;
    unsigned char old;
    char *buf;
    char *hold;
    int holdlen;

    elpptr = &elooptab[devptr->minor];

	ENTER_KERNEL_CRITICAL_SECTION();
    if (ELOOP_STATE_ALLOC != elpptr->state)
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    switch (func)
    {
/* Get link header length. */
    case NET_GET_LINKHDRLEN:
		EXIT_KERNEL_CRITICAL_SECTION();
        return ETH_HDR_LEN;

/* Get MAC address from card. */
    case NET_GET_HWADDR:
	{
		volatile uint8_t* p;
		EXIT_KERNEL_CRITICAL_SECTION();
		addr = (struct netaddr *)arg1;
		addr->type = NETADDR_ETHERNET;
		addr->len = ETH_ADDR_LEN;
		// On ARM8 64 bit it think it can write them as 8 bytes but it is mis-aligned .. GCC bug
		// The volatile is used to stop it joining them as an 8 byte write
		p = (volatile uint8_t*)&addr->addr;
		p[0] = 0xAA;
		p[1] = 0xBB;
		p[2] = 0xCC;
		p[3] = 0xDD;
		p[4] = 0xEE;
		p[5] = 0xFF;
		break;
	}
/* Get broadcast MAC address. */
    case NET_GET_HWBRC:
	{
		volatile uint8_t* p;
		EXIT_KERNEL_CRITICAL_SECTION();
		addr = (struct netaddr *)arg1;
		addr->type = NETADDR_ETHERNET;
		addr->len = ETH_ADDR_LEN;
		// On ARM8 64 bit it think it can write them as 8 bytes but it is mis-aligned .. GCC bug
		// The volatile is used to stop it joining them as an 8 byte write
		p = (volatile uint8_t*)&addr->addr;
		p[0] = 0xFF;
		p[1] = 0xFF;
		p[2] = 0xFF;
		p[3] = 0xFF;
		p[4] = 0xFF;
		p[5] = 0xFF;
		break;
	}

/* Get MTU. */
    case NET_GET_MTU:
		EXIT_KERNEL_CRITICAL_SECTION();
        return ELOOP_MTU;

/* Get next packet off hold queue */
    case ELOOP_CTRL_GETHOLD:
        buf = (char *)arg1;
        /* Wait for held packet */
        wait(elpptr->hsem);
        /* Get and clear held packet */
        hold = elpptr->hold;
        holdlen = elpptr->holdlen;
        elpptr->hold = NULL;
        elpptr->holdlen = 0;
		EXIT_KERNEL_CRITICAL_SECTION();
        /* Copy held packet to buffer */
        if (arg2 < holdlen)
        {
            holdlen = arg2;
        }
		memcpy(buf, hold, holdlen);
        /* Free hold buffer */
        buffree(hold);
        return holdlen;

/* Set flags */
    case ELOOP_CTRL_SETFLAG:
        old = elpptr->flags & arg1;
        elpptr->flags |= (arg1);
		EXIT_KERNEL_CRITICAL_SECTION();
        return old;

/* Clear flags */
    case ELOOP_CTRL_CLRFLAG:
        old = elpptr->flags & arg1;
        elpptr->flags &= ~(arg1);
		EXIT_KERNEL_CRITICAL_SECTION();
        return old;

    default:
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }
	EXIT_KERNEL_CRITICAL_SECTION();
    return OK;
}
