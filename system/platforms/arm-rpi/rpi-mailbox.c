#include <stdint.h>				// As all the registers are 32 it make sense to use uint32_t
#include <xinu.h>				// Needed for SYSERR an OK return values
#include "CriticalSection.h"	// Need access to stop interrupts for critical section
#include "rpi-platform.h"		// The generic pi platform header
#include "rpi-mailbox.h"		// This units header

/*--------------------------------------------------------------------------}
;{               RASPBERRY PI MAILBOX HARRDWARE REGISTERS					}
;{-------------------------------------------------------------------------*/
/*  The packing and alignment is required for 64 bit compiler not optional  }
{==========================================================================*/
struct __attribute__((__packed__, aligned(4))) MailBoxRegisters {
	const uint32_t Read0;											// 0x00         Read data from VC to ARM
	uint32_t Unused[3];												// 0x04-0x0F
	uint32_t Peek0;													// 0x10
	uint32_t Sender0;												// 0x14
	uint32_t Status0;												// 0x18         Status of VC to ARM
	uint32_t Config0;												// 0x1C        
	uint32_t Write1;												// 0x20         Write data from ARM to VC
	uint32_t Unused2[3];											// 0x24-0x2F
	uint32_t Peek1;													// 0x30
	uint32_t Sender1;												// 0x34
	uint32_t Status1;												// 0x38         Status of ARM to VC
	uint32_t Config1;												// 0x3C 
};

/*--------------------------------------------------------------------------}
;{				  RASPBERRY PI MAILBOX LOCATION								}
;{-------------------------------------------------------------------------*/
#define MAILBOX ((volatile __attribute__((aligned(4))) struct MailBoxRegisters*)(RPi_IO_Base_Addr + 0xB880))

/*==========================================================================}
{		           PUBLIC RASPBERRY PI MAILBOX ROUTINES 					}
{==========================================================================*/
#define MAIL_EMPTY	0x40000000		/* Mailbox Status Register: Mailbox Empty */
#define MAIL_FULL	0x80000000		/* Mailbox Status Register: Mailbox Full  */

/*-[rpi_mailbox_read]-------------------------------------------------------}
. This will read any pending data on the mailbox system on the given channel.
. RETURN: Read value if success, 0x1 for failure as low 4 bits will be zero.
.         in any actual valid value returning from mailbox.        
.--------------------------------------------------------------------------*/
uint32_t rpi_mailbox_read (MAILBOX_CHANNEL channel)
{
	uint32_t value;													// Temporary read value
	if (channel > MB_CHANNEL_GPU)  return 0x01;						// Usual bits 0-3 are zero for channel so we use to indicate error
	do {
		do {
			value = MAILBOX->Status0;								// Read mailbox0 status
		} while ((value & MAIL_EMPTY) != 0);						// Wait for data in mailbox
		value = MAILBOX->Read0;										// Read the mailbox	
	} while ((value & 0xF) != channel);								// Check response back is our channel
	value &= ~(0xF);												// Lower 4 low channel bits are not part of message
	return value;													// Return the value read
}

/*-[rpi_mailbox_write]------------------------------------------------------}
. This will execute the sending of the given data block message thru the
. mailbox system on the given channel.
. RETURN: 1 for success, 0 for failure.
.--------------------------------------------------------------------------*/
int rpi_mailbox_write (MAILBOX_CHANNEL channel, uint32_t message)
{
	uint32_t value;													// Temporary read value
	if (channel > MB_CHANNEL_GPU)  return 1;						// Invalid channel so return error
	message &= ~(0xF);												// Make sure 4 low channel bits are clear 
	message |= channel;												// OR the channel bits to the value							
	do {
		value = MAILBOX->Status1;									// Read mailbox1 status from GPU	
	} while ((value & MAIL_FULL) != 0);								// Make sure arm mailbox is not full
	MAILBOX->Write1 = message;										// Write value to mailbox
	return 0;														// Return success
}

/*-[rpi_MailBoxAccess]------------------------------------------------------}
. This will execute the sending of the given data block message thru the
. mailbox system on the given channel but disabling context switching.
. This is required because when a PI mailbox message is posted a response
. must be read before any chance of another PI mailbox message sent.
. RETURN: OK for success, SYSERR for failure.
.--------------------------------------------------------------------------*/
int rpi_MailBoxAccess (uint32_t channel, uint32_t msg)
{
	int status = SYSERR;											// Preset status as system error
	ENTER_KERNEL_CRITICAL_SECTION();								// CRITICAL SECTION START no context switch allowed
	if ( (rpi_mailbox_write(channel, msg) & 0xF) == 0x0) {			// We know write worked if low 4 bits are zero
		uint32_t result = rpi_mailbox_read(channel);				// Read the status back
		if ( (result & 0xF) == 0x0) status = OK;					// We know read worked if low 4 bits are zero
	}
	EXIT_KERNEL_CRITICAL_SECTION();									// END OF CRITICAL SECTION
	return status;													// Return fail or success of call
}