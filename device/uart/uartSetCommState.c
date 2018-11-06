/**
 * @file uart_SetCommState.c
 */
 /* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <xinu.h>
#include <uart.h>
#include <conf.h>
#include <CriticalSection.h>

int SetCommState (HANDLE hFile, LPDCB  dcb)
{
	int result = SYSERR;
	struct uart *uartptr;

	/* Check the handle is actually valid */
	if (hFile < 0 || hFile > NDEVS) return SYSERR;

	/* Disable interrupts and get a pointer to the UART structure.  */
	ENTER_KERNEL_CRITICAL_SECTION();
	uartptr = &uarttab[devtab[hFile].minor];

	/* Make sure uartInit() has run and this comport has a setting function */
	/* Some devices are hard coded and can not be set but can be read       */
	if (uartptr->csr && uartptr->SetCommStateFn)
	{

		/* Simply call the function pointer ... the specific hardware will do it's thing */
		result = uartptr->SetCommStateFn(uartptr, dcb);

	}

	/* Restore interrupts and return the API result status */
	EXIT_KERNEL_CRITICAL_SECTION();
	return result;
}