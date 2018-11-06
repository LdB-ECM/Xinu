/**
 * @file     uartStat.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <stdio.h>
#include <uart.h>

/**
 * @ingroup uartgeneric
 *
 * Prints information about the current status of a UART.
 *
 * @param uartnum
 *      The minor number of the UART.
 */
void uartStat(unsigned short uartnum)
{
    struct uart *uartptr = &uarttab[uartnum];

    if (NULL == uartptr->dev)
    {
        /* uartInit() not run yet */
        return;
    }

    printf("%s:\n", uartptr->dev->name);

    printf("\tSTATISTICS:\n");
    printf("\t------------------------------------------\n");
    printf("\t%8d Characters Output\n", uartptr->cout);
    printf("\t%8d Characters Input\n", uartptr->cin);
    printf("\t%8d Characters Overrun\n", uartptr->ovrrn);
    printf("\t%8d Receiver Error Count\n", uartptr->lserr);
    printf("\t%8d Output IRQ Count\n", uartptr->oirq);
    printf("\t%8d Input IRQ Count\n", uartptr->iirq);
	printf("\t%8d Output buffer count\n", uartptr->ocount);
	printf("\t%8d oIdle state\n", uartptr->oidle);

    if (NULL != uartptr->csr &&  uartptr->uartHwStat)
    {
		uartptr->uartHwStat(uartptr->csr);
    }

    printf("\n");
}
