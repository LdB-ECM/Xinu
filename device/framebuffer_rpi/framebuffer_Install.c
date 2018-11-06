/**
* @file FrameBuffer_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <stdint.h>
#include <xinu.h>
#include <uart.h>
#include <conf.h>
#include <framebuffer.h>

xinu_devcall framebuffer_Install (unsigned int DevTabNum, const char* devname, unsigned int fbNum, int(*rpi_mailbox) (uint32_t channel, uint32_t msg))
{
	devtab[DevTabNum].num = DevTabNum;
	devtab[DevTabNum].minor = fbNum;
	devtab[DevTabNum].name = (char*) devname;
	devtab[DevTabNum].init = screenInit;
	devtab[DevTabNum].open = 0;
	devtab[DevTabNum].close = 0;
	devtab[DevTabNum].read = 0;
	devtab[DevTabNum].write = fbWrite;
	devtab[DevTabNum].seek = 0;
	devtab[DevTabNum].getc = 0;
	devtab[DevTabNum].putc = fbPutc;
	devtab[DevTabNum].control = 0;
	devtab[DevTabNum].csr = 0;
	devtab[DevTabNum].intr = 0;
	devtab[DevTabNum].irq = 0;

	/* Connect up the pi mailbox handler */
	fbtab[fbNum].rpi_mailbox = rpi_mailbox;


	return DevTabNum;
}