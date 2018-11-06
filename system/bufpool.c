/**
 * @file bufpool.c
 *
 * This is a merge of a number of old Xinu files that were covered by bufpool.h plus new extension
 * The old files inlude bfalloc.c, bfpfree.c, bufget.c, buffree.c
 */
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <xinu.h>
#include <memory.h>
#include <bufpool.h>
#include <CriticalSection.h>

struct bfpentry bfptab[NPOOL] = { 0 };  /* List of memory buffer pools    */

/**
 * @ingroup memory_mgmt
 *
 * Acquire heap storage and subdivide into buffers.
 *
 * @param bufsize
 *      Size of individual buffers, in bytes.
 *
 * @param nbuf
 *      Number of buffers in the pool.
 *
 * @return
 *      On success, returns an identifier for the buffer pool that can be passed
 *      to bufget() or bfpfree().  On failure, returns ::SYSERR.
 */
int bfpalloc (unsigned int bufsize, unsigned int nbuf)
{
    struct bfpentry *bfpptr;
    struct poolbuf *bufptr;
    int id;

    bufsize = roundword(bufsize) + sizeof(struct poolbuf);			// Add the extra byte size for pool buffer struct

    if (bufsize > POOL_MAX_BUFSIZE ||
        bufsize < POOL_MIN_BUFSIZE || nbuf > POOL_MAX_NBUFS || nbuf < 1)
    {
        return SYSERR;												// Return system error as a critical condition not met
    }

	ENTER_KERNEL_CRITICAL_SECTION();								// Disable scheduler we are playing with buffer table entries
    for (id = 0; id < NPOOL; id++)									// For each pool entry
    {
        bfpptr = &bfptab[id];										// Set the pointer to the entry
        if (BFPFREE == bfpptr->state)								// If the entry is free
        {
            break;													// Break out of for loop entry free
        }
    }
    if (NPOOL == id)												// Check if we found a free pool entry
    {
		EXIT_KERNEL_CRITICAL_SECTION();								// We are going to exit so enable switcher again
		return SYSERR;												// No free buffer available so return system error
    }
    bfpptr->state = BFPUSED;										// Set buffer to used state
	EXIT_KERNEL_CRITICAL_SECTION();									// Ok to allow scheduler to operate

    bfpptr->nbuf = nbuf;											// Hold number of buffers
    bfpptr->bufsize = bufsize;										// Hold total size requested
    bufptr = (struct poolbuf *)memget(nbuf * bufsize);				// Allocate buffer as one big block
    if ((void *)SYSERR == bufptr)									// If allocate failed
    {
        bfpptr->state = BFPFREE;									// Set the buffer state to free we are exiting due to error 
		return SYSERR;												// Return system error .. insuffucient memory for pool
    }
    bfpptr->next = bufptr;											// Set next pointer to the allocated memory
    bfpptr->head = bufptr;											// Set the buffer head to the allocated memory
    for (int i = 0; i < nbuf; i++)									// Fill in details for each buffer in pool 
    {
        bufptr->poolid = id;										// Set the id of the pool
        bufptr->next = (struct poolbuf *)((uintptr_t)bufptr + bufsize);
        bufptr = bufptr->next;										// Set next pointer 
    }
	bfpptr->freebuf = semcreate(nbuf);								// Create semaphore on the buffer entry with the count
	if (SYSERR == (int)bfpptr->freebuf)								// Semaphore create failed
	{
		bfpptr->state = BFPFREE;									// Set the buffer state to free we are exiting due to error
		memfree(bfpptr->head, nbuf * bufsize);						// Release the allocated memory tables
		return SYSERR;												// Return system error
	}
    return id;														// Return the allocated pool id
}


/**
 * @ingroup memory_mgmt
 *
 * Frees the memory allocated for a buffer pool.
 *
 * @param poolid
 *      Identifier of the buffer pool to free, as returned by bfpalloc().
 *
 * @return
 *      ::OK if the buffer pool was valid and was successfully freed; otherwise
 *      ::SYSERR.  If @p poolid specified a valid buffer pool, then this
 *      function can only return ::SYSERR as a result of memory corruption.
 */
xinu_syscall bfpfree (int poolid)
{
	xinu_syscall memRes, semRes;
	struct bfpentry *bfpptr;

	ENTER_KERNEL_CRITICAL_SECTION();								// Disable scheduler we are playing with buffer table entries
	if (isbadpool(poolid))											// Check pool id is valid
	{
		EXIT_KERNEL_CRITICAL_SECTION();								// We are going to exit so enable scheduler again
		return SYSERR;												// Bad pool id so return system error
	}
	bfpptr = &bfptab[poolid];										// Fetch the pool buffer pointer from pool id
	bfpptr->state = BFPFREE;										// Set the pool state to free
	memRes = memfree(bfpptr->head, bfpptr->nbuf * bfpptr->bufsize);	// Free memory hold result
	semRes = semfree(bfpptr->freebuf);								// Free semaphore hold result
	EXIT_KERNEL_CRITICAL_SECTION();									// All clear so so enable scheduler again
	if ((memRes == SYSERR) || (semRes == SYSERR))					// One of the releases failed
	{
		return SYSERR;												// Return system error as a problem encountered
	}
	return OK;														// Return okay
}


/**
 * @ingroup memory_mgmt
 *
 * Allocate a buffer from a buffer pool.  If no buffers are currently available,
 * this function wait until one is, usually rescheduling the thread.  The
 * returned buffer must be freed with buffree() when the calling code is
 * finished with it.
 *
 * @param poolid
 *      Identifier of the buffer pool, as returned by bfpalloc().
 *
 * @return
 *      If @p poolid does not specify a valid buffer pool, returns ::SYSERR;
 *      otherwise returns a pointer to the resulting buffer.
 */
void *bufget (int poolid)
{
	struct bfpentry *bfpptr;
	struct poolbuf *bufptr;

	ENTER_KERNEL_CRITICAL_SECTION();								// Disable scheduler we are playing with buffer table entries
	if (isbadpool(poolid))											// Check pool id is valid
	{
		EXIT_KERNEL_CRITICAL_SECTION();								// We are going to exit so enable scheduler again
		return (void *)SYSERR;										// Bad pool id so return system error
	}
	bfpptr = &bfptab[poolid];										// Fetch the pool buffer pointer from pool id
	EXIT_KERNEL_CRITICAL_SECTION();									// All clear so so enable scheduler again we may need to wait
	wait(bfpptr->freebuf);											// Wait for a free buffer
	ENTER_KERNEL_CRITICAL_SECTION();								// Disable scheduler while we fix linking	
	bufptr = bfpptr->next;											// Get pointer to current next pointers next											
	bfpptr->next = bufptr->next;									// Advance the pool next pointer
	bufptr->next = bufptr;											// Buf ptr next points back to itself
	EXIT_KERNEL_CRITICAL_SECTION();									// Linking all done so enable scheduler again
	return (void *)(bufptr + 1);									// +1 to skip past accounting structure .. ptr arithmetic
}


/**
 * @ingroup memory_mgmt
 *
 * Return a buffer to its buffer pool.
 *
 * @param buffer
 *      Address of buffer to free, as returned by bufget().
 *
 * @return
 *      ::OK if buffer was successfully freed; otherwise ::SYSERR.  ::SYSERR can
 *      only be returned as a result of memory corruption or passing an invalid
 *      @p buffer argument.
 */
xinu_syscall buffree (void *buffer)
{
	struct bfpentry *bfpptr;
	struct poolbuf *bufptr;

	bufptr = ((struct poolbuf *)buffer) - 1;						// -1 to skip back accounting structure .. ptr arithmetic
	ENTER_KERNEL_CRITICAL_SECTION();								// Disable scheduler we are playing with buffer table entries
	if (isbadpool(bufptr->poolid) || (bufptr->next != bufptr))		// Check pool id is valid and next pointer points to self
	{
		EXIT_KERNEL_CRITICAL_SECTION();								// We are going to exit so enable scheduler again
		return SYSERR;												// Error from one of the two conditions so return system error
	}
	bfpptr = &bfptab[bufptr->poolid];								// Fetch the pool buffer pointer from pool id
	bufptr->next = bfpptr->next;									// Set the next pointer on this buffer
	bfpptr->next = bufptr;											// Set the pool next pointer to this entry
	EXIT_KERNEL_CRITICAL_SECTION();									// All clear so so enable scheduler again
	signaln(bfpptr->freebuf, 1);									// Signal to any thread waiting for a free buffer
	return OK;														// Return OK
}
