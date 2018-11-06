/**
 * @file uartInterrupt.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <stdint.h>
#include <xinu.h>
#include <thread.h>
#include <device.h>
#include <uart.h>
#include "ns16550.h"

extern int resdefer;

/**
 * @ingroup ns16550hardware
 *
 * Decode hardware interrupt request from UART device.
 */
interrupt ns16550_uartInterrupt(void)
{
	/* Set resdefer to prevent other threads from being scheduled before this
	 * interrupt handler finishes.  This prevents this interrupt handler from
	 * being executed re-entrantly.  */
	extern int resdefer;
	resdefer = 1;

	/* Check for interrupts on each UART.  Note: this assumes all the UARTs in
	 * 'uarttab' are PL011 UARTs.  */
	for (int u = 0; u < NUART; u++)
	{
		uint32_t mis;
		volatile struct ns16550_uart_csreg *MINIUART;
		struct uart *uartptr;


		/* Get a pointer to the UART structure and a pointer to the UART's
		 * hardware registers.  */
		uartptr = &uarttab[u];
		MINIUART = (volatile struct ns16550_uart_csreg*)(uartptr->csr); // Create a pointer to the hardware
		if (MINIUART == 0) continue;

		/* Check the Masked Interrupt Status register to determine which
		 * interrupts have occurred, then handle them.  */
		mis = MINIUART->IIR.Raw32;
		if (mis & 0x2)
		{
			unsigned int count = 0;

			/* Transmit interrupt is asserted.  If FIFOs are enabled, this
			 * happens when the amount of data in the transmit FIFO is less than
			 * or equal to the programmed trigger level.  If FIFOs are disabled,
			 * this happens if the Tx holding register is empty.  */

			 /* Increment number of transmit interrupts received on this UART.
			  * */
			uartptr->oirq++;

			/* If there are bytes pending in the output buffer, write them to
			 * the UART until either there are no bytes remaining or there is no
			 * space remaining in the transmit FIFO. */
			while ((MINIUART->LSR.TXFE == 1) && (uartptr->ocount > 0))
			{
				MINIUART->IO.DATA = uartptr->out[uartptr->ostart];
				(void)MINIUART->LSR.Raw32;
				uartptr->ostart += 1;
				if (uartptr->ostart >= UART_OBLEN) uartptr->ostart = 0;
				uartptr->ocount--;
				count++;
			}

			if (uartptr->ocount == 0) {
				uartptr->oidle = 0;
				MINIUART->IER.TXEI = 0;
			}
			if (count > 0)
			{
				uartptr->cout += count;
				signaln(uartptr->osema, count);
			}

		}
		if (mis & 0x4)
		{
			unsigned int count = 0;

			/* Receive interrupt is asserted.  If FIFOs are enabled, this
			 * happens when the amount of data in the receive FIFO is greater
			 * than or equal to the programmed trigger level.  If FIFOs are
			 * disabled, this happens when the Rx holding register was filled
			 * with one byte.  */

			 /* Increment number of receive interrupts received on this UART.  */
			uartptr->iirq++;

			/* Number of bytes successfully buffered so far.  */
			count = 0;

			/* Read bytes from the receive FIFO until it is empty again.  (If
			 * FIFOs are disabled, the Rx holding register acts as a FIFO of
			 * size 1, so the code still works.)  */
			do
			{
				/* Get a byte from the UART's receive FIFO.  */
				unsigned char c = MINIUART->IO.DATA;
				if (uartptr->icount < UART_IBLEN)
				{
					/* There is space for the byte in the input buffer, so add
					 * it and tally one character received.  */
					uartptr->in[(uartptr->istart +	uartptr->icount) % UART_IBLEN] = c;
					uartptr->icount++;
					count++;
				}
				else
				{
					/* There is *not* space for the byte in the input buffer, so
					 * ignore it and increment the overrun count.  */
					uartptr->ovrrn++;
				}
			} while (MINIUART->LSR.RXFDA == 1);
			/* The receive interrupt will have been automatically cleared
			 * because we read bytes from the receive FIFO until it became
			 * empty.  */

			 /* Increment cin by the number of bytes successfully buffered and
			  * signal up to that many threads that are currently waiting in
			  * uartRead() for buffered data to become available.  */
			if (count > 0)
			{
				uartptr->cin += count;
				signaln(uartptr->isema, count);
			}
		}
	}

	/* Now that the UART interrupt handler is finished, we can safely wake up
	 * any threads that were signaled.  */
	if (--resdefer > 0)
	{
		resdefer = 0;
		resched();
	}

}

