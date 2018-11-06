/**
* @file null_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <xinu.h>
#include <conf.h>
#include <tty.h>

xinu_devcall tty_Install (unsigned int DevTabNum, const char* devname, unsigned int ttyNum)
{
	devtab[DevTabNum].num = DevTabNum;
	devtab[DevTabNum].minor = ttyNum;
	devtab[DevTabNum].name = (char*)devname;
	devtab[DevTabNum].init = ttyInit;
	devtab[DevTabNum].open = ttyOpen;
	devtab[DevTabNum].close = ttyClose;
	devtab[DevTabNum].read = ttyRead;
	devtab[DevTabNum].write = ttyWrite;
	devtab[DevTabNum].seek = 0;
	devtab[DevTabNum].getc = ttyGetc;
	devtab[DevTabNum].putc = ttyPutc;
	devtab[DevTabNum].control = ttyControl;
	devtab[DevTabNum].csr = 0;
	devtab[DevTabNum].intr = 0;
	devtab[DevTabNum].irq = 0;

	ttytab[ttyNum].phw = &devtab[DevTabNum];

	return DevTabNum;
}