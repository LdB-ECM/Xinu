/**
 * @file etherClose.c
 */
/* Embedded Xinu, Copyright (C) 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <ether.h>

/* Implementation of etherClose() see details of this function in ether.h.  */
xinu_devcall etherClose (device *devptr)
{
    /* TODO: need to handle canceling all the outstanding USB requests, etc. */
    return SYSERR;
}
