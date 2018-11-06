/**
 * @file uartHwPutc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include "ns16550.h"

void ns16550_uartKickTx (struct uart *uartptr)
{
	volatile struct ns16550_uart_csreg *regptr = (volatile struct ns16550_uart_csreg*)(uintptr_t)(uartptr->csr);

	uartptr->oidle = 1;							// We are no longer idle
	if (uartptr->csr) regptr->IER.TXEI = 1;
}
