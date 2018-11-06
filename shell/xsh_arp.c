/*
 * @file     xsh_arp.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <stdio.h>
#include <string.h>
#include <network.h>
#include <arp.h>

#if NETHER
/**
 * @ingroup shell
 *
 * Shell command (arp).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK for success, SYSERR for syntax error
 */
shellcmd xsh_arp(int nargs, char *args[])
{

    int i;
    char c[32];
    device *pdev;
    struct netif *netptr;

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strcmp(args[1], "--help") == 0)
    {
        printf("Usage: %s\n\n", args[0]);
        printf("Description:\n");
        printf("\tDisplays ARP Information\n");
        printf("Options:\n");
        printf("\t--help\tdisplay this help and exit\n");
        return OK;
    }

    /* Check for correct number of arguments */
    if (nargs > 1)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return SYSERR;
    }

    printf
        ("Address                 HWaddress               Interface\r\n");
    for (i = 0; i < ARP_NENTRY; i++)
    {
        if (arptab[i].state & ARP_USED)
        {
            netaddrsprintf(c, &arptab[i].praddr);
            printf("%-24s", c);

            netaddrsprintf(c, &arptab[i].hwaddr);
            printf("%-24s", c);

            netptr = arptab[i].nif;
            pdev = (device *)&devtab[netptr->dev];
            printf("%s\r\n", pdev->name);
        }
    }

    return OK;
}
#endif /* NETHER */
