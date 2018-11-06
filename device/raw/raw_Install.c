/**
* @file null_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <xinu.h>
#include <conf.h>
#include <raw.h>

xinu_devcall raw_Install (unsigned int DevTabNum, const char* devname, unsigned int rawNum)
{
	devtab[DevTabNum].num = DevTabNum;
	devtab[DevTabNum].minor = rawNum;
	devtab[DevTabNum].name = (char*)devname;
	devtab[DevTabNum].init = rawInit;
	devtab[DevTabNum].open = rawOpen;
	devtab[DevTabNum].close = rawClose;
	devtab[DevTabNum].read = rawRead;
	devtab[DevTabNum].write = rawWrite;
	devtab[DevTabNum].seek = 0;
	devtab[DevTabNum].getc = 0;
	devtab[DevTabNum].putc = 0;
	devtab[DevTabNum].control = rawControl;
	devtab[DevTabNum].csr = 0;
	devtab[DevTabNum].intr = 0;
	devtab[DevTabNum].irq = 0;

	rawtab[rawNum].dev = &devtab[DevTabNum];
	return DevTabNum;
}