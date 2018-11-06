/**
 * @file uartHwStat.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <stdio.h>
#include <uart.h>
#include "pl011.h"


#define PL011_IMSC_TXIM   (1<<5)  //transmit interrupt mask bit, if 1: this interrupt is enabled
#define PL011_IMSC_RXIM   (1<<4)  //receive interrupt mask bit, if 1: this interrupt is enabled

void pl011_uartHwStat (void *csr)
{
    static const char * const label[2] = { "OFF", "ON " };
    unsigned int enabled;
	volatile struct PL011UARTRegisters* PL011UART;
	PL011UART = (volatile struct PL011UARTRegisters*)(csr);	// Pointer to registers

    printf("\n\tINTERRUPT ENABLE:\n");
    printf("\t------------------------------------------\n");

    enabled = PL011UART->IMSC.RXIM ? 1 : 0;
    printf("\t%s  Receiver FIFO Reached Trigger Level\n", label[enabled]);

	printf("\t%d  UARTRXINTR raw Level\n", PL011UART->RIS.RXRIS);
	printf("\t%d  UARTRTINTR raw Level\n", PL011UART->RIS.RTRIS);

    enabled = PL011UART->IMSC.TXIM ? 1 : 0;
    printf("\t%s  Transmitter FIFO Empty\n", label[enabled]);

	printf("\t%d  UARTTXINTR raw Level\n", PL011UART->RIS.TXRIS);
}
