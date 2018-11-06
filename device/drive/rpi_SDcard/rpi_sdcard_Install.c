/**
* @file rpi_sdcard_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <stdint.h>
#include <xinu.h>
#include <sdmc.h>
#include <conf.h>
#include "rpi-sdcard.h"


struct	sdmcblk	sdmctab[NSDDEV];

xinu_devcall rpi_sdcard_Install (const char* sdmcdevname, unsigned int sdmcctrlnum, uintptr_t addr)
{
	/* Set the actual data on the gpio tab */
	struct sdmcblk *sdmcptr;
	sdmcptr = &sdmctab[sdmcctrlnum].ctrl;			// Pointer to gpio pin control device entry
	sdmcptr->reg_data = addr;						// Hold the address to GPIO registers
	sdmcptr->name =sdmcdevname;						// Hold the device name

	return sdmcctrlnum;
}