/**
* @file kgetc.c
*/

/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <xinu.h>
#include <kernel.h>
#include <conf.h>

xinu_syscall kgetc(void)
{
	if (devtab[SERIAL0].getc)
	{
		return (devtab[SERIAL0].getc(&devtab[SERIAL0]));
	}
	return SYSERR;
}