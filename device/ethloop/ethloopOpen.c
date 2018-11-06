/**
 * @file     ethloopOpen.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <string.h>
#include <xinu.h>
#include <bufpool.h>
#include <device.h>
#include <ethloop.h>
#include <CriticalSection.h>
#include <semaphore.h>
#include <stdlib.h>

/**
 * @ingroup ethloop
 *
 * Open an ethloop device. 
 * @param devptr ethloop device table entry
 * @return OK if ethloop is opened properly, otherwise SYSERR
 */
xinu_devcall ethloopOpen (device *devptr, va_list ap)
{
    struct ethloop *elpptr;
    int retval = SYSERR;

    elpptr = &elooptab[devptr->minor];								// Fetch ethernet loop device
	ENTER_KERNEL_CRITICAL_SECTION();								// Disable interrupt masks

    /* Make sure the ethloop is actually closed */
	if (ELOOP_STATE_FREE == elpptr->state)							// Check ethloop is closed
	{
		/* Clear flags and stats */
		elpptr->flags = 0;											// Clear flags
		elpptr->nout = 0;											// Clear out count

		/* Create semaphore and check its valid */
		elpptr->sem = semcreate(0);									// Craete a semaphore
		if (SYSERR != (int)elpptr->sem)								// Check it's valid
		{
			
			/* Create hold semaphore and check its valid */
			elpptr->hsem = semcreate(0);							// Create a semaphore
			if (SYSERR != (int)elpptr->hsem)						// Check its valid
			{
				/* Initialize buffers */
				memset(elpptr->buffer, 0, sizeof(elpptr->buffer));	// Clear the buffer
				memset(elpptr->pktlen, 0, sizeof(elpptr->pktlen));	// Clear buffer
				elpptr->index = 0;									// Zero index
				elpptr->hold = NULL;								// Clear hold pointer
				elpptr->holdlen = 0;								// Hold length is zero
				elpptr->count = 0;									// Count is zero

				/* Allocate a buffer pool  */
				elpptr->poolid = bfpalloc(ELOOP_BUFSIZE, ELOOP_NBUF);
				if (SYSERR != elpptr->poolid)						// Check for an allocation error
				{	
					/* Link ethloop record with device table entry and mark ethloop as open */
					elpptr->state = ELOOP_STATE_ALLOC;				// Set we are in allovated buffer state
					elpptr->dev = devptr;							// Set device pointer

					/* Success */
					retval = OK;									// Set status to success which also stops semaphore release
				}	
				if (retval != OK) semfree(elpptr->hsem);			// If in error free hsem semaphore
			}
			if (retval != OK) semfree(elpptr->sem);					// If in error free sem seamphore
		}
	}
	EXIT_KERNEL_CRITICAL_SECTION();									// Restore interrupt mask
    return retval;													// Return result value
}
