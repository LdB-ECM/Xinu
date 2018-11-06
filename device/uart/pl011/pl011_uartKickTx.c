/**
 * @file uartHwPutc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <uart.h>
#include <CriticalSection.h>
#include "pl011.h"

#define PL011_UART_IMSC_TXIM   (1 << 5)   // Transmit interrupt mask

void pl011_uartKickTx (struct uart * uartptr)
{
	volatile struct PL011UARTRegisters* PL011UART;
	PL011UART = (volatile struct PL011UARTRegisters*)(uartptr->csr);	// Pointer to registers

	if (PL011UART->RIS.TXRIS == 0)  // Interrupt not already on
	{
		ENTER_KERNEL_CRITICAL_SECTION();

		uartptr->oidle = 1;
		PL011UART->IMSC.TXIM = 1;

		PL011UART->DR.DATA = uartptr->out[uartptr->ostart];
		uartptr->ostart += 1;
		if (uartptr->ostart >= UART_OBLEN) uartptr->ostart = 0;
		uartptr->ocount--;
		uartptr->cout += 1;
		signaln(uartptr->osema, 1);

		EXIT_KERNEL_CRITICAL_SECTION();
	}
}
