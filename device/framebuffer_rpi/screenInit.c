/**
 * @file screenInit.c
 *
 * Initializes communication channels between VC and ARM.
 */
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <stdint.h>
#include <xinu.h>
#include <framebuffer.h>
#include <stdlib.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include "font8x12.h"
#include <rpi-platform.h>

#include <platform.h>

extern uint32_t RPi_IO_Base_Addr;
extern uint32_t RPi_ARM_TO_GPU_Alias;

int rows;
int cols;
int cursor_row;
int cursor_col;
unsigned long background;
unsigned long foreground;
unsigned long linemap[MAPSIZE];
bool minishell;
bool screen_initialized;

struct framebuffer fbtab[NFRAMEBUFFER] __attribute__((aligned(16))) = { 0 };

/*--------------------------------------------------------------------------}
{				   DIFFERENT COLOR LAYOUT DEFINITIONS						}
{--------------------------------------------------------------------------*/

/*-[INTERNAL: RGB565 ]------------------------------------------------------}
. 16 Bit colour RGB565 layout
.--------------------------------------------------------------------------*/
typedef union {
	struct __attribute__((__packed__))
	{
		unsigned rgb565B : 5;			// Blue
		unsigned rgb565G : 6;			// Green
		unsigned rgb565R : 5;			// Red
	};
	uint16_t Raw16;						// Raw 16bit access
} RGB565;

/*-[INTERNAL: RGB ]---------------------------------------------------------}
. 24 Bit colour RGB layout
.--------------------------------------------------------------------------*/
typedef union {
	struct __attribute__((__packed__))
	{
		unsigned rgbBlue : 8;			// Blue
		unsigned rgbGreen : 8;			// Green
		unsigned rgbRed : 8;			// Red
	};
	uint8_t Raw8[3];					// Raw 3x8 bit access
} RGB;

/*-[INTERNAL: RGBA ]--------------------------------------------------------}
. 32 Bit colour RGBA layout
.--------------------------------------------------------------------------*/
typedef union
{
	struct __attribute__((__packed__))
	{
		unsigned rgbBlue : 8;			// Blue
		unsigned rgbGreen : 8;			// Green
		unsigned rgbRed : 8;			// Red
		unsigned rgbAlpha : 8;			// Alpha
	};
	RGB rgb;							// RGB triple (1st 3 bytes)
	uint32_t Raw32;						// Raw 32 bit access								
} RGBA;


/*--------------------------------------------------------------------------}
{					   32 BIT COLOUR GRAPHICS ROUTINES						}
{--------------------------------------------------------------------------*/

/*-[INTERNAL: SetPixel32]---------------------------------------------------}
. 32 Bit colour version of the SetPixel routine with the given colour. As an
. internal high speed function everything is assumed checked
.--------------------------------------------------------------------------*/
static void SetPixel32 (struct framebuffer* fb, uint32_t x, uint32_t y, uint32_t crColor)
{
	volatile uint32_t* __attribute__((aligned(4))) video_wr_ptr = (uint32_t*)(fb->address + (y * fb->pitch) + (y * 4));
	video_wr_ptr[x] = crColor;										// Write the colour
}

/*-[INTERNAL: ClearArea32]--------------------------------------------------}
. 32 Bit colour version of the clear area call which block fills the given
. area from (x1,y1) to (x2,y2) with the current brush colour. As an internal
. function the (x1,y1) pairs are assumed to be smaller than (x2,y2).
.--------------------------------------------------------------------------*/
static void ClearArea32 (struct framebuffer* fb, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t crColor)
{
	volatile uint32_t* __attribute__((aligned(4))) video_wr_ptr = (uint32_t*)(fb->address + (y1 * fb->pitch) + (x1 * 4));
	for (uint32_t y = 0; y < (y2 - y1); y++) {						// For each y line
		for (uint32_t x = 0; x < (x2 - x1); x++) {					// For each x between x1 and x2
			video_wr_ptr[x] = crColor;								// Write the current brush colour
		}
		video_wr_ptr += fb->pitch / 4;								// Next line down **Ptr addition so div 4
	}
}

/*-[INTERNAL: VertLine32]---------------------------------------------------}
. 32 Bit colour version of the vertical line draw from (x,y) up or down cy
. pixels in the given colour. This is done for absolute speed in box drawing
. quite common in GUI elements.
.--------------------------------------------------------------------------*/
static void VertLine32 (struct framebuffer* fb, uint32_t x, uint32_t y, int32_t cy, uint32_t crColor)
{
	volatile uint32_t* __attribute__((aligned(4))) video_wr_ptr = (uint32_t*)(fb->address + (y * fb->pitch) + (x * 4));
	for (uint32_t i = 0; i < abs(cy); i++) {						// For each y line
		video_wr_ptr[0] = crColor;									// Write the colour
		if (cy >= 0) video_wr_ptr += fb->pitch / 4;					// Positive offset to next line **Ptr addition so div 4
		else  video_wr_ptr -= fb->pitch / 4;						// Negative offset to next line **Ptr addition so div 4
	}
}

/*-[INTERNAL: HorzLine32]---------------------------------------------------}
. 32 Bit colour version of the horizontal line draw from (x,y) left or right
. pixels in the given colour. This is done for absolute speed in box drawing
. quite common in GUI elements.
.--------------------------------------------------------------------------*/
static void HorzLine32 (struct framebuffer* fb, uint32_t x, uint32_t y, int32_t cx, uint32_t crColor)
{
	volatile uint32_t* __attribute__((aligned(4))) video_wr_ptr = (uint32_t*)(fb->address + (y * fb->pitch) + (x * 4));
	for (uint32_t i = 0; i < abs(cx); i++) {						// For each x pixel
		video_wr_ptr[0] = crColor;									// Write the colour
		if (cx >= 0) video_wr_ptr++;								// Positive offset to next pixel
		else  video_wr_ptr--;									// Negative offset to next pixel
	}
}

/*-[INTERNAL: DiagLine32]---------------------------------------------------}
. 32 Bit colour version of the diagonal line draw from (x1,y1) to (x2,y2) in
. the given colour. This is slow compared to horz and vertical lines draws.
.--------------------------------------------------------------------------*/
static void DiagLine32 (struct framebuffer* fb, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t crColor)
{
	volatile uint32_t* __attribute__((aligned(4))) video_wr_ptr = (uint32_t*)(fb->address + (y1 * fb->pitch) + (x1 * 4));
	uint32_t tx = 0;
	uint32_t ty = 0;
	int8_t xdir = 1;
	int8_t ydir = 1;
	uint32_t eulerMax;
	uint32_t dx;
	uint32_t dy;
	if (x1 > x2) {													// Check for x1 > x2 so backwards
		xdir = -1;													// Set xdir negative
		dx = x1 - x2;												// Dx value x1 is larger
	} else dx = x2 - x1;											// Dx value x2 is larger
	if (y1 > y2) {													// Check for y1 > y2 so backwards
		ydir = -1;													// Set ydir negative
		dy = y1 - y2;												// Dy value y1 is larger
	} else dy = y2 - y1;											// Dy value y2 is larger
	eulerMax = dx;													// Start with dx value
	if (dy > eulerMax) dy = eulerMax;								// If dy greater change to that value
	for (uint32_t i = 0; i < eulerMax; i++) {						// For euler steps
		video_wr_ptr[0] = crColor;									// Write pixel
		tx += dx;													// Increment test x value by dx
		if (tx >= eulerMax) {										// If tx >= eulerMax we step
			tx -= eulerMax;											// Subtract eulerMax
			video_wr_ptr += xdir;									// Move pointer left/right 1 pixel
		}
		ty += dy;													// Increment test y value by dy
		if (ty >= eulerMax) {										// If ty >= eulerMax we step
			ty -= eulerMax;											// Subtract eulerMax
			video_wr_ptr += (ydir*(fb->pitch / 4));					// Move pointer up/down 1 line **Ptr addition so div 4
		}
	}
}

/*-[INTERNAL: WriteChar32]--------------------------------------------------}
. 32 Bit colour version of the text character draw. The given character is
. drawn at the current position in the given colours.
.--------------------------------------------------------------------------*/
static void WriteChar32 (struct framebuffer* fb, uint32_t x, uint32_t y, uint8_t Ch, uint32_t fgColor, uint32_t bgColor)
{
	volatile uint32_t* __attribute__((aligned(4))) video_wr_ptr = (uint32_t*)(fb->address + (y * fb->pitch) + (x * 4));
	volatile uint8_t* fp = (uint8_t*)(uint32_t)(&fb->scrFontPtr[(uint32_t)Ch * fb->scrFontHt * fb->scrFontByteWth]);
	for (uint32_t y = 0; y < fb->scrFontHt; y++) {					// For screen font height
		uint8_t b = 0;
		for (uint8_t x = 0; x < fb->scrFontWth; x++) {				// For screen font bit width
			if (x % 8 == 0) b = *fp++;								// Load next font character
			if ((b & 0x80) != 0) video_wr_ptr[x] = fgColor;			// Write forground colour
				else video_wr_ptr[x] = bgColor;						// Write background colour
			b <<= 1;												// Roll font bits left
		}
		video_wr_ptr += fb->pitch / 4;								// Next line down **Ptr addition so div 4
	}
}

/*-[INTERNAL: TransparentWriteChar32]---------------------------------------}
. 32 Bit colour version of the transparent text character draw. The given
. character is drawn at the current position in the text colour but leaves
. background colours of bitmap in tact.
.--------------------------------------------------------------------------*/
static void TransparentWriteChar32 (struct framebuffer* fb, uint32_t x, uint32_t y, uint8_t Ch, uint32_t fgColor)
{
	volatile uint32_t* __attribute__((aligned(4))) video_wr_ptr = (uint32_t*)(fb->address + (y * fb->pitch) + (x * 4));
	volatile uint8_t* fp = (uint8_t*)(uint32_t)(&fb->scrFontPtr[(uint32_t)Ch * fb->scrFontHt * fb->scrFontByteWth]);
	for (uint32_t y = 0; y < fb->scrFontHt; y++) {					// For screen font height
		uint8_t b = 0;
		for (uint8_t x = 0; x < fb->scrFontWth; x++) {				// For screen font bit width
			if (x % 8 == 0) b = *fp++;								// Load next font character
			if ((b & 0x80) != 0) video_wr_ptr[x] = fgColor;			// Write forground colour if set leave if not set
			b <<= 1;												// Roll font bits left
		}
		video_wr_ptr += fb->pitch / 4;								// Next line down **Ptr addition so div 4
	}
}

/*--------------------------------------------------------------------------}
{					   24 BIT COLOUR GRAPHICS ROUTINES						}
{--------------------------------------------------------------------------*/

/*-[INTERNAL: SetPixel24]---------------------------------------------------}
. 24 Bit colour version of the SetPixel routine with the given colour. As an
. internal high speed function everything is assumed checked
.--------------------------------------------------------------------------*/
static void SetPixel24 (struct framebuffer* fb, uint32_t x, uint32_t y, uint32_t crColor)
{
	volatile RGB* __attribute__((aligned(1))) video_wr_ptr = (RGB*)(fb->address + (y * fb->pitch) + (x * 3));
	RGBA color = (RGBA)crColor;										// Typecast the color to make conversion easy
	video_wr_ptr[0] = color.rgb;									// Write the rgb triple colour
}

/*-[INTERNAL: ClearArea24]--------------------------------------------------}
. 24 Bit colour version of the clear area call which block fills the given
. area from (x1,y1) to (x2,y2) with the current brush colour. As an internal
. function the (x1,y1) pairs are assumed to be smaller than (x2,y2).
.--------------------------------------------------------------------------*/
static void ClearArea24 (struct framebuffer* fb, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t crColor)
{
	volatile RGB* __attribute__((aligned(1))) video_wr_ptr = (RGB*)(fb->address + (y1 * fb->pitch) + (x1 * 3));
	RGBA color = (RGBA)crColor;										// Typecast the color to make conversion easy
	for (uint32_t y = 0; y < (y2 - y1); y++) {						// For each y line
		for (uint32_t x = 0; x < (x2 - x1); x++) {					// For each x between x1 and x2
			video_wr_ptr[x] = color.rgb;							// Write the colour
		}
		video_wr_ptr += fb->pitch / 3;								// Offset to next line **Ptr addition so div 3
	}
}

/*-[INTERNAL: VertLine24]---------------------------------------------------}
. 24 Bit colour version of the vertical line draw from (x,y) up or down cy
. pixels in the given colour. This is done for absolute speed in box drawing
. quite common in GUI elements.
.--------------------------------------------------------------------------*/
static void VertLine24 (struct framebuffer* fb, uint32_t x, uint32_t y, int32_t cy, uint32_t crColor)
{
	volatile RGB* __attribute__((aligned(1))) video_wr_ptr = (RGB*)(fb->address + (y * fb->pitch) + (x * 3));
	RGBA color = (RGBA)crColor;										// Typecast the color to make conversion easy
	for (uint32_t i = 0; i < abs(cy); i++) {						// For each y line
		video_wr_ptr[0] = color.rgb;								// Write the colour
		if (cy >= 0) video_wr_ptr += fb->pitch / 3;					// Positive offset to next line **Ptr addition so div 3
		else  video_wr_ptr -= fb->pitch / 3;						// Negative offset to next line **Ptr addition so div 3
	}
}

/*-[INTERNAL: HorzLine24]---------------------------------------------------}
. 24 Bit colour version of the horizontal line draw from (x,y) up or down cy
. pixels in the given colour. This is done for absolute speed in box drawing
. quite common in GUI elements.
.--------------------------------------------------------------------------*/
static void HorzLine24 (struct framebuffer* fb, uint32_t x, uint32_t y, int32_t cx, uint32_t crColor)
{
	volatile RGB* __attribute__((aligned(1))) video_wr_ptr = (RGB*)(fb->address + (y * fb->pitch) + (x * 3));
	RGBA color = (RGBA)crColor;										// Typecast the color to make conversion easy
	for (uint32_t i = 0; i < abs(cx); i++) {						// For each x pixel
		video_wr_ptr[0] = color.rgb;									// Write the colour
		if (cx >= 0) video_wr_ptr++;								// Positive offset to next pixel
		else  video_wr_ptr--;									// Negative offset to next pixel
	}
}

/*-[INTERNAL: DiagLine24]---------------------------------------------------}
. 24 Bit colour version of the diagonal line draw from (x1,y1) to (x2,y2) in
. the given colour. This is slow compared to horz and vertical lines draws.
.--------------------------------------------------------------------------*/
static void DiagLine24 (struct framebuffer* fb, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t crColor)
{
	volatile RGB* __attribute__((aligned(1))) video_wr_ptr = (RGB*)(fb->address + (y1 * fb->pitch) + (x1 * 3));
	RGBA color = (RGBA)crColor;										// Typecast the color to make conversion easy
	uint32_t tx = 0;
	uint32_t ty = 0;
	int8_t xdir = 1;
	int8_t ydir = 1;
	uint32_t eulerMax;
	uint32_t dx;
	uint32_t dy;
	if (x1 > x2) {													// Check for x1 > x2 so backwards
		xdir = -1;													// Set xdir negative
		dx = x1 - x2;												// Dx value x1 is larger
	} else dx = x2 - x1;											// Dx value x2 is larger
	if (y1 > y2) {													// Check for y1 > y2 so backwards
		ydir = -1;													// Set ydir negative
		dy = y1 - y2;												// Dy value y1 is larger
	} else dy = y2 - y1;											// Dy value y2 is larger
	eulerMax = dx;													// Start with dx value
	if (dy > eulerMax) dy = eulerMax;								// If dy greater change to that value
	for (uint32_t i = 0; i < eulerMax; i++) {						// For euler steps
		video_wr_ptr[0] = color.rgb;								// Write pixel
		tx += dx;													// Increment test x value by dx
		if (tx >= eulerMax) {										// If tx >= eulerMax we step
			tx -= eulerMax;											// Subtract eulerMax
			video_wr_ptr += xdir;									// Move pointer left/right 1 pixel
		}
		ty += dy;													// Increment test y value by dy
		if (ty >= eulerMax) {										// If ty >= eulerMax we step
			ty -= eulerMax;											// Subtract eulerMax
			video_wr_ptr += (ydir*(fb->pitch / 3));					// Move pointer up/down 1 line **Ptr addition so div 3
		}
	}
}

/*-[INTERNAL: WriteChar24]--------------------------------------------------}
. 24 Bit colour version of the text character draw. The given character is
. drawn at the current position in the given colours.
.--------------------------------------------------------------------------*/
static void WriteChar24 (struct framebuffer* fb, uint32_t x, uint32_t y, uint8_t Ch, uint32_t fgColor, uint32_t bgColor)
{
	volatile RGB* __attribute__((aligned(1))) video_wr_ptr = (RGB*)(fb->address + (y * fb->pitch) + (x * 3));
	volatile uint8_t* fp = (uint8_t*)(uint32_t)(&fb->scrFontPtr[(uint32_t)Ch * fb->scrFontHt * fb->scrFontByteWth]);
	RGBA fc = (RGBA)fgColor;										// Typecast the color to make conversion easy
	RGBA bc = (RGBA)bgColor;										// Typecast the color to make conversion easy
	for (uint32_t y = 0; y < fb->scrFontHt; y++) {					// For screen font height
		uint8_t b = 0;
		for (uint8_t x = 0; x < fb->scrFontWth; x++) {				// For screen font bit width
			if (x % 8 == 0) b = *fp++;								// Load next font character
			if ((b & 0x80) != 0) video_wr_ptr[x] = fc.rgb;			// Write forground colour
				else video_wr_ptr[x] = bc.rgb;						// Write background colour
			b <<= 1;												// Roll font bits left
		}
		video_wr_ptr += fb->pitch / 3;								// Next line down **Ptr addition so div 3
	}
}

/*-[INTERNAL: TransparentWriteChar24]---------------------------------------}
. 16 Bit colour version of the transparent text character draw. The given
. character is drawn at the current position in the text colour but leaves
. background colours of bitmap in tact.
.--------------------------------------------------------------------------*/
static void TransparentWriteChar24 (struct framebuffer* fb, uint32_t x, uint32_t y, uint8_t Ch, uint32_t fgColor)
{
	volatile RGB* __attribute__((aligned(1))) video_wr_ptr = (RGB*)(fb->address + (y * fb->pitch) + (x * 3));
	volatile uint8_t* fp = (uint8_t*)(uint32_t)(&fb->scrFontPtr[(uint32_t)Ch * fb->scrFontHt * fb->scrFontByteWth]);
	RGBA fc = (RGBA)fgColor;										// Typecast the color to make conversion easy
	for (uint32_t y = 0; y < fb->scrFontHt; y++) {					// For screen font height
		uint8_t b = 0;
		for (uint8_t x = 0; x < fb->scrFontWth; x++) {				// For screen font bit width
			if (x % 8 == 0) b = *fp++;								// Load next font character
			if ((b & 0x80) != 0) video_wr_ptr[x] = fc.rgb;			// Write forground colour if set leave if not set
			b <<= 1;												// Roll font bits left
		}
		video_wr_ptr += fb->pitch / 3;								// Next line down **Ptr addition so div 3
	}
}

/*--------------------------------------------------------------------------}
{					   16 BIT COLOUR GRAPHICS ROUTINES						}
{--------------------------------------------------------------------------*/

/*-[INTERNAL: SetPixel16]---------------------------------------------------}
. 16 Bit colour version of the SetPixel routine with the given colour. As an
. internal high speed function everything is assumed checked
.--------------------------------------------------------------------------*/
static void  SetPixel16 (struct framebuffer* fb, uint32_t x, uint32_t y, uint32_t crColor)
{
	volatile RGB565* __attribute__((aligned(2))) video_wr_ptr = (RGB565*)(fb->address + (y * fb->pitch) + (x * 2));
	RGBA color = (RGBA)crColor;										// Typecast the color to make conversion easy
	RGB565 Bc;
	Bc.rgb565R = color.rgbRed >> 3;									// Place red bits
	Bc.rgb565G = color.rgbGreen >> 2;								// Place green bits
	Bc.rgb565B = color.rgbBlue >> 3;								// Place blue bits
	video_wr_ptr[0] = Bc;											// Write the colour
}

/*-[INTERNAL: ClearArea16]--------------------------------------------------}
. 16 Bit colour version of the clear area call which block fills the given
. area from (x1,y1) to (x2,y2) with the current brush colour. As an internal
. function the (x1,y1) pairs are assumed to be smaller than (x2,y2).
.--------------------------------------------------------------------------*/
static void ClearArea16 (struct framebuffer* fb, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t crColor)
{
	volatile RGB565* __attribute__((aligned(2))) video_wr_ptr = (RGB565*)(fb->address + (y1 * fb->pitch) + (x1 * 2));
	RGBA color = (RGBA)crColor;										// Typecast the color to make conversion easy
	RGB565 Bc;
	Bc.rgb565R = color.rgbRed >> 3;									// Place red bits
	Bc.rgb565G = color.rgbGreen >> 2;								// Place green bits
	Bc.rgb565B = color.rgbBlue >> 3;								// Place blue bits
	for (uint32_t y = 0; y < (y2 - y1); y++) {						// For each y line
		for (uint32_t x = 0; x < (x2 - x1); x++) {					// For each x between x1 and x2
			video_wr_ptr[x] = Bc;									// Write the colour
		}
		video_wr_ptr += (fb->pitch / 2);							// Offset to next line **Ptr addition so div 2
	}
}

/*-[INTERNAL: VertLine16]---------------------------------------------------}
. 16 Bit colour version of the vertical line draw from (x,y) up or down cy
. pixels in the given colour. This is done for absolute speed in box drawing
. quite common in GUI elements.
.--------------------------------------------------------------------------*/
static void VertLine16 (struct framebuffer* fb, uint32_t x, uint32_t y, int32_t cy, uint32_t crColor)
{
	volatile RGB565* __attribute__((aligned(2))) video_wr_ptr = (RGB565*)(fb->address + (y * fb->pitch) + (x * 2));
	RGBA color = (RGBA)crColor;										// Typecast the color to make conversion easy
	RGB565 Bc;
	Bc.rgb565R = color.rgbRed >> 3;									// Place red bits
	Bc.rgb565G = color.rgbGreen >> 2;								// Place green bits
	Bc.rgb565B = color.rgbBlue >> 3;								// Place blue bits
	for (uint32_t i = 0; i < abs(cy); i++) {						// For each y line
		video_wr_ptr[0] = Bc;										// Write the colour
		if (cy >= 0) video_wr_ptr += fb->pitch / 2;					// Positive offset to next line **Ptr addition so div 2
		else  video_wr_ptr -= fb->pitch / 2;						// Negative offset to next line **Ptr addition so div 2
	}
}

/*-[INTERNAL: HorzLine16]---------------------------------------------------}
. 24 Bit colour version of the horizontal line draw from (x,y) up or down cy
. pixels in the given colour. This is done for absolute speed in box drawing
. quite common in GUI elements.
.--------------------------------------------------------------------------*/
static void HorzLine16 (struct framebuffer* fb, uint32_t x, uint32_t y, int32_t cx, uint32_t crColor)
{
	volatile RGB565* __attribute__((aligned(2))) video_wr_ptr = (RGB565*)(fb->address + (y * fb->pitch) + (x * 2));
	RGBA color = (RGBA)crColor;										// Typecast the color to make conversion easy
	RGB565 Bc;
	Bc.rgb565R = color.rgbRed >> 3;									// Place red bits
	Bc.rgb565G = color.rgbGreen >> 2;								// Place green bits
	Bc.rgb565B = color.rgbBlue >> 3;								// Place blue bits
	for (uint32_t i = 0; i < abs(cx); i++) {						// For each x pixel
		video_wr_ptr[0] = Bc;										// Write the colour
		if (cx >= 0) video_wr_ptr++;								// Positive offset to next pixel 
		else  video_wr_ptr--;									// Negative offset to next pixel 
	}
}

/*-[INTERNAL: DiagLine16]---------------------------------------------------}
. 16 Bit colour version of the diagonal line draw from (x1,y1) to (x2,y2) in
. the given colour. This is slow compared to horz and vertical lines draws.
.--------------------------------------------------------------------------*/
static void DiagLine16 (struct framebuffer* fb, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t crColor)
{
	volatile RGB565* __attribute__((aligned(2))) video_wr_ptr = (RGB565*)(fb->address + (y1 * fb->pitch) + (x1 * 2));
	RGBA color = (RGBA)crColor;										// Typecast the color to make conversion easy
	RGB565 Bc;
	Bc.rgb565R = color.rgbRed >> 3;									// Place red bits
	Bc.rgb565G = color.rgbGreen >> 2;								// Place green bits
	Bc.rgb565B = color.rgbBlue >> 3;								// Place blue bits
	uint32_t tx = 0;
	uint32_t ty = 0;
	int8_t xdir = 1;
	int8_t ydir = 1;
	uint32_t eulerMax;
	uint32_t dx;
	uint32_t dy;
	if (x1 > x2) {													// Check for x1 > x2 so backwards
		xdir = -1;													// Set xdir negative
		dx = x1 - x2;												// Dx value x1 is larger
	} else dx = x2 - x1;											// Dx value x2 is larger
	if (y1 > y2) {													// Check for y1 > y2 so backwards
		ydir = -1;													// Set ydir negative
		dy = y1 - y2;												// Dy value y1 is larger
	} else dy = y2 - y1;											// Dy value y2 is larger
	eulerMax = dx;													// Start with dx value
	if (dy > eulerMax) dy = eulerMax;								// If dy greater change to that value
	for (uint32_t i = 0; i < eulerMax; i++) {						// For euler steps
		video_wr_ptr[0] = Bc;										// Write pixel
		tx += dx;													// Increment test x value by dx
		if (tx >= eulerMax) {										// If tx >= eulerMax we step
			tx -= eulerMax;											// Subtract eulerMax
			video_wr_ptr += xdir;									// Move pointer left/right 1 pixel
		}
		ty += dy;													// Increment test y value by dy
		if (ty >= eulerMax) {										// If ty >= eulerMax we step
			ty -= eulerMax;											// Subtract eulerMax
			video_wr_ptr += (ydir*(fb->pitch / 2));					// Move pointer up/down 1 line **Ptr addition so div 2
		}
	}
}

/*-[INTERNAL: WriteChar16]--------------------------------------------------}
. 16 Bit colour version of the text character draw. The given character is
. drawn at the current position in the given colours.
.--------------------------------------------------------------------------*/
static void WriteChar16 (struct framebuffer* fb, uint32_t x, uint32_t y, uint8_t Ch, uint32_t fgColor, uint32_t bgColor)
{
	volatile RGB565* __attribute__((aligned(2))) video_wr_ptr = (RGB565*)(fb->address + (y * fb->pitch) + (x * 2));
	volatile uint8_t* fp = (uint8_t*)(uint32_t)(&fb->scrFontPtr[(uint32_t)Ch * fb->scrFontHt * fb->scrFontByteWth]);
	RGBA fc = (RGBA)fgColor;										// Typecast the color to make conversion easy
	RGBA bc = (RGBA)bgColor;										// Typecast the color to make conversion easy
	RGB565 fc565, bc565;
	fc565.rgb565R = fc.rgbRed >> 3;									// Place red bits
	fc565.rgb565G = fc.rgbGreen >> 2;								// Place green bits
	fc565.rgb565B = fc.rgbBlue >> 3;								// Place blue bits
	bc565.rgb565R = bc.rgbRed >> 3;									// Place red bits
	bc565.rgb565G = bc.rgbGreen >> 2;								// Place green bits
	bc565.rgb565B = bc.rgbBlue >> 3;								// Place blue bits
	for (uint32_t y = 0; y < fb->scrFontHt; y++) {					// For screen font height
		uint8_t b = 0;
		for (uint8_t x = 0; x < fb->scrFontWth; x++) {				// For screen font bit width
			if (x % 8 == 0) b = *fp++;								// Load next font character
			if ((b & 0x80) != 0) video_wr_ptr[x] = fc565;			// Write forground colour
				else video_wr_ptr[x] = bc565;						// Write background colour
			b <<= 1;												// Roll font bits left
		}
		video_wr_ptr += fb->pitch / 2;								// Next line down **Ptr addition so div 2
	}
}

/*-[INTERNAL: TransparentWriteChar16]---------------------------------------}
. 16 Bit colour version of the transparent text character draw. The given
. character is drawn at the current position in the text colour but leaves
. background colours of bitmap in tact.
.--------------------------------------------------------------------------*/
static void TransparentWriteChar16 (struct framebuffer* fb, uint32_t x, uint32_t y, uint8_t Ch, uint32_t fgColor)
{
	volatile RGB565* __attribute__((aligned(2))) video_wr_ptr = (RGB565*)(fb->address + (y * fb->pitch) + (x * 2));
	volatile uint8_t* fp = (uint8_t*)(uint32_t)(&fb->scrFontPtr[(uint32_t)Ch * fb->scrFontHt * fb->scrFontByteWth]);
	RGBA fc = (RGBA)fgColor;										// Typecast the color to make conversion easy
	RGB565 fc565;
	fc565.rgb565R = fc.rgbRed >> 3;									// Place red bits
	fc565.rgb565G = fc.rgbGreen >> 2;								// Place green bits
	fc565.rgb565B = fc.rgbBlue >> 3;								// Place blue bits
	for (uint32_t y = 0; y < fb->scrFontHt; y++) {					// For screen font height
		uint8_t b = 0;
		for (uint8_t x = 0; x < fb->scrFontWth; x++) {				// For screen font bit width
			if (x % 8 == 0) b = *fp++;								// Load next font character
			if ((b & 0x80) != 0) video_wr_ptr[x] = fc565;			// Write forground colour if set, leave if not set
			b <<= 1;												// Roll font bits left
		}
		video_wr_ptr += fb->pitch / 2;								// Next line down **Ptr addition so div 2
	}
}


/* screenInit(): Calls framebufferInit() several times to ensure we successfully initialize, just in case. */
xinu_devcall screenInit (device* devptr)
{
	int status = SYSERR;
	char buffer[256];
	/* First check platform init connected out special VC mailbox message function pointer */
	if (fbtab[0].rpi_mailbox != 0)
	{
		// Request data for screen physical default width/height
		uint32_t msg1[8] __attribute__((aligned(16))) = {
			sizeof(msg1), 0, 0x00040003, 0x8, 0x8, 0, 0, 0
		};

		/* We are good to ask the VC for the default screen width/height */
		if (fbtab[0].rpi_mailbox(8, (uint32_t)&msg1[0]) == OK)		// We got wth/ht settings via tag channel 8
		{
			fbtab[0].width_p = msg1[5];									// Set message physical width request
			fbtab[0].width_v = msg1[5];									// Set message virtual width request
			fbtab[0].height_p = msg1[6];								// Set message physical height request
			fbtab[0].height_v = msg1[6];								// Set message virtual height request
			fbtab[0].depth = 32;										// Set message color depth request
			fbtab[0].pitch = 0;											// Zero pitch
			fbtab[0].x = 0;												// Zero x offset
			fbtab[0].y = 0;												// Zero y offset
			fbtab[0].address = 0;										// Zero address
			fbtab[0].size = 0;											// Zero size

			/* That all set lets initialize the screen and check we got a framebuffer*/
			if ((fbtab[0].rpi_mailbox(1, ((uint32_t)&fbtab[0] | RPi_ARM_TO_GPU_Alias)) == OK)
				&& (fbtab[0].address != 0))
			{
				/* Ok screen is up we have a frame buffer */
				// Convert framebuffer address it given to us as VC
				fbtab[0].address &= ~RPi_ARM_TO_GPU_Alias;				// Ok framebuffer now in ARM format

				/* Now set the function pointers depending on colour depth */
				switch (fbtab[0].depth)
				{
				case 32:		/* 32 bit colour screen mode */
					fbtab[0].SetPixel = SetPixel32;						// Set function ptr to 32bit colour version of SetPixel
					fbtab[0].ClearArea = ClearArea32;					// Set function ptr to 32bit colour version of clear area
					fbtab[0].VertLine = VertLine32;						// Set function ptr to 32bit colour version of vertical line
					fbtab[0].HorzLine = HorzLine32;						// Set function ptr to 32bit colour version of horizontal line
					fbtab[0].DiagLine = DiagLine32;						// Set function ptr to 32bit colour version of diagonal line
					fbtab[0].WriteChar = WriteChar32;					// Set function ptr to 32bit colour version of write character
					fbtab[0].TransparentWriteChar = TransparentWriteChar32;// Set function ptr to 32bit colour version of transparent write character
					break;
				case 24:		/* 24 bit colour screen mode */
					fbtab[0].SetPixel = SetPixel24;						// Set function ptr to 24bit colour version of SetPixel
					fbtab[0].ClearArea = ClearArea24;					// Set function ptr to 24bit colour version of clear area
					fbtab[0].VertLine = VertLine24;						// Set function ptr to 24bit colour version of vertical line
					fbtab[0].HorzLine = HorzLine24;						// Set function ptr to 24bit colour version of horizontal line
					fbtab[0].DiagLine = DiagLine24;						// Set function ptr to 24bit colour version of diagonal line
					fbtab[0].WriteChar = WriteChar24;					// Set function ptr to 24bit colour version of write character
					fbtab[0].TransparentWriteChar = TransparentWriteChar24;// Set function ptr to 24bit colour version of transparent write character
					break;
				case 16:		/* 16 bit colour screen mode */
					fbtab[0].SetPixel = SetPixel16;						// Set function ptr to 16bit colour version of SetPixel
					fbtab[0].ClearArea = ClearArea16;					// Set function ptr to 16bit colour version of clear area
					fbtab[0].VertLine = VertLine16;						// Set function ptr to 16bit colour version of vertical line
					fbtab[0].HorzLine = HorzLine16;						// Set function ptr to 16bit colour version of horizontal line
					fbtab[0].DiagLine = DiagLine16;						// Set function ptr to 16bit colour version of diagonal line
					fbtab[0].WriteChar = WriteChar16;					// Set function ptr to 16bit colour version of write character
					fbtab[0].TransparentWriteChar = TransparentWriteChar16; // Set function ptr to 16bit colour version of transparent write character
					break;
				}
				status = OK;

				// Setup default screen font
				fbtab[0].scrFontByteWth = 1;							// Current screen font byte width
				fbtab[0].scrFontWth = 8;								// Current screen font width (fonts do not have to be even bytes wide)
				fbtab[0].scrFontHt = 12;								// Current screen font height
				fbtab[0].scrFontPtr = (uint8_t*)&font_8x12[0];			// Current screen font byte data

				/* Initialize global variables */
				rows = fbtab[0].height_p / fbtab[0].scrFontHt;
				cols = fbtab[0].width_p / fbtab[0].scrFontWth;
				cursor_row = 0;
				cursor_col = 0;
				background = BLACK;
				foreground = WHITE;
				minishell = FALSE;

				// clear the screen to the background color.
				screenClear(background);
				screen_initialized = TRUE;
				sprintf(&buffer[0], "Screen resolution %d x %d Colour Depth: %d Line Pitch: %d",
					(int)fbtab[0].width_p, (int)fbtab[0].height_p, 
					(int)fbtab[0].depth, (int)fbtab[0].pitch);
				TransparentTextOutXY(0, 0, &buffer[0], strlen(&buffer[0]));
				sprintf(&buffer[0], "SmartStart v%x.%x%x, ARM%d AARCH%d code, CPU: %#03X, Cores: %u FPU: %s",
					(unsigned int)(RPi_SmartStartVer.HiVersion), (unsigned int)(RPi_SmartStartVer.LoVersion >> 8), (unsigned int)(RPi_SmartStartVer.LoVersion & 0xFF),
					(unsigned int)RPi_CompileMode.ArmCodeTarget, (unsigned int)RPi_CompileMode.AArchMode * 32 + 32,
					(unsigned int)RPi_CpuId.PartNumber, (unsigned int)RPi_CoresReady, (RPi_CompileMode.HardFloats == 1) ? "HARD" : "SOFT");
				TransparentTextOutXY(0, fbtab[0].scrFontHt, &buffer[0], strlen(&buffer[0]));
				sprintf(&buffer[0], "Enumerating USB ... may take a few seconds!");
				TransparentTextOutXY(0, 2*fbtab[0].scrFontHt, &buffer[0], strlen(&buffer[0]));
				cursor_row = 2;
			}
		}
	}
	return status;
}


/* Very heavy handed clearing of the screen to a single color. */
void screenClear (uint32_t color) {
	if (fbtab[0].ClearArea) fbtab[0].ClearArea(&fbtab[0], 0, 0, fbtab[0].width_p, fbtab[0].height_p, color);
}

/* Clear the minishell window */
void minishellClear(uint32_t color) 
{
	if (fbtab[0].ClearArea) fbtab[0].ClearArea(&fbtab[0], 0, MINISHELLMINROW * fbtab[0].scrFontHt, fbtab[0].width_p, fbtab[0].height_p, color);
}

void ClearArea (uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color)
{
	if (screen_initialized && fbtab[0].ClearArea)
	{
		fbtab[0].ClearArea(&fbtab[0], x1, y1, x2, y2, color);
	}
}

void TransparentTextOutXY (uint32_t x, uint32_t y, const char* buffer, uint32_t count)
{

	if (screen_initialized && (fbtab[0].TransparentWriteChar != 0) && buffer && count)
	{
		for (int i = 0; i < count; i++) {
			fbtab[0].TransparentWriteChar(&fbtab[0], x + fbtab[0].scrFontWth*i, y, buffer[i], foreground);
		}
	}
}

void TextOutXY (uint32_t x, uint32_t y, const char* buffer, uint32_t count)
{

	if (screen_initialized && (fbtab[0].WriteChar != 0) && buffer && count)
	{
		for (int i = 0; i < count; i++) {
			fbtab[0].WriteChar(&fbtab[0], x + fbtab[0].scrFontWth*i, y, buffer[i], foreground, background);
		}
	}
}

