/**
* @file ns16550_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <stdint.h>
#include <xinu.h>
#include <uart.h>
#include <conf.h>
#include "ns16550.h"

int(*rpi_mailbox) (uint32_t channel, uint32_t msg) = 0;

xinu_devcall ns16550_Install (unsigned int DevTabNum, const char* devname, unsigned int uartnum, uintptr_t addr, int(*mailbox) (uint32_t channel, uint32_t msg))
{
	devtab[DevTabNum].num = DevTabNum;
	devtab[DevTabNum].minor = uartnum;
	devtab[DevTabNum].name = (char*)devname;
	devtab[DevTabNum].init = uartInit;
	devtab[DevTabNum].open = 0;
	devtab[DevTabNum].close = 0;
	devtab[DevTabNum].read = uartRead;
	devtab[DevTabNum].write = uartWrite;
	devtab[DevTabNum].seek = 0;
	devtab[DevTabNum].getc = uartGetc;
	devtab[DevTabNum].putc = uartPutc;
	devtab[DevTabNum].control = uartControl;
	devtab[DevTabNum].csr = (void*)addr;
	devtab[DevTabNum].intr = ns16550_uartInterrupt;
	devtab[DevTabNum].irq = 29;

	/* Set the actual hardware function pointers */
	/* Set the actual hardware function pointers */
	struct uart *uartptr;
	uartptr = &uarttab[uartnum];
	uartptr->csr = (void*)addr;
	uartptr->dev = &devtab[DevTabNum];

	uarttab[uartnum].uartKickTx = ns16550_uartKickTx;
	uarttab[uartnum].uartHwStat = ns16550_uartHwStat;

	rpi_mailbox = mailbox;
	uarttab[uartnum].SetCommStateFn = ns16550_SetCommState;	// Set SetCommState pointer to our function

	LPDCB dcbptr = &uartptr->dcb;
	dcbptr->DCBlength = sizeof(DCB);				// Size of the structure
	dcbptr->BaudRate = 115200;						// Default baud rate to 115200
	dcbptr->fBinary = 1;							// Default binary mode on
	dcbptr->fParity = 0;							// Default no parity
	dcbptr->datalength = 0b11;						// Default 8 data bits
	dcbptr->stopbits = 0b00;						// Default 1 stop bit
	dcbptr->supported_datalen = 0b1100;				// Only 7 and 8 bits are supported on this device
	dcbptr->supported_stops = 0b101;				// Only 1 and 2 stop bits are supported on this device
	dcbptr->supported_partiy = 0;					// Parity isnt supported on this device
	dcbptr->supported_CTS = 0;						// No CTS line exists
	dcbptr->supported_DSR = 0;						// No DSR line exists
	dcbptr->supported_RTS = 0;						// No RTS line exists
	dcbptr->supported_DTR = 0;						// No DTR line exists
	dcbptr->XoffChar = 0x13;						// Default XOFF character is 0x13
	dcbptr->XonChar = 0x11;							// Default XON character is 0x11
	dcbptr->ErrorChar = 0xFF;						// Default parity error replacement character is 0xFF
	dcbptr->EofChar = 0x1A;							// Default EOF char is 0x1A (ctrl + z )

	return DevTabNum;
}