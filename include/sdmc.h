/**
 * @file sdmc.h
 */
 /* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

/*---------------------------------------------------------------------------*/

#ifndef _SDMC_H_
#define _SDMC_H_

#include <stdbool.h>
#include <stdint.h>
#include <xinu.h>
#include <device.h>
#include <semaphore.h>

/*--------------------------------------------------------------------------}
{		      XINU SD MASTER CONTOLLER BLOCK STRUCTURE					    }
{           Based largely on linux driver options and flags					}
{--------------------------------------------------------------------------*/
struct sdmcblk {
	/* Pointers to associated structures */
	device *dev;								/**< Dev structure that owns this SD controller	*/
	char* name;									/**< Device name								*/

	uintptr_t reg_data;							/**< Pointer address to register for this group */

	semaphore	cmd_sem;						/**<  Command semaphore 						*/
	semaphore	tx_sem;							/**<  Transfer semaphore						*/
	uint32_t	rca;							/**<  Relative card address						*/
	char cid[16];								/**<  Card identifier							*/
	struct {
			unsigned cmd8 : 1;					/**< Card supports CMD8							*/
			unsigned sdio : 1;					/**< Card is an SDIO card						*/
			unsigned rsvd : 6;
	};
};

extern	struct	sdmcblk	sdmctab[];

#endif                          /* _SDMC_H_ */