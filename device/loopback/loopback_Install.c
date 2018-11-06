/**
* @file loopback_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <xinu.h>
#include <conf.h>
#include <loopback.h>

xinu_devcall loopback_Install (unsigned int DevTabNum, const char* devname, unsigned int loopNum)
{
	devtab[DevTabNum].num = DevTabNum;
	devtab[DevTabNum].minor = loopNum;
	devtab[DevTabNum].name = (char*)devname;
	devtab[DevTabNum].init = loopbackInit;
	devtab[DevTabNum].open = loopbackOpen;
	devtab[DevTabNum].close = loopbackClose;
	devtab[DevTabNum].read = loopbackRead;
	devtab[DevTabNum].write = loopbackWrite;
	devtab[DevTabNum].seek = 0;
	devtab[DevTabNum].getc = loopbackGetc;
	devtab[DevTabNum].putc = loopbackPutc;
	devtab[DevTabNum].control = loopbackControl;
	devtab[DevTabNum].csr = 0;
	devtab[DevTabNum].intr = 0;
	devtab[DevTabNum].irq = 0;

	looptab[loopNum] = (struct loopback ){ 0 };
	looptab[loopNum].state = LOOP_STATE_FREE;
	looptab[loopNum].index = 0;
	looptab[loopNum].flags = 0;

	return DevTabNum;
}