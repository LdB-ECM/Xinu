/**
 * @file ns16550_SetCommState.c
 */
 /* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <stdint.h>
#include <xinu.h>
#include <uart.h>
#include <conf.h>
#include <rpi-platform.h>
#include <rpi-mailbox.h>
#include "ns16550.h"


#define AUX_ENABLES  (volatile uint32_t*)(RPi_IO_Base_Addr+0x00215004)

/* This is an internal call if you get here the deviceis valid */
int ns16550_SetCommState (struct uart* uart, LPDCB  dcb)
{
	uint32_t  __attribute__((aligned(16))) msg[8] =
	{
		sizeof(msg),					// Message size
		0,								// Response will go here
		MAILBOX_TAG_GET_CLOCK_RATE,		// Get ARM memory tag
		8,								// 8 byte request
		8,								// 8 byte response
		CLK_CORE_ID,				    // CORE CLOCK
		0,								// Clock speed
		0								// Tag end marker
	};
	uint32_t baudrate, divisor;
	volatile struct ns16550_uart_csreg* MINIUART;
	MINIUART = (volatile struct ns16550_uart_csreg*)(uart->csr);	// Pointer to registers

	if ((rpi_mailbox == 0) || (rpi_mailbox(MB_CHANNEL_TAGS, (uint32_t)&msg[0]) != OK))
	{
		return -1;
	}
	baudrate = uart->dcb.BaudRate;									// DCB contains the baudrate
	divisor = (msg[6] / (baudrate * 8)) - 1;						// calculate divisor
	if (divisor <= 0xFFFF) {
		*AUX_ENABLES = 1;											// Enable miniuart

		MINIUART->CNTL.RXE = 0;										// Disable receiver
		MINIUART->CNTL.TXE = 0;										// Disable transmitter

		MINIUART->LCR.DLAB = 1;
		MINIUART->BAUD.DIVISOR = divisor;							// Set the divisor
		MINIUART->LCR.DLAB = 0;

		if (uart->dcb.datalength == 3)
			MINIUART->LCR.DATA_LENGTH = LENGTH_8_BITS;				// Data length = 8 bits
			else MINIUART->LCR.DATA_LENGTH = LENGTH_7_BITS;			// Data length = 7 bits

		/* Clear the FIFO */
		MINIUART->IIR.RXFIFO_CLEAR = 1;								// Clear the RX fifo
		MINIUART->IIR.TXFIFO_CLEAR = 1;								// Clear the TX fifo

		/* Enable interrupts */
		MINIUART->IER.RXEI = 1;										// Interrupt enable on RX fifo
		MINIUART->IER.LSI = 1;										// Interrupt enable on overrun error, parity error, framing error etc
		uart->oidle = 0;											// Transmitter is idle

		MINIUART->CNTL.RXE = 1;										// Enable receiver
		MINIUART->CNTL.TXE = 1;										// Enable transmitter
		return 0;													// Return success
	}

	return -1;
}