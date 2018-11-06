/**
 * @file     xsh_uartstat.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <uart.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if NUART
static void uartThroughput(unsigned short);

/**
 * @ingroup shell
 *
 * Shell command (uartstat UARTNUM) provides information about the current 
 * status of a UART.
 * @param nargs number of arguments in args array
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_uartstat(int nargs, char *args[])
{
    unsigned short dev, throughput = FALSE;
    char n;

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strcmp(args[1], "--help") == 0)
    {
        printf("Usage: %s [-t] <UARTNUM>\n\n", args[0]);
        printf("Description:\n");
        printf("\tDisplays statistics for uart <UARTNUM>.  If no \n");
        printf("\t<UARTNUM> is specified, statistics are displayed for ");
        printf("all uarts.\n");
        printf("Options:\n");
        printf
            ("\t-t\t\tdisplay throughput statistics for uart <UARTNUM> ");
        printf("every second\n");
        printf("\t<UARTNUM>\t");
        printf("the number of the uart to display statistics for\n");
        printf("\t--help\t\tdisplay this help and exit\n");
        return 1;
    }

    /* Check for correct number of arguments */
    if (nargs < 2)
    {
        for (n = 0; n < NUART; n++)
        {
            uartStat(n);
        }

        return 0;
    }
    else if (2 == nargs)
    {
        dev = (unsigned short)atol(args[1]);
    }
    else if (3 == nargs)
    {
        if (strcmp(args[1], "-t") != 0)
        {
            fprintf(stderr, "Invalid argument '%s', try %s --help\n",
                    args[1], args[0]);
            return 1;
        }
        dev = (unsigned short)atol(args[2]);
        throughput = TRUE;
    }
    else                        //if (nargs > 3)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }

    /* Check for valid device */
    if (dev >= NUART)
    {
        fprintf(stderr, "%s: (%d) No such uart\n", args[0], dev);
        return 1;
    }

    if (throughput)
    {
        uartThroughput(dev);
        return 0;
    }

    uartStat(dev);

    return 0;
}

static void uartThroughput(unsigned short uartnum)
{
    unsigned int cout, cin;

    struct uart *uptr;
    uptr = &uarttab[uartnum];

    while (TRUE)
    {
        cout = uptr->cout;
        cin = uptr->cin;
        sleep(1000);            /* sleep for 1 second */
        printf("Tx: %4d bytes/sec, ", uptr->cout - cout);
        printf("Rx: %4d bytes/sec\n", uptr->cin - cin);
    }
}
#endif /* NUART */
