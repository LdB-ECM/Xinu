/**
* @file kputc.c
*/

/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <xinu.h>
#include <kernel.h>
#include <conf.h>

xinu_syscall kputc (char ch)
{
	if (devtab[SERIAL0].putc) 
	{
		return (devtab[SERIAL0].putc(&devtab[SERIAL0], ch));
	}
	return SYSERR;
}