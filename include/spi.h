/**
 * @file spi.h
 */
 /* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

/*---------------------------------------------------------------------------*/

#ifndef _SPI_H_
#define _SPI_H_

#include <stdbool.h>
#include <stdint.h>
#include <xinu.h>
#include <device.h>
#include <semaphore.h>

/*--------------------------------------------------------------------------}
{		        	XINU SPI CONTOL BLOCK STRUCTURE						    }
{           Based largely on linux driver options and flags					}
{--------------------------------------------------------------------------*/
typedef struct __attribute__((__packed__)) _SPI_CB {
	unsigned int max_speed_hz;			//			The Maximum clock rate to be used with this chip

	/* 16 bit pack fields to ensure alignment */
	unsigned chip_select : 8;		
	unsigned bits_per_word : 8;

	/* 16 bit pack fields to ensure alignment */
	enum {
		SPI_MODE_1 = 0, 
		SPI_MODE_2 = 1,
		SPI_MODE_3 = 2,  
	} SPI_MODE : 2;								// SPI operation mode 
	unsigned SPI_CS_HIGH : 1;					// SPI marks high
	unsigned SPI_LSB_FIRST : 1;					// LSB transmitted first
	unsigned SPI_3WIRE : 1;						// SPI is in 3 wire mode
	unsigned SPI_LOOP : 1;						// SPI is in loop mode
	unsigned SPI_NO_CS : 1;
	unsigned SPI_READY : 1;
	unsigned SPI_TX_DUAL : 1;
	unsigned SPI_TX_QUAD : 1;
	unsigned SPI_RX_DUAL : 1;
	unsigned SPI_RX_QUAD : 1;
	unsigned unused : 4;
} SPI_CB;


/*--------------------------------------------------------------------------}
{						XINU SPI DEVICE STRUCTURE						    }
{			This is based on linux SPI device options and flags				}
{--------------------------------------------------------------------------*/
struct spi_device {
	/* Pointers to associated structures */
	device *dev;								/**< Dev structure that owns this SPI	*/
	spi_controller *controller;					/**< SPI controller						*/

	/* Internal SPI CPNTROL BLOCK record */
	SPI_CB	spi_cb;								/**< Internal SPI control block record	*/

	/* SPI input fields */
	semaphore isema;							/**< Input semaphore					*/
	unsigned short istart;						/**< Index of first byte                */
	unsigned short icount;						/**< Bytes in input buffer              */
	unsigned char* in[];						/**< Input buffer pointer               */

	/* SPI output fields */
	semaphore osema;							/**< Output semaphore					*/
	unsigned short ostart;						/**< Index of first byte                */
	unsigned short ocount;						/**< Bytes in output buffer             */
	unsigned char* out[];						/**< Output buffer pointer	            */

};

extern struct spi_device spitab[];


#endif                          /* _GPIO_H_ */