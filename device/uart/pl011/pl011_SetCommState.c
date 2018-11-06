/**
 * @file uart_SetCommState.c
 */
 /* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <stdint.h>
#include <xinu.h>
#include <uart.h>
#include <conf.h>
#include "pl011.h"


/* This is an internal call if you get here the deviceis valid */
int pl011_SetCommState (struct uart* uart, LPDCB  dcb)
{
	uint32_t baudrate, divisor, fracpart;
	int result = SYSERR;
	volatile struct PL011UARTRegisters* PL011UART;
	PL011UART = (volatile struct PL011UARTRegisters*)(uart->csr);	// Pointer to registers

	/* The assumption made is the UART clock was set to 4Mhz */
	baudrate = uart->dcb.BaudRate;									// DCB contains the baudrate
	divisor = 4000000 / (baudrate * 16);							// Calculate the major divisor from clock
	fracpart = 4000000 - (divisor * baudrate * 16);					// Fraction left
	fracpart *= 4;													// fraction part *64 /16 = *4 																		
	fracpart += (baudrate / 2);										// Add half baudrate for rough round
	fracpart /= baudrate;											// Divid the baud rate
	/* Disable the UART by zeroing the "control register".  */
	PL011UART->CR.Raw32 = 0;										// Disable uart while we set it up
	PL011UART->IMSC.Raw32 = 0;										// Disable all interrupt masks
	PL011UART->ICR.Raw32 = 0x7FF;									// Clear all interrupts		

	PL011UART->IBRD.DIVISOR = divisor;								// Set the 16bit integer divisor
	PL011UART->FBRD.DIVISOR = fracpart;								// Write the bits of fraction	
	PL011UART->LCRH.PEN = 0;										// No parity it's not supported
	switch (uart->dcb.datalength)									// based on DCB entry set the data length
	{
		case 2:														// 7 data bits requested
			PL011UART->LCRH.DATALEN = PL011_DATA_7BITS;				// Set 7 data bits up				
			break;
		case 3:														// 8 data bits requested
		default:													// Invalid entry use 8
			PL011UART->LCRH.DATALEN = PL011_DATA_8BITS;				// Set 8 data bits up
			break;
	}

	switch (uart->dcb.stopbits)										// based on DCB entry set the stop
	{
		case 2:														// 2 stop bits requested
			PL011UART->LCRH.STP2 = 1;								// Set 2 stop bit flag				
			break;
		case 0:														// 1 stop bit requested
		default:													// Invalid entry use 8
			PL011UART->LCRH.STP2 = 0;								// Set 1 stop bit flag
			break;
	}

	PL011UART->IFLS.TXIFLSEL = TXIFLSEL_EIGHTH;
	PL011UART->IFLS.RXIFLSEL = RXIFLSEL_EIGHTH;

	/* Set the interrupt masks we will respond to */
	PL011UART->IMSC.Raw32 = 0;										// Initially clear all
	PL011UART->IMSC.RXIM = 1;										// Set RX interrupt mask
	PL011UART->IMSC.TXIM = 0;										// Set TX interrupt mask
	uart->oidle = 0;												// Uart is in idle state

	PL011UART->LCRH.FEN = 0;										// Fifo's enabled
	PL011UART->CR.UARTEN = 1;										// Uart enable
	PL011UART->CR.RXE = 1;											// Transmit enable
	PL011UART->CR.TXE = 1;											// Receive enable

	return result;
}