/**
 * @file rpi_sdcard_init.c
 *
 * Initialization for the Raspbeery Pi SD controller.
 */
/* Embedded Xinu, Copyright (C) 2013, 2018.  All rights reserved. */

#include <stdbool.h>
#include <stdint.h>
#include "rpi-sdcard.h"


struct	sdmcblk	sdmctab[NSDDEV];

/*------------------------------------------------------------------------
 *  sdmcinit - initialize the SD memory card device
 *------------------------------------------------------------------------
 */
xinu_devcall sdmcinit( struct dentry *devptr )
{
	

	return OK;
}
