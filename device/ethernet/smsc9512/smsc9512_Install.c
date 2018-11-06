/**
* @file smsc9512_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <xinu.h>
#include <conf.h>
#include <ether.h>
#include "smsc9512.h"

xinu_devcall smsc9512_Install (unsigned int DevTabNum, const char* devname, unsigned int ethNum)
{
	devtab[DevTabNum].num = DevTabNum;
	devtab[DevTabNum].minor = ethNum;
	devtab[DevTabNum].name = (char*)devname;
	devtab[DevTabNum].init = etherInit;
	devtab[DevTabNum].open = smsc9512Open;
	devtab[DevTabNum].close = etherClose;
	devtab[DevTabNum].read = etherRead;
	devtab[DevTabNum].write = smsc9512Write;
	devtab[DevTabNum].seek = 0;
	devtab[DevTabNum].getc = 0;
	devtab[DevTabNum].putc = 0;
	devtab[DevTabNum].control = etherControl;
	devtab[DevTabNum].csr = 0;
	devtab[DevTabNum].intr = 0;
	devtab[DevTabNum].irq = 0;

	ethertab[ethNum].dev = &devtab[DevTabNum];
	/* Set the driver header */
	ethertab[ethNum].specific_driver_header = &smsc9512_driver;

	/* Set the function pointers on install */
	ethertab[ethNum].set_mac_address = smsc9512_set_mac_address;
	ethertab[ethNum].get_mac_address = smsc9512_get_mac_address;
	ethertab[ethNum].set_loopback_mode = smsc9512_set_loopback_mode;
	return DevTabNum;
}