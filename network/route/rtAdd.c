/** 
 * @file rtAdd.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <network.h>
#include <route.h>

/**
 * @ingroup route
 */
xinu_syscall rtAdd(const struct netaddr *dst, const struct netaddr *gate,
                   const struct netaddr *mask, struct netif *nif)
{
    struct rtEntry *rtptr;
    unsigned char octet;
    unsigned short length;
    int i;

    /* Error check pointers */
    if ((NULL == dst) || (NULL == mask) || (NULL == nif))
    {
        return SYSERR;
    }
    RT_TRACE("Dest = %d.%d.%d.%d", dst->addr[0], dst->addr[1],
             dst->addr[2], dst->addr[3]);
    RT_TRACE("Mask = %d.%d.%d.%d", mask->addr[0], mask->addr[1],
             mask->addr[2], mask->addr[3]);
    if (gate)
        RT_TRACE("Gate = %d.%d.%d.%d", gate->addr[0], gate->addr[1],
                 gate->addr[2], gate->addr[3]);
    RT_TRACE("nif = %d", nif - netiftab);

    /* Allocate an entry in the route table */
    rtptr = rtAlloc();
    if ((SYSERR == (int)rtptr) || (NULL == rtptr))
    {
        return SYSERR;
    }

    /* Populate the entry */
    netaddrcpy(&rtptr->dst, dst);
    netaddrmask(&rtptr->dst, mask);
    if (NULL == gate)
    {
        rtptr->gateway.type = NULL;
    }
    else
    {
        netaddrcpy(&rtptr->gateway, gate);
    }
    netaddrcpy(&rtptr->mask, mask);
    rtptr->nif = nif;

    /* Calculate mask length */
    length = 0;
    for (i = 0; i < mask->len; i++)
    {
        octet = mask->addr[i];
        // This totals non-zero bits
        while (octet > 0)
        {
            if (octet & 0x01)
            {
                length++;
            }
            octet = octet >> 1;
        }
    }
    rtptr->masklen = length;

    rtptr->state = RT_USED;
    return OK;
}
