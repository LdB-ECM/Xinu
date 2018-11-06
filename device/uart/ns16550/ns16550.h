/**
 * @file ns16550.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#ifndef _NS16550_H_
#define _NS16550_H_

#include <stdint.h>
#include <xinu.h>

/*--------------------------------------------------------------------------}
{         MINI UART IO Register BCM2835 ARM Peripheral manual page 11		}
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned DATA : 8;											// @0-7 Transmit Read/write data if DLAB=0, DLAB = 1 Lower 8 bits of 16 bit baud rate generator 
		unsigned reserved : 24;										// @8-31 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} mu_io_reg_t;

/*--------------------------------------------------------------------------}
{ MINI UART INTERRUPT ENABLE Register BCM2835 ARM Peripheral manual page 12	}
{   PAGE HAS ERRORS: https://elinux.org/BCM2835_datasheet_errata            }
{   It is essentially same as standard 16550 register IER                   }
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned RXEI : 1;											// @0	 If this bit is set the interrupt line is asserted if the receive FIFO holds at least 1 byte.
		unsigned TXEI : 1;											// @1	 If this bit is set the interrupt line is asserted if the transmit FIFO is empty.  
		unsigned LSI : 1;											// @2	 If this bit is set the Receiver Line Status interrupt is asserted on overrun error, parity error, framing error etc
		unsigned MSI : 1;											// @3	 If this bit is set the Modem Status interrupt is asserted on a change To Send(CTS), Data Set Ready(DSR)
		unsigned reserved : 28;										// @4-31 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} mu_ie_reg_t;

/*--------------------------------------------------------------------------}
{   MINI UART INTERRUPT ID Register BCM2835 ARM Peripheral manual page 13	}
{--------------------------------------------------------------------------*/
typedef union
{
	const struct __attribute__((__packed__))
	{																// THE REGISTER READ DEFINITIONS 
		unsigned PENDING : 1;										// @0	 This bit is clear whenever an interrupt is pending 
		enum {
			MU_NO_INTERRUPTS = 0,									//		 No interrupts pending
			MU_TXE_INTERRUPT = 1,									//		 Transmit buffer empty causing interrupt
			MU_RXD_INTERRUPTS = 2,									//		 receive fifa has data causing interrupt 
		} SOURCE : 2;												// @1-2	 READ this register shows the interrupt ID bits 
		unsigned reserved_rd : 29;									// @3-31 Reserved - Write as 0, read as don't care 
	};
	struct __attribute__((__packed__))
	{																// THE REGISTER WRITE DEFINITIONS 
		unsigned unused : 1;										// @0	 This bit has no use when writing 
		unsigned RXFIFO_CLEAR : 1;									// @1	 Clear the receive fifo by writing a 1
		unsigned TXFIFO_CLEAR : 1;									// @2	 Clear the transmit fifo by writing a 1
		unsigned reserved_wr : 29;									// @3-31 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} mu_ii_reg_t;

/*--------------------------------------------------------------------------}
{	MINI UART LINE CONTROL Register BCM2835 ARM Peripheral manual page 14	}
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		enum {
			LENGTH_5_BITS = 0,										// Not valid on a Pi
			LENGTH_6_BITS = 1,										// Not valid on a Pi
			LENGTH_7_BITS = 2,
			LENGTH_8_BITS = 3,
		} DATA_LENGTH : 2;											// @0-1	 If clear the UART works in 7-bit mode, If set the UART works in 8-bit mode
		unsigned reserved : 4;										// @2-5	 Reserved, write zero, read as don’t care Some of these bits have functions in a 16550 compatible UART but are ignored here
		unsigned BREAK : 1;											// @6	 If set high the UART1_TX line is pulled low continuously
		unsigned DLAB : 1;											// @7	 DLAB access control bit.
		unsigned reserved1 : 24;									// @8-31 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} mu_lcr_reg_t;

/*--------------------------------------------------------------------------}
{	MINI UART MODEM CONTROL Register BCM2835 ARM Peripheral manual page 14	}
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned reserved : 1;										// @0	 Reserved, write zero, read as don’t care 
		unsigned RTS : 1;											// @1	 If clear the UART1_RTS line is high, If set the UART1_RTS line is low 
		unsigned reserved1 : 30;									// @2-31 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} mu_mcr_reg_t;


/*--------------------------------------------------------------------------}
{	MINI UART LINE STATUS Register BCM2835 ARM Peripheral manual page 15	}
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned RXFDA : 1;											// @0	 This bit is set if the receive FIFO holds at least 1 
		unsigned RXOE : 1;											// @1	 This bit is set if there was a receiver overrun
		unsigned reserved : 3;										// @2-4	 Reserved, write zero, read as don’t care 
		unsigned TXFE : 1;											// @5	 This bit is set if the transmit FIFO can accept at least one byte
		unsigned TXIdle : 1;										// @6	 This bit is set if the transmit FIFO is empty and the transmitter is idle. (Finished shifting out the last bit). 
		unsigned reserved1 : 25;									// @7-31 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} mu_lsr_reg_t;

/*--------------------------------------------------------------------------}
{	MINI UART MODEM STATUS Register BCM2835 ARM Peripheral manual page 15	}
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned reserved : 4;										// @0-3	 Reserved, write zero, read as don’t care 
		unsigned CTS : 1;											// @4	 This bit is the inverse of the CTS input, If set the UART1_CTS pin is low If clear the UART1_CTS pin is high 
		unsigned reserved1 : 27;									// @5-31 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} mu_msr_reg_t;

/*--------------------------------------------------------------------------}
{	  MINI UART SCRATCH Register BCM2835 ARM Peripheral manual page 16	    }
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned USER_DATA : 8;										// @0-7	 One whole byte extra on top of the 134217728 provided by the SDC  
		unsigned reserved : 24;										// @8-31 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} mu_scratch_reg_t;

/*--------------------------------------------------------------------------}
{     MINI UART CONTROL Register BCM2835 ARM Peripheral manual page 16	    }
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned RXE : 1;											// @0	 If this bit is set the mini UART receiver is enabled, If this bit is clear the mini UART receiver is disabled  
		unsigned TXE : 1;											// @1	 If this bit is set the mini UART transmitter is enabled, If this bit is clear the mini UART transmitter is disabled
		unsigned EnableRTS : 1;										// @2	 If this bit is set the RTS line will de-assert if the rc FIFO reaches it 'auto flow' level. If this bit is clear RTS is controlled by the AUX_MU_MCR_REG register bit 1. 
		unsigned EnableCTS : 1;										// @3	 If this bit is set the transmitter will stop if the CTS line is de-asserted. If this bit is clear the transmitter will ignore the status of the CTS line
		enum {
			FIFOhas3spaces = 0,
			FIFOhas2spaces = 1,
			FIFOhas1spaces = 2,
			FIFOhas4spaces = 3,
		} RTSflowLevel : 2;											// @4-5	 These two bits specify at what receiver FIFO level the RTS line is de-asserted in auto-flow mode
		unsigned RTSassertLevel : 1;								// @6	 If set the RTS auto flow assert level is low, If clear the RTS auto flow assert level is high
		unsigned CTSassertLevel : 1;								// @7	 If set the CTS auto flow assert level is low, If clear the CTS auto flow assert level is high
		unsigned reserved : 24;										// @8-31 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} mu_cntl_reg_t;


/*--------------------------------------------------------------------------}
{	  MINI UART STATUS Register BCM2835 ARM Peripheral manual page 18	    }
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned RXFDA : 1;											// @0	 This bit is set if the receive FIFO holds at least 1
		unsigned TXFE : 1;											// @1	 If this bit is set the mini UART transmitter FIFO can accept at least one more symbol
		unsigned RXIdle : 1;										// @2	 If this bit is set the receiver is idle. If this bit is clear the receiver is busy
		unsigned TXIdle : 1;										// @3	 If this bit is set the transmitter is idle. If this bit is clear the transmitter is idle
		unsigned RXOE : 1;											// @4	 This bit is set if there was a receiver overrun
		unsigned TXFF : 1;											// @5	 The inverse of bit 0
		unsigned RTS : 1;											// @6	 This bit shows the status of the RTS line
		unsigned CTS : 1;											// @7	 This bit shows the status of the CTS line
		unsigned TXFCE : 1;											// @8	 If this bit is set the transmitter FIFO is empty. Thus it can accept 8 symbols
		unsigned TX_DONE : 1;										// @9	 This bit is set if the transmitter is idle and the transmit FIFO is empty. It is a logic AND of bits 2 and 8 
		unsigned reserved : 6;										// @10-15 Reserved - Write as 0, read as don't care 
		unsigned RXFIFOLEVEL : 4;									// @16-19 These bits shows how many symbols are stored in the receive FIFO The value is in the range 0-8 
		unsigned reserved1 : 4;										// @20-23 Reserved - Write as 0, read as don't care 
		unsigned TXFIFOLEVEL : 4;									// @24-27 These bits shows how many symbols are stored in the transmit FIFO The value is in the range 0-8
		unsigned reserved2 : 4;										// @28-31 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} mu_stat_reg_t;

/*--------------------------------------------------------------------------}
{	  MINI UART BAUDRATE Register BCM2835 ARM Peripheral manual page 19  	}
{--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned DIVISOR : 16;										// @0-15	 Baudrate divisor  
		unsigned reserved : 16;										// @16-31	 Reserved - Write as 0, read as don't care 
	};
	uint32_t Raw32;													// Union to access all 32 bits as a uint32_t
} mu_baudrate_reg_t;

 /*--------------------------------------------------------------------------}
 {	  MINIUART STRUCTURE LAYOUT BCM2835 ARM Peripheral manual page 8	    }
 {--------------------------------------------------------------------------*/
struct __attribute__((__packed__, aligned(4))) ns16550_uart_csreg 
{
	mu_io_reg_t IO;													// +0x0
	mu_ie_reg_t IER;												// +0x4
	mu_ii_reg_t IIR;												// +0x8
	mu_lcr_reg_t LCR;												// +0xC
	mu_mcr_reg_t MCR;												// +0x10
	const mu_lsr_reg_t LSR;											// +0x14	** READ ONLY HENCE CONST **
	const mu_msr_reg_t MSR;											// +0x18	** READ ONLY HENCE CONST **
	mu_scratch_reg_t SCRATCH;										// +0x1C
	mu_cntl_reg_t CNTL;												// +0x20
	const mu_stat_reg_t STAT;										// +0x24	** READ ONLY HENCE CONST **
	mu_baudrate_reg_t BAUD;											// +0x28;
};


/* Alternative names for control and status registers                   */
//#define rbr buffer              /**< receive buffer (read only)         */
//#define thr buffer              /**< transmit hold (write only)         */
//#define fcr iir                 /**< FIFO control (write only)          */
//#define dll buffer              /**< divisor latch low byte             */
//#define dlm ier                 /**< divisor latch high byte            */

/* UART Bit flags for control and status registers                      */
/* Interrupt enable bits                                                */
#define UART_IER_ERBFI  0x01    /**< Received data interrupt mask       */
#define UART_IER_ETBEI  0x02    /**< Transmitter buffer empty interrupt */
#define UART_IER_ELSI   0x04    /**< Recv line status interrupt mask    */
#define UART_IER_EMSI   0x08    /**< Modem status interrupt mask        */

/* Interrupt identification masks */
#define UART_IIR_IRQ    0x01    /**< Interrupt pending bit              */
#define UART_IIR_IDMASK 0x0E    /**< 3-bit field for interrupt ID       */
#define UART_IIR_MSC    0x00    /**< Modem status change                */
#define UART_IIR_THRE   0x02    /**< Transmitter holding register empty */
#define UART_IIR_RDA    0x04    /**< Receiver data available            */
#define UART_IIR_RLSI   0x06    /**< Receiver line status interrupt     */
#define UART_IIR_RTO    0x0C    /**< Receiver timed out                 */

/* FIFO control bits */
#define UART_FCR_EFIFO  0x01    /**< Enable in and out hardware FIFOs   */
#define UART_FCR_RRESET 0x02    /**< Reset receiver FIFO                */
#define UART_FCR_TRESET 0x04    /**< Reset transmit FIFO                */
#define UART_FCR_TRIG0  0x00    /**< RCVR FIFO trigger level one char   */
#define UART_FCR_TRIG1  0x40    /**< RCVR FIFO trigger level 1/4        */
#define UART_FCR_TRIG2  0x80    /**< RCVR FIFO trigger level 2/4        */
#define UART_FCR_TRIG3  0xC0    /**< RCVR FIFO trigger level 3/4        */

/* Line control bits */
#define UART_LCR_DLAB   0x80    /**< Divisor latch access bit           */
#define UART_LCR_8N1    0x03    /**< 8 bits, no parity, 1 stop          */

/* Modem control bits */
#define UART_MCR_OUT2   0x08    /**< User-defined OUT2.                 */
#define UART_MCR_LOOP   0x10    /**< Enable loopback test mode          */

/* Line status bits */
#define UART_LSR_DR     0x01    /**< Data ready                         */
#define UART_LSR_THRE   0x20    /**< Transmit-hold-register empty       */
#define UART_LSR_TEMT   0x40    /**< Transmitter empty                  */

extern int (*rpi_mailbox) (uint32_t channel, uint32_t msg);

interrupt ns16550_uartInterrupt(void);

xinu_devcall ns16550_uartHwInit(device *devptr);
void ns16550_uartKickTx(struct uart *uartptr);
void ns16550_uartHwStat(void *csr);

int ns16550_SetCommState(struct uart* uart, LPDCB  dcb);

#endif                          /* _NS16550_H_ */
