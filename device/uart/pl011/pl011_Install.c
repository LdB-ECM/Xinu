/**
* @file pl011_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <stdint.h>
#include <xinu.h>
#include <uart.h>
#include <conf.h>
#include "pl011.h"

xinu_devcall pl011_Install (unsigned int DevTabNum, const char* devname, unsigned int uartnum, uintptr_t addr)
{
	devtab[DevTabNum].num = DevTabNum;				// Driver table number	
	devtab[DevTabNum].minor = uartnum;				// Hold the uart number
	devtab[DevTabNum].name = (char*)devname;		// Hold the device name string
	devtab[DevTabNum].init = uartInit;				// Standard UART initialze function
	devtab[DevTabNum].open = 0;						// No open function
	devtab[DevTabNum].close = 0;					// No close function
	devtab[DevTabNum].read = uartRead;				// Read from the pl011 function
	devtab[DevTabNum].write = uartWrite;			// Write to the pl011 function
	devtab[DevTabNum].seek = 0;						// No seek function
	devtab[DevTabNum].getc = uartGetc;				// Get a chacter from the pl011
	devtab[DevTabNum].putc = uartPutc;				// Put a chaacter out the pl011
	devtab[DevTabNum].control = uartControl;		// Change line and flags
	devtab[DevTabNum].csr = (void*)addr;			// Hold the base address
	devtab[DevTabNum].intr = pl011_uartInterrupt;	// Interrupt function pointer
	devtab[DevTabNum].irq = 57;						// Irq to use

	/* Set the actual data on the uart tab */
	struct uart *uartptr;
	uartptr = &uarttab[uartnum];
	uartptr->csr = (void*)addr;
	uartptr->dev = &devtab[DevTabNum];

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

	uartptr->SetCommStateFn = pl011_SetCommState;	// Set SetCommState pointer to our function
	uartptr->uartKickTx = pl011_uartKickTx;			// Set the Hardware Putc function pointer
	uartptr->uartHwStat = pl011_uartHwStat;			// Set the Hardware stats function pointer

	return DevTabNum;
}