/**
* @file telnet_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <xinu.h>
#include <conf.h>
#include <telnet.h>

xinu_devcall telnet_Install (unsigned int DevTabNum, const char* devname, unsigned int telnetNum)
{
	devtab[DevTabNum].num = DevTabNum;
	devtab[DevTabNum].minor = telnetNum;
	devtab[DevTabNum].name = (char*)devname;
	devtab[DevTabNum].init = telnetInit;
	devtab[DevTabNum].open = telnetOpen;
	devtab[DevTabNum].close = telnetClose;
	devtab[DevTabNum].read = telnetRead;
	devtab[DevTabNum].write = telnetWrite;
	devtab[DevTabNum].seek = 0;
	devtab[DevTabNum].getc = telnetGetc;
	devtab[DevTabNum].putc = telnetPutc;
	devtab[DevTabNum].control = telnetControl;
	devtab[DevTabNum].csr = 0;
	devtab[DevTabNum].intr = 0;
	devtab[DevTabNum].irq = 0;

	return DevTabNum;
}