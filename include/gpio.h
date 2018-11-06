/**
 * @file gpio.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdbool.h>
#include <stdint.h>
#include <xinu.h>
#include <device.h>
#include <semaphore.h>


/*--------------------------------------------------------------------------}
{		      XINU GPIO PIN CONTOL DEVICE CONTROL STRUCTURE				    }
{--------------------------------------------------------------------------*/
struct pinctrl_dev 
{
	volatile uintptr_t reg_data;						// Pointer address to register data for this group (GCC 7.2.1 bug .. needs volatile .. NFI why )
	unsigned char pin_valid[8];							// 64 bits that are set for each pin 0..63 that is valid												
	const char *name;									// Pointer to name of this group of IO .. usually GPIO0, GPIO1 etc
};

/*--------------------------------------------------------------------------}
{		  	XINU GPIO PIN CONTROLLER DEVICE FUNCTION TABLE				    }
{--------------------------------------------------------------------------*/
struct gpio_controller_function 
{
	int(*pin_initialize) (struct pinctrl_dev *pctldev, void* config);
	int (*pin_value_get) (struct pinctrl_dev *pctldev, unsigned int pin);
	int (*pin_value_set) (struct pinctrl_dev *pctldev, unsigned int pin, bool turn_on);
	int (*pin_mode_set)  (struct pinctrl_dev *pctldev, unsigned int pin, unsigned int mode);
};



#define GPIO_DIR_IN        0x00				/**< set pin to input mode				*/
#define GPIO_DIR_OUT       0x01				/**< set pin to output mode				*/

/* GPIO bit flags for pins */
#define GPIO0              0x01
#define GPIO1              0x02
#define GPIO2              0x04
#define GPIO3              0x08
#define GPIO4              0x10
#define GPIO5              0x20
#define GPIO6              0x40
#define GPIO7              0x80

#define GPIO_PIN_COUNT     8

/* Buttons */
#define GPIO_BUT_CISCO    GPIO4 /**< Front Cisco button                 */
#define GPIO_BUT_RESET    GPIO6 /**< Back reset button                  */

/* LEDs */
#define GPIO_LED_WLAN     GPIO0 /**< WLAN LED                           */
#define GPIO_LED_POWER    GPIO1 /**< Power LED (hardware controlled)    */
#define GPIO_LED_CISCOWHT GPIO2 /**< White Cisco LED                    */
#define GPIO_LED_CISCOONG GPIO3 /**< Orange Cisco LED                   */
#define GPIO_LED_DMZ      GPIO7 /**< DMZ LED                            */

/**
 * Control and status registers for the GPIO.
 */
struct gpio_csreg
{
    volatile unsigned long input;       /**< input                              */
    volatile unsigned long output;      /**< output                             */
    volatile unsigned long enable;      /**< direction                          */
    volatile unsigned long control;     /**< usage unkmown                      */
};

/* LED control functions */

/**
 * Turns an LED on
 * @param n GPIO bit for LED (use GPIO_LED_* constants)
 */
#define gpioLEDOn(n) ((struct gpio_csreg *)GPIO_BASE)->enable |= (n); \
							((struct gpio_csreg *)GPIO_BASE)->output &= ~(n)

/**
 * Turns an LED off
 * @param n GPIO bit for LED (use GPIO_LED_* constants)
 */
#define gpioLEDOff(n) ((struct gpio_csreg *)GPIO_BASE)->enable &= ~(n)


 /**
  * GPIO structure
  */
struct gpio_device
{
	/* Pointers to associated structures */
	device *dev;								/**< Dev structure that owns this GPIO	*/

	/* Internal GPIO control records */
	struct pinctrl_dev ctrl;					/**< Internal GPIO control block record	*/
	struct gpio_controller_function func;		/**< Internal GPIO function pointers	*/


	semaphore iosema;							/**< GPIO access semaphore         */
};

extern struct gpio_device gpiotab[NGPIO];


#endif                          /* _GPIO_H_ */
