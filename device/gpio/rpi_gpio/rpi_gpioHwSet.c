/**
 * @file rpi-gpioHwSet.c
 */
 /* Embedded Xinu, Copyright (C) 20018.  All rights reserved. */

#include <stdbool.h>
#include <stdint.h>
#include <xinu.h>
#include <gpio.h>
#include "rpi_gpio.h"


int rpi_gpioHwSet(struct pinctrl_dev *pctldev, unsigned int pin, bool turnon)
{
	if (pin < 54)
	{
		uint32_t value = 1 << (pin % 8);							// Bit mask for pin in a byte 
		if (pctldev->pin_valid[pin / 8] & value)					// Bit from bitmask is set in valid byte .. AKA port valid
		{
			uint32_t bit, regnum;
			volatile __attribute__((aligned(4))) uint32_t* p;
			volatile struct __attribute__((aligned(4))) GPIORegisters* GPIO;

			/* Create pointer to the actual physical hardware registers  */
			GPIO = (volatile struct GPIORegisters*)(pctldev->reg_data);

			regnum = pin / 32;										// Register number
			bit = 1 << (pin % 32);									// Create pin mask bit
			if (turnon) p = &GPIO->GPSET[regnum];					// turnon true means set output port
				else p = &GPIO->GPCLR[regnum];						// turnon false means clear output port
			*p = bit;												// Output bit	

			return 0;												// Return success
		}
	}
	return -1;														// Return fail	
}
