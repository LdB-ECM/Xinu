/**
 * @file rpi-gpioHwSetMode.c
 */
 /* Embedded Xinu, Copyright (C) 20018.  All rights reserved. */

#include <stdint.h>
#include <xinu.h>
#include <gpio.h>
#include "rpi_gpio.h"


int rpi_gpioHwSetMode (struct pinctrl_dev *pctldev, unsigned int pin, unsigned int mode)
{
	if ((pin < 54) && (mode < (GPIO_ALTFUNC3 + 1)))
	{
		uint8_t valid_mask = (1 << (pin % 8));						// Bit mask for pin in a byte 
		if (pctldev->pin_valid[pin / 8] & valid_mask)				// Bit from bitmask is set in valid byte .. AKA port valid
		{
			unsigned int shift_count;
			uint32_t value;
			volatile struct GPIORegisters* GPIO;

			/* Create pointer to the actual physical hardware registers  */
			GPIO = (volatile struct GPIORegisters*)(pctldev->reg_data);

			shift_count = ((pin % 10) * 3);							// Create shift count  (range 0 .. 27)
			value = GPIO->GPFSEL[pin / 10];							// Read control mode registe
			value &= ~(7 << shift_count);							// Clear GPIO mode bits for that port
			value |= (mode << shift_count);							// Logical OR GPIO mode bits onto result
			GPIO->GPFSEL[pin / 10] = value;							// Write value back to register

			return 0;												// Return success
		}
	}
	return -1;														// Return fail							
}
