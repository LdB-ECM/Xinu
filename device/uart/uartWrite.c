/**
 * @file uartWrite.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdint.h>
#include <xinu.h>
#include <uart.h>
#include <CriticalSection.h>


/**
 * @ingroup uartgeneric
 *
 * Write a buffer of data to a UART.
 *
 * Caveat: this operates asynchronously, so the data written may be held in an
 * internal buffer and not yet actually written to the hardware.  The UART
 * driver's lower half (interrupt handler; see uartInterrupt()) is responsible
 * for actually writing the data to the hardware.  Exception: when the UART
 * transmitter is idle, uartWrite() can directly write one byte to the hardware.
 *
 * @param devptr
 *      Pointer to the device table entry for a UART.
 * @param buf
 *      Pointer to the buffer of data to write.
 * @param len
 *      Number of bytes to write.
 *
 * @return
 *      On success, returns the number of bytes written, which normally is @p
 *      len, but may be less than @p len if the UART has been set to
 *      non-blocking mode.  Returns ::SYSERR on other error (currently, only if
 *      uartInit() has not yet been called).
 */
xinu_devcall uartWrite (device *devptr, const void *buf, unsigned int len)
{
    struct uart *uartptr;
    unsigned int count;
	unsigned char* inbuf = (unsigned char*)buf;

    /* Disable interrupts and get a pointer to the UART structure and a pointer
     * to the UART's hardware registers.  */
	ENTER_KERNEL_CRITICAL_SECTION();
    uartptr = &uarttab[devptr->minor];

    /* Make sure uartInit() has run.  */
    if (NULL == uartptr->csr)
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    /* Attempt to write each byte in the buffer.  */
    for (count = 0; count < len; count++)
    {
        /* If the UART is in non-blocking mode, ensure there is space for a
            * byte in the output buffer for the lower half (interrupt handler).
            * If not, return early with a short count.  */
		if ((uartptr->ocount == UART_OBLEN) && 
			(uartptr->oflags & UART_OFLAG_NOBLOCK) )
		{
			break;
		}      
		/* Wait for semaphore to become free .. if blocked thread will sleep in there */
		wait(uartptr->osema);
        uartptr->out[(uartptr->ostart + uartptr->ocount) % UART_OBLEN] = inbuf[count];
		uartptr->ocount++;										// Increment output count
    }

	/* Restore interrupts and return the number of bytes written.  */
	EXIT_KERNEL_CRITICAL_SECTION();

	/* Kick transmission if needed */
	if ((uartptr->oidle == 0) && (uartptr->ocount > 0) && (uartptr->uartKickTx)) uartptr->uartKickTx(uartptr);


    return count;
}
