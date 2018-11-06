/**
* @file usbKbd_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <stdint.h>
#include <xinu.h>
#include <usb_core_driver.h>
#include <usbkbd.h>

xinu_devcall usbKbd_Install (unsigned int DevTabNum, const char* devname, unsigned int kbdnum)
{
	devtab[DevTabNum].num = DevTabNum;
	devtab[DevTabNum].minor = kbdnum;
	devtab[DevTabNum].name = (char*)devname;
	devtab[DevTabNum].init = usbKbdInit;
	devtab[DevTabNum].open = 0;
	devtab[DevTabNum].close = 0;
	devtab[DevTabNum].read = usbKbdRead;
	devtab[DevTabNum].write = 0;
	devtab[DevTabNum].seek = 0;
	devtab[DevTabNum].getc = usbKbdGetc;
	devtab[DevTabNum].putc = 0;
	devtab[DevTabNum].control = usbKbdControl;
	devtab[DevTabNum].csr = 0;
	devtab[DevTabNum].intr = 0;
	devtab[DevTabNum].irq = 0;

	return DevTabNum;
}

