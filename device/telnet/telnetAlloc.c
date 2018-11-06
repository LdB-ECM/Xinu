/**
 * @file telnetAlloc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <conf.h>
#include <stddef.h>

#include <CriticalSection.h>
#include <telnet.h>

/**
 * @ingroup telnet
 *
 * Allocate an available telnet device.
 * @return device number for a telnet device, SYSERR if none are free
 */
int telnetAlloc(void)
{
    int i;

	ENTER_KERNEL_CRITICAL_SECTION();
    for (i = 0; i < NTELNET; ++i)
    {
        if (TELNET_STATE_FREE == telnettab[i].state)
        {
            telnettab[i].state = TELNET_STATE_ALLOC;
			EXIT_KERNEL_CRITICAL_SECTION();
            return i + TELNET0;
        }
    }
	EXIT_KERNEL_CRITICAL_SECTION();

    return SYSERR;
}
