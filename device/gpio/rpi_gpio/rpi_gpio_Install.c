/**
* @file rpi_gpio_Install.c
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <stdint.h>
#include <xinu.h>
#include <gpio.h>
#include <conf.h>
#include "rpi_gpio.h"

xinu_devcall rpi_gpio_Install (const char* gpiodevname, unsigned int gpioctrlnum, uintptr_t addr)
{

	/* Set the actual data on the gpio tab */
	struct pinctrl_dev *pinctrl;
	pinctrl = &gpiotab[gpioctrlnum].ctrl;			// Pointer to gpio pin control device entry
	pinctrl->reg_data = addr;						// Hold the address to GPIO registers
	pinctrl->name = gpiodevname;					// Hold the device name

	/* Set the 64 bit masks marking valid GPIO pins */
	pinctrl->pin_valid[0] = 0b11111100;				// Valid GPIO ports 2,3,4,5,6,7
	pinctrl->pin_valid[1] = 0b11111111;				// Valid GPIO ports 8,9,10,11,12,13,14,15
	pinctrl->pin_valid[2] = 0b11111111;				// Valid GPIO ports 16,17,18,19,20,21,22,23
	pinctrl->pin_valid[3] = 0b00001111;				// Valid GPIO ports 24,25,26,27 
	pinctrl->pin_valid[4] = 0b00000000;
	pinctrl->pin_valid[5] = 0b00000000;
	pinctrl->pin_valid[6] = 0b00000000;
	pinctrl->pin_valid[7] = 0b00000000;

	gpiotab[gpioctrlnum].func.pin_value_get = rpi_gpioHwGet;	// Read pin value function ptr set
	gpiotab[gpioctrlnum].func.pin_value_set = rpi_gpioHwSet;	// Write value to pin function ptr set
	gpiotab[gpioctrlnum].func.pin_mode_set = rpi_gpioHwSetMode;	// Pin mode setup function ptr set

	return gpioctrlnum;
}