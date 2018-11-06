/*
 * @file     xsh_rdate.c
 *
 */
/* Embedded Xinu, Copyright (C) 2008, 2018.  All rights reserved. */

#include <xinu.h>
#include <stdio.h>
#include <string.h>
#include <date.h>
#include <ipv4.h>

#if NETHER
/**
 * @ingroup shell
 *
 * Shell command (rdate).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK for success, SYSERR for syntax error
 */
shellcmd xsh_rdate(int nargs, char *args[])
{

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strcmp(args[1], "--help") == 0)
    {
        printf("Usage: %s <host>\n\n", args[0]);
        printf("Description:\n");
        printf("\tDisplays current time\n");
        printf("Options:\n");
        printf("\t<host>\tremote machine from which to get the time.\n");
        printf
            ("\t\tIf none specified, defaults to value set at compile time.\n");
        printf("\t--help\tdisplay this help and exit\n");
        return OK;
    }

    /* Check for correct number of arguments */
    if (nargs > 2)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return SYSERR;
    }
    if (nargs < 2)
    {
        if (SYSERR == rdate(RDATE_SERVER))
        {
            printf("Server %s: Not responding due to timeout.\n",
                   RDATE_SERVER);
        }
        else
        {
            printDate(get_datetime());
        }

        return OK;
    }

#if NETHER
    if (SYSERR == rdate(args[1]))
    {
        printf("Server %s: Not responding due to timeout.\n", args[1]);
    }
    else
    {
        printDate(get_datetime());
    }
#endif

    return OK;
}
#endif /* NETHER */
