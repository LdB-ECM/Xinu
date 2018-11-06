/**
 * @file udpAlloc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <CriticalSection.h>
#include <udp.h>

/**
 * @ingroup udpexternal
 *
 * Allocate an available UDP device.
 *
 * @return
 *      Device number for a UDP device, or ::SYSERR if none are free.
 */
unsigned short udpAlloc(void)
{
    int i;

	ENTER_KERNEL_CRITICAL_SECTION();
    for (i = 0; i < NUDP; i++)
    {
        if (UDP_FREE == udptab[i].state)
        {
            udptab[i].state = UDP_ALLOC;
			EXIT_KERNEL_CRITICAL_SECTION();
            return i + UDP0;
        }
    }
	EXIT_KERNEL_CRITICAL_SECTION();

    return SYSERR;
}
