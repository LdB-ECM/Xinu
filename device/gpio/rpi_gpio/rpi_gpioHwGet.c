/**
 * @file rpi_gpioHwGet.c
 */
 /* Embedded Xinu, Copyright (C) 20018.  All rights reserved. */

#include <stdbool.h>
#include <stdint.h>
#include <xinu.h>
#include <gpio.h>
#include "rpi_gpio.h"


int rpi_gpioHwGet (struct pinctrl_dev *pctldev, unsigned int pin)
{
	if (pin < 54)
	{
		uint32_t value = 1 << (pin % 8);							// Bit mask for pin in a byte 
		if (pctldev->pin_valid[pin / 8] & value)					// Bit from bitmask is set in valid byte .. AKA port valid
		{
			uint32_t bit;
			volatile struct __attribute__((aligned(4))) GPIORegisters* GPIO;

			/* Create pointer to the actual physical hardware registers  */
			GPIO = (volatile struct GPIORegisters*)(pctldev->reg_data);

			bit = 1 << (pin % 32);									// Create pin mask bit
			value = GPIO->GPLEV[pin / 32];							// Read port level
			return  (value & bit) ? true : false;					// Return true if bit set, false if bit not set
		}
	}
	return -1;														// Return fail	
}

