/**
 * @file fbPutc.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <stdbool.h>
#include <xinu.h>
#include <stddef.h>
#include <framebuffer.h>
#include <device.h>
#include <string.h>

#include <stdio.h>

extern int rows;
extern int cols;
extern int cursor_col;
extern int cursor_row;
extern unsigned long foreground;
extern unsigned long background;
extern bool screen_initialized;


typedef enum {
	VTPARSE_STATE_ESCAPE = 0,		// Parsing for escape
	VTPARSE_STATE_BRACKET,			// ESC found, parsing for bracket
	VTPARSE_STATE_CSI,				// CSI found, parsing for control sequences
	VTPARSE_STATE_BGCOLOR,			// Background colour control found, parsing colour
	VTPARSE_STATE_FGCOLOR,			// Foreground colour control found, parsing colour
	VTPARSE_STATE_CONTROL,			// CSI control code, parse which control code
	VTPARSE_TAB_CONTROL,			// CSI control code, parse which tab code
	VTPARSE_STATE_EQUALS,			// CSI equal state found, parse equal value
	VTPARSE_STATE_ENDVAL,			// End parse
} vtparse_state_t;


struct vtparse_state {
	vtparse_state_t state;
	unsigned char style;
	unsigned char next_style;
	unsigned short tab;
};

struct color_char {
	unsigned char style;
	unsigned char ascii;
};

#define ESC '\x1B'

static struct vtparse_state term_state =
{
	.state = VTPARSE_STATE_ESCAPE,
	.style = 0x0F,
	.next_style = 0x0F,
	.tab = 0,
};

static inline unsigned char convert_color (unsigned char color)
{
	const unsigned char lookup_table[8] = { 0, 4, 2, 6, 1, 5, 3, 7 };
	return lookup_table[(int)color];
}

struct color_char vt100_parse (struct vtparse_state *state, char x)
{
	struct color_char rv = {
		.style = state->style,
		.ascii = '\0'
	};
	switch (state->state) {
	case VTPARSE_STATE_ESCAPE:
		if (x == ESC) 
		{
			state->state = VTPARSE_STATE_BRACKET;
		}
		else {
			rv.ascii = x;
		}
		break;
	case VTPARSE_STATE_BRACKET:
		if (x == '[')
			state->state = VTPARSE_STATE_CSI;
		else {
			state->state = VTPARSE_STATE_ESCAPE;
			rv.ascii = x;
		}
		break;
	case VTPARSE_STATE_CSI:
		switch (x) {
			case '0':
				state->state = VTPARSE_STATE_ENDVAL;
				state->next_style = 0x0F;
				break;
			case '1':
				state->state = VTPARSE_STATE_ENDVAL;
				state->next_style |= (1 << 3);
				break;
			case '2':
				state->state = VTPARSE_STATE_CONTROL;
				break;
			case '3':
				state->state = VTPARSE_STATE_FGCOLOR;
				break;
			case '4':
				state->state = VTPARSE_STATE_BGCOLOR;
				break;
			case '=':
				state->state = VTPARSE_STATE_EQUALS;
				break;
			case 'H':
				state->state = VTPARSE_STATE_ESCAPE;
				break;
			case '[':
				state->state = VTPARSE_TAB_CONTROL;
				state->tab = 0;
				break;
			default:
				if (x >= '5' && x <= '9')
				{
					state->tab = (x - '0');
					state->state = VTPARSE_TAB_CONTROL;
				} 
				else {
					state->state = VTPARSE_STATE_ESCAPE;
				}
				break;
		}
		break;
	case VTPARSE_STATE_BGCOLOR:
		if (x >= '0' && x <= '7') {
			state->state = VTPARSE_STATE_ENDVAL;
			state->next_style &= 0x1F;
			state->next_style |= convert_color(x - '0') << 4;
		}
		else {
			state->state = VTPARSE_STATE_ESCAPE;
			state->next_style = state->style;
			rv.ascii = x;
		}
		break;
	case VTPARSE_TAB_CONTROL :
		if (x >= '0' && x <= '9') 
		{
			state->tab *= 10;
			state->tab += (x - '0');
		}
		else if (x == ']')
		{
			/* do nothing */
		} else if (x == 'G')
		{
			state->state = VTPARSE_STATE_ESCAPE;
			cursor_col = state->tab;
		}
		else 
		{
			state->state = VTPARSE_STATE_ESCAPE;
		}
		break;
	case VTPARSE_STATE_FGCOLOR:
		if (x >= '0' && x <= '7') {
			state->state = VTPARSE_STATE_ENDVAL;
			state->next_style &= 0xF8;
			state->next_style |= convert_color(x - '0');
		}
		else if (x == '9') {
			state->state = VTPARSE_STATE_ENDVAL;
			state->style = 0x0F;
			state->next_style = 0x0F;
		}
		else {
			state->state = VTPARSE_STATE_ESCAPE;
			state->next_style = state->style;
			rv.ascii = x;
		}
		break;
	case VTPARSE_STATE_CONTROL:
		switch (x) {
			case 'J':
				screenClear(background);
				cursor_row = 0;
				cursor_col = 0;
				break;
			case 'K':
				cursor_col = 0;
				ClearArea(0, cursor_row*fbtab[0].scrFontHt, fbtab[0].width_p, (cursor_row+1)*fbtab[0].scrFontHt, background);
				break;
		}
		state->state = VTPARSE_STATE_ESCAPE;
		break;
	case VTPARSE_STATE_EQUALS:
		if (x == '1') {
			state->state = VTPARSE_STATE_ENDVAL;
			state->next_style &= ~(1 << 3);
		}
		else {
			state->state = VTPARSE_STATE_ESCAPE;
			state->next_style = state->style;
			rv.ascii = x;
		}
		break;
	case VTPARSE_STATE_ENDVAL:
		if (x == ';') {
			state->state = VTPARSE_STATE_CSI;
		}
		else if (x == 'm') {
			// Finish and apply styles
			state->state = VTPARSE_STATE_ESCAPE;
			state->style = state->next_style;
		}
		else {
			state->state = VTPARSE_STATE_ESCAPE;
			state->next_style = state->style;
			rv.ascii = x;
		}
		break;
	default:
		break;
	}
	return rv;
}

static uint32_t vt_color[16] = { BLACK , RASPBERRY, DARKGREEN, BROWN, DARKBLUE, PURPLE, BLUEIVY, GRAY, BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE};

xinu_devcall fbPutc(device *devptr, char ch)
{
	if (screen_initialized)
	{
		struct color_char rv = vt100_parse(&term_state, ch);
		if (term_state.state == VTPARSE_STATE_ESCAPE) {
			switch (rv.ascii)
			{
			case '\r':
				cursor_col = 0;
				term_state.tab = 0;
				break;
			case '\n':
				cursor_row++;
				cursor_col = 0;
				term_state.tab = 0;
				break;
			case '\t':
				cursor_col += 4;
				break;
			case 8:
				{
					uint32_t x1, y1;
					if (cursor_col > 0) cursor_col--;
					x1 = cursor_col * fbtab[0].scrFontWth;
					y1 = cursor_row * fbtab[0].scrFontHt;
					ClearArea(x1, y1, x1 + fbtab[0].scrFontWth, y1 + fbtab[0].scrFontHt, background);
				}
				break;
			case '\0':
				break;
			default:
				{
					drawChar(rv.ascii, cursor_col * fbtab[0].scrFontWth,
							cursor_row * fbtab[0].scrFontHt, vt_color[rv.style]);
					cursor_col++;
					if (cursor_col >= cols)
					{
						cursor_col = 0;
						cursor_row += 1;
					}
				}
			}
			if ((minishell == TRUE) && (cursor_row == rows))
			{
				minishellClear(background);
				cursor_row = rows - MINISHELLMINROW;
			}
			else if (cursor_row == rows)
			{
				screenClear(background);
				cursor_row = 0;
			}
		}

		return ch;
	}
	return SYSERR;
}

