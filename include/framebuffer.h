/**
 * @file framebuffer.h
 * Constants and declarations associated with the framebuffer system.
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include <stdbool.h>
#include <xinu.h>
#include <device.h>
#include <stdint.h>

/* Some basic colors. 8 bits transparency, 8 bits red, 8 bits green, 8 bits blue.*/
#define WHITE		0xFFFFFFFF
#define RED			0xFFFF0000
#define GREEN		0xFF00FF00
#define BLUE		0xFF0000FF
#define BLACK		0xFF000000
#define GRAY		0xFF808080
#define SILVER		0xFFC0C0C0
#define YELLOW		0xFFFFFF00
#define CYAN		0xFF00FFFF
#define MAGENTA		0xFFFF00FF
#define INDIGO		0xFF2E0854
#define DARKBLUE	0xFF0000A0
#define ORANGE		0xFFFFA500
#define BROWN		0xFF6F4E37
#define RASPBERRY	0xFFE30B5C
#define LEAFGREEN	0xFF009900
#define DARKGREEN	0xFF254117
#define PURPLE		0xFF4B0082
#define BLUEIVY		0xFF3090C7
#define PINK		0xFFFF0090

#define ERRORCOLOR	0x00000000

struct defaultcolor {
    char *colorname;
    unsigned long colornum;
};

/* Turtle constants */
#define TURTLE_BODY 0xFF2C7C34 
#define TURTLE_HEAD 0xFF808D43 
#define BODY_RADIUS 25
#define HEAD_RADIUS 8

#define PI 3.14159

/* Top of struct is the same as GPU uses. */
/*  Must be packed for use on 64 BIT      */
/*  Must be 32bits on 64 bit compile      */
struct __attribute__((__packed__)) framebuffer {
	uint32_t width_p;	/* requested width of physical display */
	uint32_t height_p;	/* requested height of physical display */
	uint32_t width_v;	/* requested width of virtual display */
	uint32_t height_v;	/* requested height of virtual display */
	uint32_t pitch;		/* zero upon request; in response: # bytes between rows */
	uint32_t depth;		/* requested depth in bits per pixel */
	uint32_t x;			/* requested x offset of virtual framebuffer */
	uint32_t y;			/* requested y offset of virtual framebuffer */
	uint32_t address;	/* framebuffer address. Zero upon request; failure if zero upon response. */
	uint32_t size;		/* framebuffer size. Zero upon request.  */

	/* SCREEN FONT DATA */
	uint8_t scrFontByteWth;	// Current screen font byte width
	uint8_t scrFontWth;		// Current screen font width in pixels
	uint8_t scrFontHt;      // Current screen font height in pixels
	uint8_t* scrFontPtr;	// Current screen font byte data

	/* FUNCTION POINTER TO VC MAILBOX .... PlatformInit.c must set this */
	int (*rpi_mailbox) (uint32_t channel, uint32_t msg);

	/* NOW A SET OF FUNCTION POINTERS THAT VARY WITH COLOUR DEPTH SET*/
	/* INTERNALLY USED BUT EASIER TO KEEP ALL DATA TOGETHER */
	void (*SetPixel) (struct framebuffer* fb, uint32_t x, uint32_t y, uint32_t crColor);
	void (*ClearArea) (struct framebuffer* fb, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t crColor);
	void (*VertLine) (struct framebuffer* fb, uint32_t x, uint32_t y, int32_t cy, uint32_t crColor);
	void (*HorzLine) (struct framebuffer* fb, uint32_t x, uint32_t y, int32_t cx, uint32_t crColor);
	void (*DiagLine) (struct framebuffer* fb, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t crColor);
	void (*WriteChar) (struct framebuffer* fb, uint32_t x, uint32_t y, uint8_t Ch, uint32_t fgColor, uint32_t bgColor);
	void (*TransparentWriteChar) (struct framebuffer* fb, uint32_t x, uint32_t y, uint8_t Ch, uint32_t fgColor);
};

extern struct framebuffer fbtab[];
extern unsigned long foreground;
extern unsigned long background;
extern int rows;
extern int cols;
extern int cursor_col; 
extern int cursor_row;
extern bool minishell;

#define MINISHELLMINROW 16 /* When running in minishell mode, only use the bottom fifteen lines of the screen */

// include a "line memory" array that remembers all drawn lines
// currently used by the xsh_turtle function.
// each entry in linemap maps to y * width + x
#define MAPSIZE 2048 * 2048
extern unsigned long linemap[];

/* Turtle command structs and definitions */
#define COMMANDNAMELENGTH 10    /* max length of procedure name */
#define COMMANDLENGTH 100       /* max length of new procedure */
#define MAXNEWCOMMANDS 10       /* cannot add more than 10 new commands at a time */

struct defaultcommand {
    char commandname[COMMANDNAMELENGTH];
    void (*command) (char*);
}; 

struct newcommand {
    char name[COMMANDNAMELENGTH];
    char text[COMMANDLENGTH];
};
extern struct newcommand newcommandtab[];

/* driver functions */
xinu_devcall fbWrite(device *, const void*, unsigned int);
xinu_devcall fbPutc(device *, char);
xinu_syscall fbprintf(char *fmt, ...);

/* other function prototypes */
xinu_devcall screenInit (device *devptr);
void ClearArea (uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color);
void TransparentTextOutXY (uint32_t x, uint32_t y, const char* buffer, uint32_t count);



void screenClear (uint32_t color);
void minishellClear (uint32_t color);
void viewlinemap(void); 
void drawPixel (int, int, uint32_t);
void drawLine (int, int, int, int, uint32_t);
void initlinemap(void);
void drawSegment(int, int, int, int, unsigned long);
void drawBody(int, int, int, unsigned long);
void fillBody(int, int, int, unsigned long, bool);
void drawChar(char, int, int, uint32_t);
void drawRect(int, int, int, int, unsigned long);
void fillRect(int, int, int, int, unsigned long, bool);
void drawCircle(int, int, int, unsigned long);
void fillCircle(int, int, int, unsigned long, bool);
double sin(int);
double cos(int);

#endif /* _FRAMEBUFFER_H_*/
