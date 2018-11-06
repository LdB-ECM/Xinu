/**
 * @file uartInit.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <uart.h>
#include <interrupt.h>

/**
 * @ingroup uartgeneric
 *
 * Global table of UART devices that are available in the current Embedded Xinu
 * configuration.
 */
struct uart uarttab[NUART] = { 0 };

/**
 * @ingroup uartgeneric
 *
 * Initialize a UART, including the entry in Xinu's UART table as well as the
 * hardware itself.
 *
 * @param devptr
 *      Pointer to the device table entry for the UART.
 *
 * @return OK on success; SYSERR on failure.
 */
xinu_devcall uartInit(device *devptr)
{
    /* Initialize the entry for this UART in Xinu's UART table.  */

    struct uart *uartptr = &uarttab[devptr->minor];

    /* Initialize statistical counts. */
    uartptr->cout = 0;
    uartptr->cin = 0;
    uartptr->lserr = 0;
    uartptr->ovrrn = 0;
    uartptr->iirq = 0;
    uartptr->oirq = 0;

    /* Initialize the input buffer, including a semaphore for threads to wait
     * on.  */
    uartptr->isema = semcreate(0);
    uartptr->iflags = 0;
    uartptr->istart = 0;
    uartptr->icount = 0;
    if (isbadsem(uartptr->isema))
    {
        return SYSERR;
    }

    /* Initialize the output buffer, including a semaphore for threads to wait
     * on.  */
    uartptr->osema = semcreate(UART_OBLEN);
    uartptr->oflags = 0;
    uartptr->ostart = 0;
    uartptr->ocount = 0;
    uartptr->oidle = 0;
    if (isbadsem(uartptr->osema))
    {
        semfree(uartptr->isema);
        return SYSERR;
    }

	/* Save pointers to the device and the hardware registers in the UART structure.  */
	//uartptr->dev = devptr;
	//uartptr->csr = devptr->csr;

    /* Initialize the actual hardware.  */

	/* DCB all setup ... setup the port */
	if (uartptr->SetCommStateFn)
		uartptr->SetCommStateFn(uartptr, &uartptr->dcb);

	/* Register the UART's interrupt handler with XINU's interrupt vector, then
	 * actually enable the UART's interrupt line.  */
	set_interrupt_handler(devptr->irq, devptr->intr);
	enable_irq(devptr->irq);

	return OK;
}
