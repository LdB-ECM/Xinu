/**
 * @file uart.h
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _UART_H_
#define _UART_H_

#include <stdbool.h>
#include <xinu.h>
#include <device.h>
#include <semaphore.h>

/* UART Buffer lengths */
#ifndef UART_IBLEN
#define UART_IBLEN      1024
#endif
#ifndef UART_OBLEN
#define UART_OBLEN      1024
#endif

#define UART_BAUD       115200		/**< Default console baud rate.         */



/*--------------------------------------------------------------------------}
{					STANDARD WINDOWS DCB STRUCTURE						    }
{    The equivalent on linux is the options on tcgetattr(fd, &options);     }
{     and tcsetattr(fd, TCSANOW, &options);  May yet add both API calls;    }
{--------------------------------------------------------------------------*/
typedef struct __attribute__((__packed__)) _DCB {
	unsigned int DCBlength;				//			Length of this DCB descriptor
	unsigned int BaudRate;				//			The baud rate at which the device is operating

    /* 16 bit pack fields to ensure alignment */
	unsigned int fBinary : 1;			// @0		TRUE, binary mode is enabled
	unsigned int fParity : 2;			// @1-2		Parity modes  0x00 = No Parity, 0x1 = Odd Parity, 0x2 = Even Parity
	unsigned int fOutxCtsFlow : 1;		// @3		TRUE, the CTS (clear-to-send) signal is used for output flow control
	unsigned int fOutxDsrFlow : 1;		// @4		TRUE, the DSR (data-set-ready) signal is monitored for output flow control.
	unsigned int fDtrControl : 2;		// @5-6		DTR flow control modes  0x00 = No flow control, 0x1 = DTR line on when port open, 0x2 = DTR handshaking
	unsigned int fDsrSensitivity : 1;	// @7		TRUE, the rx data is ignored unless DSR modem input line is high
	unsigned int fTXContinueOnXoff : 1;	// @8		TRUE, sending continues if it is XoffLim bytes from being full and an XOFF char has been sent
	unsigned int fOutX : 1;				// @9		TRUE, transmission stops when the XoffChar character is received  
	unsigned int fInX : 1;				// @10		TRUE, XoffChar char sent XoffLim bytes from full, XonChar char sent XonLim bytes of being empty.
	unsigned int fErrorChar : 1;		// @11		TRUE, and fParity TRUE, and parity error chars are replaced by by the ErrorChar member
	unsigned int fNull : 1;				// @12		TRUE, null bytes are discarded when received
	unsigned int fRtsControl : 2;		// @13-14	RTS flow control modes  0x00 = RTS low always, 0x1 = RTS line on when port open, 0x2 = RTS handshaking
	unsigned int fAbortOnError : 1;		// @15		TRUE, on any error the driver terminates all operations until ClearCommError function is called.

	/* 16 bit pack fields to ensure alignment */
	unsigned int datalength : 2;		// @0-1		Data length modes 0x00 = 5 Bits, 0x01 = 6 bits, 0x10 = 7 bits  0x11 = 8bits
	unsigned int stopbits : 2;			// @2-3		Data stop bits	  0x00 = 1 stop, 0x01 = 1.5 stop , 0x10 = 2 stop bits
	unsigned int supported_datalen : 4;	// @4-7		**READ ONLY** Data lengths supported 4bit mask (8bit 7bit 6 bit 5 bit), not all devices support all settings 
	unsigned int supported_stops : 3;	// @8-10	**READ ONLY** Stop bits supported 3 bit mask (2 stop,  1.5 stop  1 stop), not all devices support all settings
	unsigned int supported_partiy : 1;	// @11		**READ ONLY** Parity is supported if set
	unsigned int supported_CTS : 1;		// @12		**READ ONLY** CTS pin exists and is supported if set
	unsigned int supported_DSR : 1;		// @13		**READ ONLY** DSR pin exists and is supported if set
	unsigned int supported_RTS : 1;		// @14		**READ ONLY** RTS pin exists and is supported if set
	unsigned int supported_DTR : 1;		// @15		**READ ONLY** DTR pin exists and is supported if set

	/* 16 bit pack fields to ensure alignment */
	unsigned int XonLim;				//			Minimum number of bytes in the input buffer before flow control is activated to allow transmission by the sender (XON or handshake)
	unsigned int XoffLim;				//          Minimum number of free bytes in the input buffer before flow control is inhibit the sender (XOFF or handshake)

	/* 16 bit pack fields to ensure alignment */
	unsigned int XonChar : 8;			//			The value of the XON character for both transmission and reception
	unsigned int XoffChar: 8;			//			The value of the XOFF character for both transmission and reception
	
	/* 16 bit pack fields to ensure alignment */
	unsigned int ErrorChar : 8;			//			The value of the character used to replace bytes received with a parity error
	unsigned int EofChar : 8;			//			The value of the character used to signal the end of data

} DCB, *LPDCB;

/**
 * UART control block
 */
struct uart
{
    /* Pointers to associated structures */
    void *csr;						/**< Control & status register pointer	*/
    device *dev;					/**< Dev structure that owns this UART	*/

	/* Internal DCB record */
	DCB	dcb;						/**< Internal DCB record				*/

    /* Statistical Counts */
    unsigned int cout;				/**< Characters output                  */
    unsigned int cin;				/**< Characters input                   */
    unsigned int lserr;				/**< Receiver error count               */
    unsigned int ovrrn;				/**< Characters overrun                 */
    unsigned int iirq;				/**< Input IRQ count                    */
    unsigned int oirq;				/**< Output IRQ count                   */

    /* UART input fields */
    unsigned char iflags;			/**< Input flags                        */
    semaphore isema;				/**< Count of input bytes ready         */
    unsigned short istart;			/**< Index of first byte                */
    unsigned short icount;			/**< Bytes in buffer                    */
    unsigned char in[UART_IBLEN];	/**< Input buffer                       */

    /* UART output fields */
    unsigned char oflags;			/**< Output flags                       */
    semaphore osema;				/**< Count of buffer space free         */
	volatile unsigned short ostart; /**< Index of first byte                */
    volatile unsigned short ocount;	/**< Bytes in buffer                    */
    unsigned char out[UART_OBLEN];  /**< Output buffer                      */
    bool oidle;						/**< UART transmitter idle              */

	/* These function pointers are the actual hardware specific calls */
	/* They are set automatically as each UART driver is installed    */
	void (*uartKickTx) (struct uart* uartptr);
	void (*uartHwStat)(void *csr);

	/**< UART API hardware specific functions    */	
	int (*GetCommStateFn) (struct uart* uart, LPDCB  dcb);			// GetCommState function pointer for device
	int (*SetCommStateFn) (struct uart* uart, LPDCB  dcb);			// SetCommState function pointer for device
};

extern struct uart uarttab[];

/* UART input flags */
#define UART_IFLAG_NOBLOCK    0x0001 /**< do non-blocking input         */
#define UART_IFLAG_ECHO       0x0002 /**< echo input                    */

/* UART output flags */
#define UART_OFLAG_NOBLOCK    0x0001 /**< do non-blocking output        */

/* uartControl() functions  */
#define UART_CTRL_SET_IFLAG   0x0010 /**< set input flags               */
#define UART_CTRL_CLR_IFLAG   0x0011 /**< clear input flags             */
#define UART_CTRL_GET_IFLAG   0x0012 /**< get input flags               */
#define UART_CTRL_SET_OFLAG   0x0013 /**< set output flags              */
#define UART_CTRL_CLR_OFLAG   0x0014 /**< clear output flags            */
#define UART_CTRL_GET_OFLAG   0x0015 /**< get output flags              */
#define UART_CTRL_OUTPUT_IDLE 0x0016 /**< determine if transmit idle    */

/* Driver functions */
xinu_devcall uartInit (device*);
xinu_devcall uartRead (device*, void*, unsigned int);
xinu_devcall uartWrite (device*, const void *, unsigned int);
xinu_devcall uartGetc  (device*);
xinu_devcall uartPutc  (device*, char);
xinu_devcall uartControl (device *, int, long, long);
interrupt uartInterrupt (void);
void uartStat (unsigned short);

/* API functions */
int SetCommState (HANDLE hFile, LPDCB  dcb);
int GetCommState (HANDLE hFile, LPDCB  dcb);

#endif                          /* _UART_H_ */
