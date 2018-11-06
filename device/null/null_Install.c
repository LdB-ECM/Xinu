/**
* @file null_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <xinu.h>
#include <conf.h>

xinu_devcall null_Install (unsigned int DevTabNum, const char* devname)
{
	devtab[DevTabNum].num = DevTabNum;
	devtab[DevTabNum].minor = 0;
	devtab[DevTabNum].name = (char*)devname;
	devtab[DevTabNum].init = 0;
	devtab[DevTabNum].open = 0;
	devtab[DevTabNum].close = 0;
	devtab[DevTabNum].read = 0;
	devtab[DevTabNum].write = 0;
	devtab[DevTabNum].seek = 0;
	devtab[DevTabNum].getc = 0;
	devtab[DevTabNum].putc = 0;
	devtab[DevTabNum].control = 0;
	devtab[DevTabNum].csr = 0;
	devtab[DevTabNum].intr = 0;
	devtab[DevTabNum].irq = 0;

	return DevTabNum;
}