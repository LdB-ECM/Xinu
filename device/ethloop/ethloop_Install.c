/**
* @file ethloop_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <xinu.h>
#include <conf.h>
#include <ethloop.h>

xinu_devcall ethloop_Install (unsigned int DevTabNum, const char* devname, unsigned int eloopNum)
{
	devtab[DevTabNum].num = DevTabNum;
	devtab[DevTabNum].minor = eloopNum;
	devtab[DevTabNum].name = (char*)devname;
	devtab[DevTabNum].init = ethloopInit;
	devtab[DevTabNum].open = ethloopOpen;
	devtab[DevTabNum].close = ethloopClose;
	devtab[DevTabNum].read = ethloopRead;
	devtab[DevTabNum].write = ethloopWrite;
	devtab[DevTabNum].seek = 0;
	devtab[DevTabNum].getc = 0;
	devtab[DevTabNum].putc = 0;
	devtab[DevTabNum].control = ethloopControl;
	devtab[DevTabNum].csr = 0;
	devtab[DevTabNum].intr = 0;
	devtab[DevTabNum].irq = 0;

	elooptab[eloopNum].dev = &devtab[DevTabNum];
	return DevTabNum;
}