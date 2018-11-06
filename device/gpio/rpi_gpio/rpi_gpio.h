/**
 * @file rpi-gpio.h
 */
 /* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

/*---------------------------------------------------------------------------*/

#ifndef _RPI_GPIO_H_
#define _RPI_GPIO_H_

#include <stdint.h>
#include <xinu.h>

/***************************************************************************}
{					     PUBLIC ENUMERATION CONSTANTS			            }
****************************************************************************/

/*--------------------------------------------------------------------------}
;{	      ENUMERATED FSEL REGISTERS ... BCM2835.PDF MANUAL see page 92		}
;{-------------------------------------------------------------------------*/
/* In binary so any error is obvious */
typedef enum {
	GPIO_INPUT = 0b000,									// 0
	GPIO_OUTPUT = 0b001,								// 1
	GPIO_ALTFUNC5 = 0b010,								// 2
	GPIO_ALTFUNC4 = 0b011,								// 3
	GPIO_ALTFUNC0 = 0b100,								// 4
	GPIO_ALTFUNC1 = 0b101,								// 5
	GPIO_ALTFUNC2 = 0b110,								// 6
	GPIO_ALTFUNC3 = 0b111,								// 7
} GPIOMODE;

/*--------------------------------------------------------------------------}
;{	    ENUMERATED GPIO FIX RESISTOR ... BCM2835.PDF MANUAL see page 101	}
;{-------------------------------------------------------------------------*/
/* In binary so any error is obvious */
typedef enum {
	NO_RESISTOR = 0b00,									// 0
	PULLUP = 0b01,										// 1
	PULLDOWN = 0b10,									// 2
} GPIO_FIX_RESISTOR;

/*==========================================================================}
{    RASPBERRY PI GPIO HARDWARE REGISTERS - BCM2835.PDF Manual Section 6	}
{--------------------------------------------------------------------------*/
/*  The packing and alignment is required for 64 bit compiler .. plz leave  }
{==========================================================================*/
struct __attribute__((__packed__, aligned(4))) GPIORegisters
{
	uint32_t GPFSEL[6];												// 0x00  GPFSEL0 - GPFSEL5
	uint32_t reserved1;												// 0x18  reserved
	uint32_t GPSET[2];												// 0x1C  GPSET0 - GPSET1;
	uint32_t reserved2;												// 0x24  reserved
	uint32_t GPCLR[2];												// 0x28  GPCLR0 - GPCLR1
	uint32_t reserved3;												// 0x30  reserved
	const uint32_t GPLEV[2];										// 0x34  GPLEV0 - GPLEV1   ** Read only hence const
	uint32_t reserved4;												// 0x3C  reserved
	uint32_t GPEDS[2];												// 0x40  GPEDS0 - GPEDS1 
	uint32_t reserved5;												// 0x48  reserved
	uint32_t GPREN[2];												// 0x4C  GPREN0 - GPREN1;	 
	uint32_t reserved6;												// 0x54  reserved
	uint32_t GPFEN[2];												// 0x58  GPFEN0 - GPFEN1;
	uint32_t reserved7;												// 0x60  reserved
	uint32_t GPHEN[2];												// 0x64  GPHEN0 - GPHEN1;
	uint32_t reserved8;												// 0x6c  reserved
	uint32_t GPLEN[2];												// 0x70  GPLEN0 - GPLEN1;
	uint32_t reserved9;												// 0x78  reserved
	uint32_t GPAREN[2];												// 0x7C  GPAREN0 - GPAREN1;
	uint32_t reserved10;											// 0x84  reserved
	uint32_t GPAFEN[2]; 											// 0x88  GPAFEN0 - GPAFEN1;
	uint32_t reserved11;											// 0x90  reserved
	uint32_t GPPUD; 												// 0x94  GPPUD 
	uint32_t GPPUDCLK[2]; 											// 0x98  GPPUDCLK0 - GPPUDCLK1;
};


int rpi_gpioHwGet (struct pinctrl_dev *pctldev, unsigned int pin);
int rpi_gpioHwSet (struct pinctrl_dev *pctldev, unsigned int pin, bool turnon);
int rpi_gpioHwSetMode (struct pinctrl_dev *pctldev, unsigned int pin, unsigned int mode);

#endif                          /* _RPI_GPIO_H_ */