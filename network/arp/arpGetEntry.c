/**
 * @file arpGetEntry.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <arp.h>
#include <clock.h>
#include <CriticalSection.h>

/**
 * @ingroup arp
 *
 * Obtains an entry from the ARP table given a protocol address.
 * @param praddr protocol address
 * @return entry for correspoding praddr in ARP table, NULL if none exists
 */
struct arpEntry *arpGetEntry(const struct netaddr *praddr)
{
    struct arpEntry *entry = NULL;  /**< pointer to ARP table entry   */

    ARP_TRACE("Getting ARP entry");
	ENTER_KERNEL_CRITICAL_SECTION();

    /* Loop through ARP table */
    for (int i = 0; i < ARP_NENTRY; i++)
    {
        /* If ARP entry is not used, skip to next entry */
        if (!(ARP_USED & arptab[i].state))
        {
            continue;
        }

        entry = &arptab[i];
        /* Check if entry has timed out */
        if (entry->expires < clktime)
        {
            ARP_TRACE("\tEntry %d expired", i);
            arpFree(entry);
            continue;
        }

        /* Check if protocol type and address match */
        if (netaddrequal(&entry->praddr, praddr))
        {
			EXIT_KERNEL_CRITICAL_SECTION();
            ARP_TRACE("\tEntry %d matches", i);
            return entry;
        }
    }

	EXIT_KERNEL_CRITICAL_SECTION();
    ARP_TRACE("\tNo entry matches");
    return NULL;
}
