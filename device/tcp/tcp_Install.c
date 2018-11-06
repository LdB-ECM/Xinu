/**
* @file null_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <xinu.h>
#include <conf.h>
#include <tcp.h>

xinu_devcall tcp_Install (unsigned int DevTabNum, const char* devname, unsigned int tcpNum)
{
	devtab[DevTabNum].num = DevTabNum;
	devtab[DevTabNum].minor = tcpNum;
	devtab[DevTabNum].name = (char*)devname;
	devtab[DevTabNum].init = tcpInit;
	devtab[DevTabNum].open = tcpOpen;
	devtab[DevTabNum].close = tcpClose;
	devtab[DevTabNum].read = tcpRead;
	devtab[DevTabNum].write = tcpWrite;
	devtab[DevTabNum].seek = 0;
	devtab[DevTabNum].getc = tcpGetc;
	devtab[DevTabNum].putc = tcpPutc;
	devtab[DevTabNum].control = tcpControl;
	devtab[DevTabNum].csr = 0;
	devtab[DevTabNum].intr = 0;
	devtab[DevTabNum].irq = 0;

	return DevTabNum;
}