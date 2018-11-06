/**
 * @file uartInterrupt.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <uart.h>
#include <thread.h>
#include "pl011.h"


#define PL011_MIS_OEMIS   (1<<10) //overrun error interrupt status
#define PL011_MIS_BEMIS   (1<<9)  //break error interrupt status
#define PL011_MIS_PEMIS   (1<<8)  //parity error interrupt status
#define PL011_MIS_FEMIS   (1<<7)  //framing error interrupt status
#define PL011_MIS_RTMIS   (1<<6)  //receive timeout interrupt status
#define PL011_MIS_TXMIS   (1<<5)  //transmit interrupt status
#define PL011_MIS_RXMIS   (1<<4)  //receive interrupt status
#define PL011_MIS_DSRMMIS (1<<3)  //DSR interrupt status
#define PL011_MIS_DCDMMIS (1<<2)  //DCD interrupt status
#define PL011_MIS_CTSMMIS (1<<1)  //CTS interrupt status
#define PL011_MIS_RIMMIS  (1<<0)  //RI interrupt status


#define PL011_ICR_OEIC   (1<<10) //overrun error interrupt clear
#define PL011_ICR_BEIC   (1<<9)  //break error interrupt clear
#define PL011_ICR_PEIC   (1<<8)  //parity error interrupt clear
#define PL011_ICR_FEIC   (1<<7)  //framing error interrupt clear
#define PL011_ICR_RTIC   (1<<6)  //receive timeout interrupt clear
#define PL011_ICR_TXIC   (1<<5)  //transmit interrupt clear
#define PL011_ICR_RXIC   (1<<4)  //receive interrupt clear
#define PL011_ICR_DSRMIC (1<<3)  //DSR interrupt clear
#define PL011_ICR_DCDMIC (1<<2)  //DCD interrupt clear
#define PL011_ICR_CTSMIC (1<<1)  //CTS interrupt clear
#define PL011_ICR_RIMIC  (1<<0)  //RI interrupt clear


#define PL011_FR_RI   (1<<8) //set to 1 when ring indicator pin is low
#define PL011_FR_TXFE (1<<7) //set to 1 when TX FIFO/register is empty
#define PL011_FR_RXFF (1<<6) //set to 1 when RX FIFO/register is full
#define PL011_FR_TXFF (1<<5) //set to 1 when TX FIFO/register is full
#define PL011_FR_RXFE (1<<4) //set to 1 when RX FIFO/register is empty
#define PL011_FR_BUSY (1<<3) //set to 1 when UART is transmitting data
#define PL011_FR_DCD  (1<<2) //set to 1 when DCD pin is low
#define PL011_FR_DSR  (1<<1) //set to 1 when DSR pin is low
#define PL011_FR_CTS  (1<<0) //set to 1 when CTS pin is low

/**
 * @ingroup uarthardware
 *
 * Handle an interrupt request from a PL011 UART.
 */
interrupt pl011_uartInterrupt (void)
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
		uint32_t status;
        unsigned char c;
		volatile struct PL011UARTRegisters* PL011UART;
		volatile struct uart *uartptr;

        /* Get a pointer to the UART structure and a pointer to the UART's
         * hardware registers.  */
        uartptr = &uarttab[u];
		PL011UART = (volatile struct PL011UARTRegisters*)(uartptr->csr);	// Pointer to registers
		if (PL011UART == 0) continue;
        /* Read Interrupt Status register we need to determine which interrupts have occurred, then handle them.  */
        status = PL011UART->MIS.Raw32;
        
		if (status & PL011_MIS_TXMIS)
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
             * space remaining in the transmit FIFO.  (If FIFOs are disabled,
             * the Tx holding register acts like a FIFO of size 1, so the code
             * still works.)  Otherwise, the UART is now idle, so set the
             * "oidle" flag, which will allow the next call to uartWrite() to
             * start transmitting again by writing a byte directly to the
             * hardware.  */
			while ((PL011UART->FR.TXFF == 0) && (uartptr->ocount > 0))
            {
				PL011UART->DR.DATA = uartptr->out[uartptr->ostart];
				(void)PL011UART->FR.Raw32;
				uartptr->ostart += 1;
				if (uartptr->ostart >= UART_OBLEN) uartptr->ostart = 0;
                uartptr->ocount--;
                count++;
            } 
			if (uartptr->ocount == 0) {
				uartptr->oidle = 0;
				PL011UART->ICR.TXIC = 1;
			}

            /* One or more bytes were successfully removed from the output
                * buffer and written to the UART hardware.  Increment the total
                * number of bytes written to this UART and signal up to @count
                * threads waiting in uartWrite() to tell them there is now
                * space in the output buffer.  */
			if (count > 0)
			{
				uartptr->cout += count;
				signaln(uartptr->osema, count);
			}

        }
        if ((status & PL011_MIS_RXMIS) || (status & PL011_MIS_RTMIS))
        {
			unsigned int count = 0;
            /* Receive interrupt is asserted.  If FIFOs are enabled, this
             * happens when the amount of data in the receive FIFO is greater
             * than or equal to the programmed trigger level.  If FIFOs are
             * disabled, this happens when the Rx holding register was filled
             * with one byte.  */

            /* Increment number of receive interrupts received on this UART.  */
            uartptr->iirq++;
			PL011UART->ICR.RXIC = 1;
			PL011UART->ICR.RTIC = 1;

            /* Read bytes from the receive FIFO until it is empty again.  (If
             * FIFOs are disabled, the Rx holding register acts as a FIFO of
             * size 1, so the code still works.)  */
            do {
                /* Get a byte from the UART's receive FIFO.  */
                c = PL011UART->DR.DATA;
				(void)PL011UART->FR.Raw32;
                if (uartptr->icount < UART_IBLEN)
                {
                    /* There is space for the byte in the input buffer, so add
                     * it and tally one character received.  */
                    uartptr->in[(uartptr->istart +
                                 uartptr->icount) % UART_IBLEN] = c;
                    uartptr->icount++;
                    count++;
                }
                else
                {
                    /* There is *not* space for the byte in the input buffer, so
                     * ignore it and increment the overrun count.  */
                    uartptr->ovrrn++;
                }
			}  while (PL011UART->FR.RXFE == 0);
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
