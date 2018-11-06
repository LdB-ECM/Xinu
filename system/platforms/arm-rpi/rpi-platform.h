/**
* @file rpi-platform.h
*
* C definitions matching those provided by start.S
*
*/
#ifndef _RPI_PLATFORM_H_
#define _RPI_PLATFORM_H_

#include <stdint.h>			// Need for uint32_t

/***************************************************************************}
{					     PUBLIC ENUMERATION CONSTANTS			            }
****************************************************************************/

/*--------------------------------------------------------------------------}
;{	  ENUMERATED CODE TARGET ... WHICH ARM CPU THE CODE IS COMPILED FOR		}
;{-------------------------------------------------------------------------*/
typedef enum {
	ARM5_CODE = 5,										// ARM 5 CPU is targetted
	ARM6_CODE = 6,										// ARM 6 CPU is targetted
	ARM7_CODE = 7,										// ARM 7 CPU is targetted
	ARM8_CODE = 8,										// ARM 8 CPU is targetted
} ARM_CODE_TYPE;

/*--------------------------------------------------------------------------}
;{	 ENUMERATED AARCH TARGET ... WHICH AARCH TARGET CODE IS COMPILED FOR	}
;{-------------------------------------------------------------------------*/
typedef enum {
	AARCH32 = 0,										// AARCH32 - 32 bit
	AARCH64 = 1,										// AARCH64 - 64 bit
} AARCH_MODE;

/***************************************************************************}
{		 		    PUBLIC STRUCTURE DEFINITIONS				            }
****************************************************************************/

/*--------------------------------------------------------------------------}
{				 COMPILER TARGET SETTING STRUCTURE DEFINED					}
{--------------------------------------------------------------------------*/
typedef union
{
	struct
	{
		ARM_CODE_TYPE ArmCodeTarget : 4;							// @0    Compiler code target
		AARCH_MODE AArchMode : 1;									// @5    Code AARCH type compiler is producing
		unsigned CoresSupported : 3;								// @6    Cores the code is setup to support
		unsigned reserved : 23;										// @9-30 reserved
		unsigned HardFloats : 1;									// @31	 Compiler code for hard floats
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} CODE_TYPE;

/*--------------------------------------------------------------------------}
{						ARM CPU ID STRUCTURE DEFINED						}
{--------------------------------------------------------------------------*/
typedef union
{
	struct
	{
		unsigned Revision : 4;										// @0-3  CPU minor revision 
		unsigned PartNumber : 12;									// @4-15  Partnumber
		unsigned Architecture : 4;									// @16-19 Architecture
		unsigned Variant : 4;										// @20-23 Variant
		unsigned Implementer : 8;									// @24-31 reserved
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} CPU_ID;

/*--------------------------------------------------------------------------}
{				SMARTSTART VERSION STRUCTURE DEFINED						}
{--------------------------------------------------------------------------*/
typedef union
{
	struct
	{
		unsigned LoVersion : 16;									// @0-15  SmartStart minor version 
		unsigned HiVersion : 8;										// @16-23 SmartStart major version
		unsigned _reserved : 8;										// @24-31 reserved
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} SMARTSTART_VER;


/***************************************************************************}
{        PUBLIC INTERFACE DATA VARIABLES DEFINED AND SET IN start.S         }
{***************************************************************************/
extern uint32_t RPi_IO_Base_Addr;									// RPI IO base address auto-detected by SmartStartxx.S
extern uint32_t RPi_ARM_TO_GPU_Alias;								// RPI ARM to GPU alias address auto-detected by SmartStartxx.S
extern uint32_t RPi_BootAddr;										// RPI address processor booted from auto-detected by SmartStartxx.S
extern uint32_t RPi_CoresReady;										// RPI cpu cores made read for use by SmartStartxx.S
extern uint32_t RPi_CPUBootMode;									// RPI cpu mode it was in when it booted
extern CPU_ID RPi_CpuId;											// RPI CPU type auto-detected by SmartStartxx.S
extern CODE_TYPE RPi_CompileMode;									// RPI code type that compiler produced
extern uint32_t RPi_CPUCurrentMode;									// RPI cpu current operation mode
extern SMARTSTART_VER RPi_SmartStartVer;							// SmartStart version
#if (__ARM_FP == 12)
extern uint32_t get_fpexc(void);									// If HARD FLOATS returns fpexc register
extern uint32_t get_fpscr(void);									// If HARD FLOATS returns fpscr register
#endif

#endif