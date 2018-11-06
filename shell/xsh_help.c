/** 
 * @file     xsh_help.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <shell.h>
#include <stdio.h>
#include <string.h>

/**
 * @ingroup shell
 *
 * Shell command (help) provides a list of commands recognized by the
 * shell, or displays help information for a particular command.
 * @param nargs number of arguments in args array
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_help(int nargs, char *args[])
{
    unsigned char i;
    char *command_args[2];      /* temporary storage for [command] --help */

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strcmp(args[1], "--help") == 0)
    {
        printf("Usage:\n");
        printf("\t%s [command]\n", args[0]);
        printf("Description:\n");
        printf("\tProvides a list of commands for the shell.\n");
        printf("\tIf command is provided, help information will\n");
        printf("\tbe provided for the specified command; equivalent\n");
        printf("\tto entering 'command --help' into the shell.\n");
        printf("Options:\n");
        printf("\tcommand\tcommand name to display for which to\n");
        printf("\t\tdisplay help information\n");
        printf("\t--help\tdisplay this help and exit\n");
        return 0;
    }

    /* Check for correct number of arguments */
    if (nargs > 2)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information.\n",
                args[0]);
        return 1;
    }

    /* Output help for specific command, if 'command' argument was supplied */
    if (nargs == 2)
    {
        for (i = 0; i < ncommand; i++)
        {
            if (strcmp(args[1], commandtab[i].name) == 0)
            {
                command_args[0] = args[1];
                command_args[1] = "--help";
                (*commandtab[i].procedure) (2, command_args);
                return 0;
            }
        }
        printf("%s: no help topics match '%s'.\n", args[0], args[1]);
        printf("  Try '%s --help' for more information.\n", args[0]);
        return 1;
    }

    /* Output command list */
    printf("Shell Commands:\n");
    for (i = 0; i < ncommand; i++)
    {
        printf("\t%s\n", commandtab[i].name);
    }

    return 0;
}
