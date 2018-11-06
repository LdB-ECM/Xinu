/**
 * @file uart_GetCommState.c
 */
 /* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <string.h>
#include <xinu.h>
#include <uart.h>
#include <conf.h>
#include <CriticalSection.h>

int GetCommState (HANDLE hFile, LPDCB  dcb)
{
	int result = SYSERR;
	struct uart *uartptr;

	/* Check the handle is actually valid */
	if (hFile < 0 || hFile > NDEVS) return SYSERR;

	/* Disable interrupts and get a pointer to the UART structure.  */
	ENTER_KERNEL_CRITICAL_SECTION();
	uartptr = &uarttab[devtab[hFile].minor];

	/* Make sure uartInit() has run */
	if (uartptr->csr)
	{

		/* If there exists a GetCommState function to override default call it */
		if (uartptr->GetCommStateFn)
		{
			result = uartptr->GetCommStateFn(uartptr, dcb);			// Call override function pointer
		}
		else {
			/* Most devices simply copy cack the internal DCB block */
			if (dcb)												// Check the pointer is valid 
			{
				memcpy(dcb, &uartptr->dcb, sizeof(dcb));
				result = 1;											// Return success
			}
		}

	}

	/* Restore interrupts and return the API result status */
	EXIT_KERNEL_CRITICAL_SECTION();
	return result;
}