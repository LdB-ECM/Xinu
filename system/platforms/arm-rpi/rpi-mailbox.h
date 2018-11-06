/**
* @file rpi-mailbox.h
*
* Definitions specific to the mailbox used in the Raspberry Pi 1,2, 3.
*
* The mailbox functions are detailed here
* https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface
*/

#ifndef _RPI_MAILBOX_H_
#define _RPI_MAILBOX_H_

#include <stdint.h>		// As all the registers are 32 it make sense to use uint32_t

/*--------------------------------------------------------------------------}
{					    ENUMERATED MAILBOX CLOCK ID							}
{		  https://github.com/raspberrypi/firmware/wiki/Mailboxes			}
{--------------------------------------------------------------------------*/
typedef enum {
	CLK_EMMC_ID		= 0x1,									// Mailbox Tag Channel EMMC clock ID 
	CLK_UART_ID		= 0x2,									// Mailbox Tag Channel uart clock ID
	CLK_ARM_ID		= 0x3,									// Mailbox Tag Channel ARM clock ID
	CLK_CORE_ID		= 0x4,									// Mailbox Tag Channel SOC core clock ID
	CLK_V3D_ID		= 0x5,									// Mailbox Tag Channel V3D clock ID
	CLK_H264_ID		= 0x6,									// Mailbox Tag Channel H264 clock ID
	CLK_ISP_ID		= 0x7,									// Mailbox Tag Channel ISP clock ID
	CLK_SDRAM_ID	= 0x8,									// Mailbox Tag Channel SDRAM clock ID
	CLK_PIXEL_ID	= 0x9,									// Mailbox Tag Channel PIXEL clock ID
	CLK_PWM_ID		= 0xA,									// Mailbox Tag Channel PWM clock ID
} MB_CLOCK_ID;

/*--------------------------------------------------------------------------}
{	                  ENUMERATED MAILBOX CHANNELS							}
{		  https://github.com/raspberrypi/firmware/wiki/Mailboxes			}
{--------------------------------------------------------------------------*/
typedef enum {
	MB_CHANNEL_POWER	= 0x0,								// Mailbox Channel 0: Power Management Interface 
	MB_CHANNEL_FB		= 0x1,								// Mailbox Channel 1: Frame Buffer
	MB_CHANNEL_VUART	= 0x2,								// Mailbox Channel 2: Virtual UART
	MB_CHANNEL_VCHIQ	= 0x3,								// Mailbox Channel 3: VCHIQ Interface
	MB_CHANNEL_LEDS		= 0x4,								// Mailbox Channel 4: LEDs Interface
	MB_CHANNEL_BUTTONS	= 0x5,								// Mailbox Channel 5: Buttons Interface
	MB_CHANNEL_TOUCH	= 0x6,								// Mailbox Channel 6: Touchscreen Interface
	MB_CHANNEL_COUNT	= 0x7,								// Mailbox Channel 7: Counter
	MB_CHANNEL_TAGS		= 0x8,								// Mailbox Channel 8: Tags (ARM to VC)
	MB_CHANNEL_GPU		= 0x9,								// Mailbox Channel 9: GPU (VC to ARM)
} MAILBOX_CHANNEL;


/*--------------------------------------------------------------------------}
{	                 ENUMERATED MAILBOX POWER ID							}
{  https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface	}
{--------------------------------------------------------------------------*/
typedef enum {
	MB_POWER_SDCARD	= 0x0,									// SD CARD power management id 
	MB_POWER_UART0	= 0x1,									// UART0 power management id 
	MB_POWER_UART1	= 0x2,									// UART1 power management id
	MB_POWER_USBHCD = 0x3,									// USB HCD power management id
	MB_POWER_I2C0	= 0x4,									// I2C channel 0 power management id
	MB_POWER_I2C1	= 0x5,									// I2C channel 1 power management id
	MB_POWER_I2C2	= 0x6,									// I2C channel 2 power management id
	MB_POWER_SPI	= 0x7,									// SPI power management id
	MB_POWER_CCP2TX = 0x8,									// CCP2TX power management id
} MAILBOX_POWER_ID;

/*--------------------------------------------------------------------------}
{	            ENUMERATED MAILBOX TAG CHANNEL COMMANDS						}
{  https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface  }
{--------------------------------------------------------------------------*/
typedef enum {
	/* Videocore info commands */
	MAILBOX_TAG_GET_VERSION					= 0x00000001,	// Get firmware revision

	/* Hardware info commands */
	MAILBOX_TAG_GET_BOARD_MODEL				= 0x00010001,	// Get board model
	MAILBOX_TAG_GET_BOARD_REVISION			= 0x00010002,	// Get board revision
	MAILBOX_TAG_GET_BOARD_MAC_ADDRESS		= 0x00010003,	// Get board MAC address
	MAILBOX_TAG_GET_BOARD_SERIAL			= 0x00010004,	// Get board serial
	MAILBOX_TAG_GET_ARM_MEMORY				= 0x00010005,	// Get ARM memory
	MAILBOX_TAG_GET_VC_MEMORY				= 0x00010006,	// Get VC memory
	MAILBOX_TAG_GET_CLOCKS					= 0x00010007,	// Get clocks

	/* Power commands */
	MAILBOX_TAG_GET_POWER_STATE				= 0x00020001,	// Get power state
	MAILBOX_TAG_GET_TIMING					= 0x00020002,	// Get timing
	MAILBOX_TAG_SET_POWER_STATE				= 0x00028001,	// Set power state

	/* GPIO commands */
	MAILBOX_TAG_GET_GET_GPIO_STATE			= 0x00030041,	// Get GPIO state
	MAILBOX_TAG_SET_GPIO_STATE				= 0x00038041,	// Set GPIO state

	/* Clock commands */
	MAILBOX_TAG_GET_CLOCK_STATE				= 0x00030001,	// Get clock state
	MAILBOX_TAG_GET_CLOCK_RATE				= 0x00030002,	// Get clock rate
	MAILBOX_TAG_GET_MAX_CLOCK_RATE			= 0x00030004,	// Get max clock rate
	MAILBOX_TAG_GET_MIN_CLOCK_RATE			= 0x00030007,	// Get min clock rate
	MAILBOX_TAG_GET_TURBO					= 0x00030009,	// Get turbo

	MAILBOX_TAG_SET_CLOCK_STATE				= 0x00038001,	// Set clock state
	MAILBOX_TAG_SET_CLOCK_RATE				= 0x00038002,	// Set clock rate
	MAILBOX_TAG_SET_TURBO					= 0x00038009,	// Set turbo

	/* Voltage commands */
	MAILBOX_TAG_GET_VOLTAGE					= 0x00030003,	// Get voltage
	MAILBOX_TAG_GET_MAX_VOLTAGE				= 0x00030005,	// Get max voltage
	MAILBOX_TAG_GET_MIN_VOLTAGE				= 0x00030008,	// Get min voltage

	MAILBOX_TAG_SET_VOLTAGE					= 0x00038003,	// Set voltage

	/* Temperature commands */
	MAILBOX_TAG_GET_TEMPERATURE				= 0x00030006,	// Get temperature
	MAILBOX_TAG_GET_MAX_TEMPERATURE			= 0x0003000A,	// Get max temperature

	/* Memory commands */
	MAILBOX_TAG_ALLOCATE_MEMORY				= 0x0003000C,	// Allocate Memory
	MAILBOX_TAG_LOCK_MEMORY					= 0x0003000D,	// Lock memory
	MAILBOX_TAG_UNLOCK_MEMORY				= 0x0003000E,	// Unlock memory
	MAILBOX_TAG_RELEASE_MEMORY				= 0x0003000F,	// Release Memory
																	
	/* Execute code commands */
	MAILBOX_TAG_EXECUTE_CODE				= 0x00030010,	// Execute code

	/* QPU control commands */
	MAILBOX_TAG_EXECUTE_QPU					= 0x00030011,	// Execute code on QPU
	MAILBOX_TAG_ENABLE_QPU					= 0x00030012,	// QPU enable

	/* Displaymax commands */
	MAILBOX_TAG_GET_DISPMANX_HANDLE			= 0x00030014,	// Get displaymax handle
	MAILBOX_TAG_GET_EDID_BLOCK				= 0x00030020,	// Get HDMI EDID block

	/* SD Card commands */
	MAILBOX_GET_SDHOST_CLOCK				= 0x00030042,	// Get SD Card EMCC clock
	MAILBOX_SET_SDHOST_CLOCK				= 0x00038042,	// Set SD Card EMCC clock

	/* Framebuffer commands */
	MAILBOX_TAG_ALLOCATE_FRAMEBUFFER		= 0x00040001,	// Allocate Framebuffer address
	MAILBOX_TAG_BLANK_SCREEN				= 0x00040002,	// Blank screen
	MAILBOX_TAG_GET_PHYSICAL_WIDTH_HEIGHT	= 0x00040003,	// Get physical screen width/height
	MAILBOX_TAG_GET_VIRTUAL_WIDTH_HEIGHT	= 0x00040004,	// Get virtual screen width/height
	MAILBOX_TAG_GET_COLOUR_DEPTH			= 0x00040005,	// Get screen colour depth
	MAILBOX_TAG_GET_PIXEL_ORDER				= 0x00040006,	// Get screen pixel order
	MAILBOX_TAG_GET_ALPHA_MODE				= 0x00040007,	// Get screen alpha mode
	MAILBOX_TAG_GET_PITCH					= 0x00040008,	// Get screen line to line pitch
	MAILBOX_TAG_GET_VIRTUAL_OFFSET			= 0x00040009,	// Get screen virtual offset
	MAILBOX_TAG_GET_OVERSCAN				= 0x0004000A,	// Get screen overscan value
	MAILBOX_TAG_GET_PALETTE					= 0x0004000B,	// Get screen palette

	MAILBOX_TAG_RELEASE_FRAMEBUFFER			= 0x00048001,	// Release Framebuffer address
	MAILBOX_TAG_SET_PHYSICAL_WIDTH_HEIGHT	= 0x00048003,	// Set physical screen width/heigh
	MAILBOX_TAG_SET_VIRTUAL_WIDTH_HEIGHT	= 0x00048004,	// Set virtual screen width/height
	MAILBOX_TAG_SET_COLOUR_DEPTH			= 0x00048005,	// Set screen colour depth
	MAILBOX_TAG_SET_PIXEL_ORDER				= 0x00048006,	// Set screen pixel order
	MAILBOX_TAG_SET_ALPHA_MODE				= 0x00048007,	// Set screen alpha mode
	MAILBOX_TAG_SET_VIRTUAL_OFFSET			= 0x00048009,	// Set screen virtual offset
	MAILBOX_TAG_SET_OVERSCAN				= 0x0004800A,	// Set screen overscan value
	MAILBOX_TAG_SET_PALETTE					= 0x0004800B,	// Set screen palette
	MAILBOX_TAG_SET_VSYNC					= 0x0004800E,	// Set screen VSync
	MAILBOX_TAG_SET_BACKLIGHT				= 0x0004800F,	// Set screen backlight

	/* VCHIQ commands */
	MAILBOX_TAG_VCHIQ_INIT					= 0x00048010,	// Enable VCHIQ

	/* Config commands */
	MAILBOX_TAG_GET_COMMAND_LINE			= 0x00050001,	// Get command line 

	/* Shared resource management commands */
	MAILBOX_TAG_GET_DMA_CHANNELS			= 0x00060001,	// Get DMA channels

	/* Cursor commands */
	MAILBOX_TAG_SET_CURSOR_INFO				= 0x00008010,	// Set cursor info
	MAILBOX_TAG_SET_CURSOR_STATE			= 0x00008011,	// Set cursor state
} TAG_CHANNEL_COMMAND;

/***************************************************************************}
{		 		    PUBLIC STRUCTURE DEFINITIONS				            }
****************************************************************************/

typedef enum {
	MODEL_1A		= 0,
	MODEL_1B		= 1,
	MODEL_1A_PLUS	= 2,
	MODEL_1B_PLUS	= 3,
	MODEL_2B		= 4,
	MODEL_ALPHA		= 5,
	MODEL_CM		= 6,		// Compute Module
	MODEL_2A        = 7,
	MODEL_PI3		= 8,
	MODEL_PI_ZERO	= 9,
	MODEL_CM3       = 0xA,      // Compute module 3
	MODEL_PI_ZEROW	= 0xC,
	MODEL_PI3B_PLUS = 0xD,
} RPI_BOARD_TYPE;

#define BCM2835 0
#define BCM2836 1
#define BCM2837 2

/*--------------------------------------------------------------------------}
{			PI MODEL STRUCTURE RETURNED FROM TAG 0x00010002					}
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__)) 
	{
		unsigned revision : 4;										// @0-3		Board revison
		unsigned model: 8;											// @4-12	Model
		unsigned BCM28xx : 4;										// @13-15	CPU
		unsigned Manufacturer : 4;									// @16-19	Manufacturer ID
		unsigned MemorySize : 3;									// @20-22	Memory size on board
		unsigned Revision1 : 1;										// @23		Revison 1 if set, revision 0 if zero
		unsigned _reserved : 8;										// @24-31	reserved for future use
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} RPI_MODEL_ID;

/*==========================================================================}
{		           PUBLIC RASPBERRY PI MAILBOX ROUTINES 					}
{==========================================================================*/

/*-[rpi_MailBoxAccess]------------------------------------------------------}
. This will execute the sending of the given data block message thru the
. mailbox system on the given channel but disabling context switching.
. This is required because when a PI mailbox message is posted a response
. must be read before any chance of another PI mailbox message sent.
. RETURN: OK for success, SYSERR for failure.
.--------------------------------------------------------------------------*/
int rpi_MailBoxAccess (uint32_t channel, uint32_t msg);

#endif