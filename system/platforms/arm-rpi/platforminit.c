/**
 * @file platforminit.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <platform.h>
#include <string.h>
#include <framebuffer.h>
#include <usbkbd.h>
#include <stdio.h>
#include "rpi-platform.h"
#include "rpi-mailbox.h"


/**
 * Powers on the DWC hardware.
 */
xinu_syscall usbpoweron(void)
{
	int retval;
	uint32_t  __attribute__((aligned(16))) msg[8] =
	{
		sizeof(msg),					// Message size
		0,								// Response will go here
		MAILBOX_TAG_SET_POWER_STATE,	// Power set command
		8,								// 8 byte request
		8,								// 8 byte response
		MB_POWER_USBHCD,				// Power ID to set
		1,								// Power on
		0								// Tag end marker
	};
	retval = rpi_MailBoxAccess(8, (uint32_t)&msg[0]);
	return (retval == OK) ? USB_STATUS_SUCCESS : USB_STATUS_HARDWARE_ERROR;
}

xinu_syscall usbpoweroff(void)
{
	uint32_t  __attribute__((aligned(16))) msg[8] =
	{
		sizeof(msg),					// Message size
		0,								// Response will go here
		MAILBOX_TAG_SET_POWER_STATE,	// Power set command
		8,								// 8 byte request
		8,								// 8 byte response
		MB_POWER_USBHCD,				// Power ID to set
		0,								// Power off
		0								// Tag end marker
	};
	return (OK == rpi_MailBoxAccess(8, (uint32_t)&msg[0])) ? OK : SYSERR;
}


/* Internal routine to platform.init to get last ARM memory address */
static uint32_t RPi_LastARMAddr(void)
{
	uint32_t  __attribute__((aligned(16))) msg[8] =
	{
		sizeof(msg),					// Message size
		0,								// Response will go here
		MAILBOX_TAG_GET_ARM_MEMORY,		// Get ARM memory tag
		8,								// 8 byte request
		8,								// 8 byte response
		0,								// Zero base response
		0,								// Zero size response
		0								// Tag end marker
	};

	if (rpi_MailBoxAccess(MB_CHANNEL_TAGS, (uint32_t)&msg[0]) == OK)// Get the ARM memory base and size from PI mailbox
	{
		return (msg[5] + msg[6]);									// Size is base + size ... base will be zero anyhow
	}
	return 0;														// Some error occured
}

/* Internal routine to platform.init to get board model */
static uint32_t RPi_getModel (void)
{
	uint32_t  __attribute__((aligned(16))) msg[7] =
	{
		sizeof(msg),					// Message size
		0,								// Response will go here
		MAILBOX_TAG_GET_BOARD_REVISION,	// Get Pi board model
		4,								// 4 byte request
		4,								// 4 byte response
		0,								// Model response
		0,								// Tag end marker
	};

	if (rpi_MailBoxAccess(MB_CHANNEL_TAGS, (uint32_t)&msg[0]) == OK)// Get the ARM memory base and size from PI mailbox
	{
		return (msg[5]);											// Return model id bits
	}
	return 0;														// Some error occured
}



/* DEVICES WE KNOW HOW TO INSTALL */
xinu_devcall ns16550_Install(unsigned int DevTabNum, const char* devname, unsigned int uartnum, uintptr_t addr, int(*mailbox) (uint32_t channel, uint32_t msg));
xinu_devcall usbKbd_Install (unsigned int DevTabNum, const char* devname, unsigned int kbdnum);
xinu_devcall framebuffer_Install (unsigned int DevTabNum, const char* devname, unsigned int fbNum, int(*rpi_mailbox) (uint32_t channel, uint32_t msg));
xinu_devcall null_Install (unsigned int DevTabNum, const char* devname);
xinu_devcall loopback_Install (unsigned int DevTabNum, const char* devname, unsigned int loopNum);
xinu_devcall tty_Install (unsigned int DevTabNum, const char* devname, unsigned int ttyNum);
xinu_devcall smsc9512_Install (unsigned int DevTabNum, const char* devname, unsigned int ethNum);
xinu_devcall lan78xx_Install (unsigned int DevTabNum, const char* devname, unsigned int ethNum);
xinu_devcall ethloop_Install (unsigned int DevTabNum, const char* devname, unsigned int eloopNum);
xinu_devcall raw_Install (unsigned int DevTabNum, const char* devname, unsigned int rawNum);
xinu_devcall udp_Install (unsigned int DevTabNum, const char* devname, unsigned int udpNum);
xinu_devcall tcp_Install (unsigned int DevTabNum, const char* devname, unsigned int tcpNum);
xinu_devcall telnet_Install (unsigned int DevTabNum, const char* devname, unsigned int telnetNum);
xinu_devcall pl011_Install (unsigned int DevTabNum, const char* devname, unsigned int uartnum, uintptr_t addr);

#include <gpio.h>
xinu_devcall rpi_gpio_Install(const char* gpiodevname, unsigned int gpioctrlnum, uintptr_t addr);


const char* model_names[14] = { [MODEL_1A] = "Pi1 ModelA", [MODEL_1B] = "Pi1 ModelB",
							    [MODEL_1A_PLUS] = "Pi1 ModelA+", [MODEL_1B_PLUS] = "Pi1 ModelB+",
							   [MODEL_2B] = "Pi2 ModelB", [MODEL_ALPHA] = "Model Alpha",
							   [MODEL_CM] = "Compute Module", [MODEL_2A] = "Pi2 ModelA",
							   [MODEL_PI3] = "Pi3 Standard", [MODEL_PI_ZERO] = "Pi Zero",
							   [MODEL_CM3] = "Compute Module 3", [MODEL_PI_ZEROW] = "Pi Zero W",
							   [MODEL_PI3B_PLUS] = "Pi3 Model B+", };


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


/**
 * Initializes platform specific information for the Raspberry Pi hardware.
 * @return OK
 */
int platforminit(void)
{
    strlcpy(platform.family, "BCM2835", PLT_STRMAX);
    strlcpy(platform.name, "Raspberry Pi", PLT_STRMAX);
	platform.minaddr = 0;
	platform.maxaddr = RPi_LastARMAddr();					// Get last memory address valid on the ARM from mailbox
    platform.clkfreq = 1000000;
    platform.serial_low = 0;   /* Used only if serial # not found in atags */
    platform.serial_high = 0;  /* Used only if serial # not found in atags */

	uint32_t  __attribute__((aligned(16))) msg[8] =
	{
		sizeof(msg),					// Message size
		0,								// Response will go here
		MAILBOX_TAG_GET_MAX_CLOCK_RATE,	// Get ARM max speed
		8,								// 8 byte request
		8,								// 8 byte response
		3,								// ARM CLOCK
		0,								// Clock speed
		0								// Tag end marker
	};
	if (rpi_MailBoxAccess(MB_CHANNEL_TAGS, (uint32_t)&msg[0]) == OK)
	{
		msg[1] = 0;
		msg[2] = MAILBOX_TAG_SET_CLOCK_RATE;
		msg[3] = 8;
		msg[4] = 8;
		msg[5] = 3;
		if (rpi_MailBoxAccess(MB_CHANNEL_TAGS, (uint32_t)&msg[0]) == OK)
		{
			/* ARM speed taken to max */
		}
	}


	// For now install the Pi GPIO block .. it isn't in device table yet but gives us GPIO functions
	rpi_gpio_Install("GPIO0", 0, RPi_IO_Base_Addr + 0x00200000);


	RPI_MODEL_ID id;
	id.Raw32 = RPi_getModel();
	platform.model_id = id.model;
	if (id.model < 14) strlcpy(platform.family, model_names[id.model], PLT_STRMAX);
	switch (id.model)
	{
		/* These models the miniUART is console out*/
		case MODEL_1A:
		case MODEL_1B:
		case MODEL_1A_PLUS:
		case MODEL_1B_PLUS:

		case MODEL_PI_ZEROW:
			gpiotab[0].func.pin_mode_set(&gpiotab[0].ctrl, 14, GPIO_ALTFUNC5);
			gpiotab[0].func.pin_mode_set(&gpiotab[0].ctrl, 15, GPIO_ALTFUNC5);
			ns16550_Install(0, "SERIAL0", 0, RPi_IO_Base_Addr + 0x00215040, rpi_MailBoxAccess);
			pl011_Install(27, "SERIAL1", 1, RPi_IO_Base_Addr + 0x00201000);
			break;

		/* These models the PL011 is console out*/
		case MODEL_PI3B_PLUS:
		case MODEL_2B:
		case MODEL_ALPHA:
		case MODEL_CM:
		case MODEL_PI3:
		case MODEL_PI_ZERO:
			gpiotab[0].func.pin_mode_set(&gpiotab[0].ctrl, 14, GPIO_ALTFUNC0);
			gpiotab[0].func.pin_mode_set(&gpiotab[0].ctrl, 15, GPIO_ALTFUNC0);
			pl011_Install(0, "SERIAL0", 0, RPi_IO_Base_Addr + 0x00201000);
			ns16550_Install(27, "SERIAL1", 1, RPi_IO_Base_Addr + 0x00215040, rpi_MailBoxAccess);
			break;

		/* If we don't know model guess miniUART is console out*/
		default:
			gpiotab[0].func.pin_mode_set(&gpiotab[0].ctrl, 14, GPIO_ALTFUNC5);
			gpiotab[0].func.pin_mode_set(&gpiotab[0].ctrl, 15, GPIO_ALTFUNC5);
			ns16550_Install(0, "SERIAL0", 0, RPi_IO_Base_Addr + 0x00215040, rpi_MailBoxAccess);
			pl011_Install(27, "SERIAL1", 1, RPi_IO_Base_Addr + 0x00201000);
	}

	usbKbd_Install(1, "USBKBD0", 0);

	const char* montext = "KBDMON0";
	devtab[2].num = 2;
	devtab[2].name = (char*)&montext;
	devtab[2].read = usbKbdRead;
	devtab[2].write = fbWrite;
	devtab[2].getc = usbKbdGetc;
	devtab[2].putc = fbPutc;

	framebuffer_Install(3, "FRAMEBUF", 0, rpi_MailBoxAccess);
	   	

	null_Install(4, "DEVNULL");
	loopback_Install(5, "LOOP0", 0);
	tty_Install(6, "CONSOLE", 0);
	tty_Install(7, "TTY1", 1);
	tty_Install(8, "TTYLOOP", 2);

	/* None of these cut down models have ethernet so ignore installing */
	if ((id.model != MODEL_PI_ZERO) &&
		(id.model != MODEL_PI_ZEROW) &&
		(id.model != MODEL_CM) &&
		(id.model != MODEL_CM3))
	{
		platform.EtherCount = 1;
		if (id.model == MODEL_PI3B_PLUS)
			lan78xx_Install(9, "ETH0", 0);
		else
			smsc9512_Install(9, "ETH0", 0);

		ethloop_Install(10, "ELOOP", 0);
		raw_Install(11, "RAW0", 0);
		raw_Install(12, "RAW1", 1);
		udp_Install(13, "UDP0", 0);
		udp_Install(14, "UDP1", 1);
		udp_Install(15, "UDP2", 2);
		udp_Install(16, "UDP3", 3);
		tcp_Install(17, "TCP0", 0);
		tcp_Install(18, "TCP1", 1);
		tcp_Install(19, "TCP2", 2);
		tcp_Install(20, "TCP3", 3);
		tcp_Install(21, "TCP4", 4);
		tcp_Install(22, "TCP5", 5);
		tcp_Install(23, "TCP6", 6);
		telnet_Install(24, "TELNET0", 0);
		telnet_Install(25, "TELNET1", 1);
		telnet_Install(26, "TELNET2", 2);
	}
	else platform.EtherCount = 0;

	sprintf(&platform.details[0], "SmartStart v%x.%x%x, ARM%d AARCH%d code, CPU: %#03X, Cores: %u FPU: %s",
		(unsigned int)(RPi_SmartStartVer.HiVersion), (unsigned int)(RPi_SmartStartVer.LoVersion >> 8), (unsigned int)(RPi_SmartStartVer.LoVersion & 0xFF),
		(unsigned int)RPi_CompileMode.ArmCodeTarget, (unsigned int)RPi_CompileMode.AArchMode * 32 + 32,
		(unsigned int)RPi_CpuId.PartNumber, (unsigned int)RPi_CoresReady, (RPi_CompileMode.HardFloats == 1) ? "HARD" : "SOFT");

	return OK;
}
