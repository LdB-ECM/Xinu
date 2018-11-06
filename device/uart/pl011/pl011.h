/**
 * @file pl011.h
 */

/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#ifndef _PL011_H_
#define _PL011_H_

#include <stdint.h>
#include <xinu.h>

/*--------------------------------------------------------------------------}
{     PL011 UART DATA Register BCM2835 ARM Peripheral manual page 179/180	}
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned DATA : 8;											// @0-7 Transmit Read/write data
		unsigned FE : 1;											// @8	Framing error
		unsigned PE : 1;											// @9	Parity error
		unsigned BE : 1;											// @10	Break error
		unsigned OE : 1;											// @11	Overrun error
		unsigned _reserved : 20;									// @12-31 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} pl011_data_reg_t;

/*--------------------------------------------------------------------------}
{     PL011 UART FR Register BCM2835 ARM Peripheral manual page 181/182	    }
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned CTS : 1;											// @0	  Clear to send. This bit is the complement of the UART clear to send, nUARTCTS
		unsigned DSR : 1;											// @1	  Unsupported, write zero, read as don't care 
		unsigned DCD : 1;											// @2	  Unsupported, write zero, read as don't care  
		unsigned BUSY : 1;											// @3	  UART busy. If this bit is set to 1, the UART is busy transmitting data.
		unsigned RXFE : 1;											// @4	  Receive FIFO empty. The meaning of this bit depends on the state of the FEN bit
		unsigned TXFF : 1;											// @5	  Transmit FIFO full. The meaning of this bit depends on the state of the FEN bit
		unsigned RXFF : 1;											// @6	  Receive FIFO full. The meaning of this bit depends on the state of the FEN bit 
		unsigned TXFE : 1;											// @7	  Transmit FIFO empty. The meaning of this bit depends on the state of the FEN bit 
		unsigned _reserved : 24;									// @8-31  Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} pl011_fr_reg_t;

/*--------------------------------------------------------------------------}
{      PL011 UART IBRD Register BCM2835 ARM Peripheral manual page 183	    }
{                 IBRD = Integer Baud Rate Divisor                          }
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned DIVISOR : 16;										// @0-15 Integer baud rate divisor
		unsigned _reserved : 16;									// @12-31 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} pl011_ibrd_reg_t;

/*--------------------------------------------------------------------------}
{      PL011 UART FBRD Register BCM2835 ARM Peripheral manual page 184	    }
{                 FBRD = Fractional Baud Rate Divsor                        }
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned DIVISOR : 6;										// @0-5	  Factional baud rate divisor
		unsigned _reserved : 26;									// @6-31  Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} pl011_fbrd_reg_t;

/*--------------------------------------------------------------------------}
{    PL011 UART LRCH Register BCM2835 ARM Peripheral manual page 184/185	}
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned BRK : 1;											// @0	  Send break. If this bit is set to 1, a low-level is continually output on the TXD output
		unsigned PEN : 1;											// @1	  Parity enable, 0 = parity is disabled, 1 = parity via bit 2. 
		unsigned EPS : 1;											// @2	  If PEN = 1 then 0 = odd parity, 1 = even parity
		unsigned STP2 : 1;											// @3	  Two stops = 1, 1 stop bit = 0
		unsigned FEN : 1;											// @4	  FIFO's enable = 1, No FIFO's uart becomes 1 character deep = 0
		enum {
			PL011_DATA_5BITS = 0,
			PL011_DATA_6BITS = 1,
			PL011_DATA_7BITS = 2,
			PL011_DATA_8BITS = 3,
		} DATALEN : 2;												// @5-6	  Data length for transmission
		unsigned SPS : 1;											// @7	  Stick parity select 1 = enabled, 0 = stick parity is disabled 
		unsigned _reserved : 24;									// @8-31  Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} pl011_lrch_reg_t;

/*--------------------------------------------------------------------------}
{    PL011 UART CR Register BCM2835 ARM Peripheral manual page 185/186/187	}
{                        CR = Control Register                              }                
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned UARTEN : 1;										// @0	  UART enable = 1, disabled = 0
		unsigned _unused : 6;										// @2-6	  unused bits
		unsigned LBE : 1;											// @7	  Loop back enable = 1
		unsigned TXE : 1;											// @8	  TX enabled = 1, disabled = 0
		unsigned RXE : 1;											// @9	  RX enabled = 1, disabled = 0
		unsigned DTR_unused : 1;									// @10	  DTR unused
		unsigned RTS : 1;											// @11	  RTS complement when the bit is programmed to a 1 then nUARTRTS is LOW.
		unsigned OUT : 2;											// @12-13 Unsupported
		unsigned RTSEN : 1;											// @14	  RTS hardware flow control enable if this bit is set to 1. 
		unsigned CTSEN : 1;											// @15	  CTS hardware flow control enable if this bit is set to 1.
		unsigned _reserved : 16;									// @16-31 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} pl011_cr_reg_t;


/*--------------------------------------------------------------------------}
{    PL011 UART IFLS Register BCM2835 ARM Peripheral manual page 187/188	}
{                 IFLS = Interrupt Fifo Level Select						}
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{

		/* Transmit interrupt FIFO level select of trigger point for the transmit interrupt */
		enum {
			TXIFLSEL_EIGHTH			= 0b000,	// Trigger on 1/8 of FIFO depth
			TXIFLSEL_QUARTER		= 0b001,	// Trigger on 1/4 of FIFO depth
			TXIFLSEL_HALF			= 0b010,	// Trigger on 1/2 of FIFO depth
			TXIFLSEL_THREEQUARTERS	= 0b011,	// Trigger on 3/4 of FIFO depth
			TXIFLSEL_SEVENEIGHTHS	= 0b100,	// Trigger on 7/8 of FIFO depth
		} TXIFLSEL : 4;												// @0-3	  Transmit interrupt FIFO level select

		/*  Receive interrupt FIFO level select of trigger point for the  receive  interrupt */
		enum {
			RXIFLSEL_EIGHTH			= 0b000,	// Trigger on 1/8 of FIFO depth
			RXIFLSEL_QUARTER		= 0b001,	// Trigger on 1/4 of FIFO depth
			RXIFLSEL_HALF			= 0b010,	// Trigger on 1/2 of FIFO depth
			RXIFLSEL_THREEQUARTERS	= 0b011,	// Trigger on 3/4 of FIFO depth
			RXIFLSEL_SEVENEIGHTHS	= 0b100,	// Trigger on 7/8 of FIFO depth
		} RXIFLSEL : 4;												// @4-7		Receive interrupt FIFO level select
		
		unsigned _reserved : 24;									// @8-31  Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} pl011_ifls_reg_t;

/*--------------------------------------------------------------------------}
{    PL011 UART IMSC Register BCM2835 ARM Peripheral manual page 188/189	}
{               IMSC = Interrupt Mask Set and Clear	    					}
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned RIMIM : 1;											// @0		Unsupported, write zero, read as don't care 
		unsigned CTSMIM : 1;										// @1		Reads UARTCTSINTR interrupt mask status. Can write 0 or 1 to change mask. 
		unsigned DCDMIM : 1;										// @2		Unsupported, write zero, read as don't care 
		unsigned DSRMIM : 1;										// @3		Unsupported, write zero, read as don't care
		unsigned RXIM : 1;											// @4		Reads UARTRXINTR (RX interrupt) mask status. Can write 0 or 1 to change mask
		unsigned TXIM : 1;											// @5		Reads UARTTXINTR (TX interrupt) mask status. Can write 0 or 1 to change mask
		unsigned RTIM : 1;											// @6		Reads UARTRTINTR (Timeout interrupt) mask status. Can write 0 or 1 to change mask
		unsigned FEIM : 1;											// @7		Reads UARTFEINTR (Framing Error interrupt) mask status. Can write 0 or 1 to change mask
		unsigned PEIM : 1;											// @8		Reads UARTPEINTR (Parity Error interrupt) mask status. Can write 0 or 1 to change mask
		unsigned BEIM : 1;											// @9		Reads UARTBEINTR (Break Error interrupt) mask status. Can write 0 or 1 to change mask
		unsigned OEIM : 1;											// @10		Reads UARTOEINTR (Overrun Error interrupt) mask status. Can write 0 or 1 to change mask
		unsigned _reserved : 21;									// @11-31	Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} pl011_imsc_reg_t;


/*--------------------------------------------------------------------------}
{     PL011 UART RIS Register BCM2835 ARM Peripheral manual page 189/190 	}
{                     RIS = Raw Interrupt State		    					}
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned RIRMIS : 1;										// @0		Unsupported, write zero, read as don't care 
		unsigned CTSRMIS : 1;										// @1		Reads raw UARTCTSINTR interrupt state. 
		unsigned DCDRMIS : 1;										// @2		Unsupported, write zero, read as don't care 
		unsigned DSRRMIS : 1;										// @3		Unsupported, write zero, read as don't care
		unsigned RXRIS : 1;											// @4		Reads raw UARTRXINTR interrupt state (RX interrupt).
		unsigned TXRIS : 1;											// @5		Reads raw UARTTXINTR interrupt state (TX interrupt).
		unsigned RTRIS : 1;											// @6		Reads raw UARTRTINTR interrupt state (Timeout interrupt).
		unsigned FERIS : 1;											// @7		Reads raw UARTFEINTR interrupt state (Framing Error interrupt).
		unsigned PERIS : 1;											// @8		Reads raw UARTPEINTR interrupt state (Parity Error interrupt).
		unsigned BERIS : 1;											// @9		Reads raw UARTBEINTR interrupt state (Break Error interrupt).
		unsigned OERIS : 1;											// @10		Reads raw UARTOEINTR interrupt state (Overrun Error interrupt).
		unsigned _reserved : 21;									// @11-31	Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} pl011_ris_reg_t;

/*--------------------------------------------------------------------------}
{     PL011 UART RIS Register BCM2835 ARM Peripheral manual page 190/191 	}
{                    MIS = Mask Interrupt State		    					}
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned RIMMIS : 1;										// @0		Unsupported, write zero, read as don't care 
		unsigned CTSMMIS : 1;										// @1		Reads raw UARTCTSINTR interrupt mask state. 
		unsigned DCDMMIS : 1;										// @2		Unsupported, write zero, read as don't care 
		unsigned DSRMMIS : 1;										// @3		Unsupported, write zero, read as don't care
		unsigned RXMIS : 1;											// @4		Reads raw UARTRXINTR interrupt mask state (RX interrupt).
		unsigned TXMIS : 1;											// @5		Reads raw UARTTXINTR interrupt mask state (TX interrupt).
		unsigned RTMIS : 1;											// @6		Reads raw UARTRTINTR interrupt mask state (Timeout interrupt).
		unsigned FEMIS : 1;											// @7		Reads raw UARTFEINTR interrupt mask state (Framing Error interrupt).
		unsigned PEMIS : 1;											// @8		Reads raw UARTPEINTR interrupt mask state (Parity Error interrupt).
		unsigned BEMIS : 1;											// @9		Reads raw UARTBEINTR interrupt mask state (Break Error interrupt).
		unsigned OEMIS : 1;											// @10		Reads raw UARTOEINTR interrupt mask state (Overrun Error interrupt).
		unsigned _reserved : 21;									// @11-31	Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} pl011_mis_reg_t;

/*--------------------------------------------------------------------------}
{       PL011 UART ICR Register BCM2835 ARM Peripheral manual page 192	    }
{                    ICR = Interrupt Clear Register							}
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned RIMIC : 1;											// @0	  Unsupported, write zero, read as don't care 
		unsigned CTSMIC : 1;										// @1	  nUARTCTS modem masked interrupt status 
		unsigned DCDMIC : 1;										// @2	  Unsupported, write zero, read as don't care 
		unsigned DSRMIC : 1;										// @3	  Unsupported, write zero, read as don't care 
		unsigned RXIC : 1;											// @4	  Receive interrupt clear. 
		unsigned TXIC : 1;											// @5	  Transmit interrupt clear
		unsigned RTIC : 1;											// @6	  Receive timeout interrupt clear. 
		unsigned FEIC : 1;											// @7	  Framing error interrupt clear.
		unsigned PEIC : 1;											// @8	  Parity error interrupt clear.
		unsigned BEIC : 1;											// @9	  Break error interrupt clear.
		unsigned OEIC : 1;											// @10	  Overrun error interrupt clear.
		unsigned _reserved : 21;									// @11-31 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} pl011_icr_reg_t;


/*--------------------------------------------------------------------------}
{	 PL011 UART STRUCTURE LAYOUT BCM2835 ARM Peripheral manual page 175	    }
{--------------------------------------------------------------------------*/
struct __attribute__((__packed__, aligned(4))) PL011UARTRegisters {
	pl011_data_reg_t DR;											// +0x0
	uint32_t RSRECR;												// +0x4
	uint32_t _unused[4];											// +0x8, +0xC, +0x10, +0x14
	pl011_fr_reg_t FR;												// +0x18
	uint32_t _unused1[2];											// +0x1C, 0x20
	pl011_ibrd_reg_t IBRD;											// +0x24
	pl011_fbrd_reg_t FBRD;											// +0x28
	pl011_lrch_reg_t LCRH;											// +0x2C
	pl011_cr_reg_t CR;												// +0x30
	pl011_ifls_reg_t IFLS;											// +0x34
	pl011_imsc_reg_t IMSC;											// +0x38
	const pl011_ris_reg_t RIS;										// +0x3C   ** READ ONLY
	const pl011_mis_reg_t MIS;										// +0x40   ** READ ONLY
	pl011_icr_reg_t ICR;											// +0x44
	uint32_t DMACR;													// +0x48
};


/* Device Table Interface calls matching Conf.h as devtab[x]  */
/* Others come from the base uart code via function pointers  */
interrupt pl011_uartInterrupt (void);

/* Specific function pointer calls */
xinu_devcall pl011_HwInit (device *devptr);
void pl011_uartKickTx (struct uart * uartptr);
void pl011_uartHwStat (void *csr);

int pl011_SetCommState (struct uart* uart, LPDCB  dcb);

#endif                          /* _PL011_H_ */
