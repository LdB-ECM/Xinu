/**
* @file udp_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <xinu.h>
#include <conf.h>
#include <udp.h>

xinu_devcall udp_Install (unsigned int DevTabNum, const char* devname, unsigned int udpNum)
{
	devtab[DevTabNum].num = DevTabNum;
	devtab[DevTabNum].minor = udpNum;
	devtab[DevTabNum].name = (char*)devname;
	devtab[DevTabNum].init = udpInit;
	devtab[DevTabNum].open = udpOpen;
	devtab[DevTabNum].close = udpClose;
	devtab[DevTabNum].read = udpRead;
	devtab[DevTabNum].write = udpWrite;
	devtab[DevTabNum].seek = 0;
	devtab[DevTabNum].getc = 0;
	devtab[DevTabNum].putc = 0;
	devtab[DevTabNum].control = udpControl;
	devtab[DevTabNum].csr = 0;
	devtab[DevTabNum].intr = 0;
	devtab[DevTabNum].irq = 0;

	udptab[udpNum].dev = &devtab[DevTabNum];
	return DevTabNum;
}