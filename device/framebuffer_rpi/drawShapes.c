/**
 * @file drawShapes.c
 *
 * Allows shapes to be rendered onscreen.
 */
/* Embedded Xinu, Copyright (C) 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <string.h>
#include <framebuffer.h>

/* Draws a colored pixel at given (x, y) coordinates. */

void drawPixel(int x, int y, uint32_t color)
{
	if (fbtab[0].SetPixel) fbtab[0].SetPixel(&fbtab[0], x, y, color);
}


void drawLine (int x1, int y1, int x2, int y2, uint32_t color)
{
	if (x1 == x2) {
		if (fbtab[0].HorzLine) fbtab[0].HorzLine(&fbtab[0], x1, y1, x2 - x1, color);
	} if (y1 == y2) {
		if (fbtab[0].VertLine) fbtab[0].VertLine(&fbtab[0], x1, y1, y2 - y1, color);
	}
	else if (fbtab[0].DiagLine) fbtab[0].DiagLine(&fbtab[0], x1, y1, x2, y2, color);
}

/* Modified drawLine specifically for turtle graphics */
void drawSegment(int x1, int y1, int x2, int y2, unsigned long color)
{
    int deltax, stepx, deltay, stepy, error, i;
    int maxy = fbtab[0].height_p - (fbtab[0].scrFontHt * MINISHELLMINROW) - 1;
	if (x2 > x1) {
		deltax = x2 - x1;
		stepx = 1;
	} else {
		deltax = x1 - x2;
		stepx = -1;
	}
	if (y2 > y1) {
		deltay = y2 - y1;
		stepy = 1;
	} else {
		deltay = y1 - y2;
		stepy = -1;
	}
	error = deltax - deltay;
	while ( (x1 != x2 + stepx) && (y1 != y2 + stepy) ) {
        //error checking
        if ( (y1 < maxy) && (y1 >= 0) && (x1 < fbtab[0].width_p) && (x1 >= 0) ) {
		    drawPixel(x1, y1, color);
            i = y1 * fbtab[0].width_p + x1;
            linemap[i] = color; //add it to the linemap.
        }
		if (error * 2 >= -1 * deltay) {
			x1 = x1 + stepx;
			error = error - deltay;
		}
		if (error * 2 <= deltax) {
			y1 = y1 + stepy;
			error = error + deltax;
		}
	}
}

void drawRect(int x1, int y1, int x2, int y2, unsigned long color) {
	drawLine(x1, y1, x2, y1, color);
	drawLine(x1, y2, x2, y2, color);
	drawLine(x2, y1, x2, y2, color);
	drawLine(x1, y1, x1, y2, color);
}

void fillRect(int x1, int y1, int x2, int y2, unsigned long color, bool gradient) {
	int iterations;
	if ( (x2 - x1) > (y2 - y1) )
		iterations = (x2 - x1) / 2;
	else iterations = (y2 - y1) / 2;

	while (iterations != 0) {
		drawRect(x1 + iterations, y1 + iterations, x2 - iterations, y2 - iterations, color);
		iterations--;
		if (gradient) color += 2;
	}
	drawRect(x1, y1, x2, y2, color);
}

/* Midpoint Circle Algorithm calculation of a circle.
 * Based on "x^2 + y^2 = r^2" */

void drawCircle (int x0, int y0, int radius, unsigned long color)
{
    int x = radius, y = 0;
  	int radiusError = 1 - x;

  	while(x >= y)
  	{
    	drawPixel(x + x0, y + y0, color);
    	drawPixel(y + x0, x + y0, color);
    	drawPixel(-x + x0, y + y0, color);
    	drawPixel(-y + x0, x + y0, color);
    	drawPixel(-x + x0, -y + y0, color);
    	drawPixel(-y + x0, -x + y0, color);
    	drawPixel(x + x0, -y + y0, color);
    	drawPixel(y + x0, -x + y0, color);
    	y++;
        if(radiusError < 0)
           	radiusError += 2 * y + 1;
       	else
       	{
           	x--;
           	radiusError += 2 * (y - x + 1);
       	}
  	}
}

void fillCircle (int x0, int y0, int radius, unsigned long color, bool gradient) {
	int rad = radius;
	while (rad != 0) {
		drawCircle(x0, y0, rad, color);
		rad--;
		if (gradient) color++;
	}
}

/* Turtle graphics specific circle functions */
void drawBody (int x0, int y0, int radius, unsigned long color)
{
    int x = radius, y = 0;
  	int radiusError = 1 - x;

    //the maximum y-value we can have so that we don't draw any part of turtle in the minishell area
    int maxy = fbtab[0].height_p - (MINISHELLMINROW * fbtab[0].scrFontHt) - BODY_RADIUS - (HEAD_RADIUS * 2);
    //don't even try to draw turtle if y is out of bounds
    if ((y0 > maxy) || (y0 < 0) || (x0 > fbtab[0].width_p) || (x0 < 0)) {
        return;
    }

  	while(x >= y)
  	{
       	    drawPixel(x + x0, y + y0, color);
       	    drawPixel(y + x0, x + y0, color);
       	    drawPixel(-x + x0, y + y0, color);
       	    drawPixel(-y + x0, x + y0, color);
       	    drawPixel(-x + x0, -y + y0, color);
       	    drawPixel(-y + x0, -x + y0, color);
       	    drawPixel(x + x0, -y + y0, color);
       	    drawPixel(y + x0, -x + y0, color);
    		y++;
            if(radiusError < 0)
               	radiusError += 2 * y + 1;
        	else
        	{
              	x--;
               	radiusError += 2 * (y - x + 1);
        	}
  	}
}

void fillBody (int x0, int y0, int radius, unsigned long color, bool gradient) {
	int rad = radius;
	while (rad != 0) {
		drawBody(x0, y0, rad, color);
		rad--;
		if (gradient) color++;
	}
}

void drawChar(char c, int x, int y, uint32_t color)
{
	if (fbtab[0].TransparentWriteChar)
		fbtab[0].TransparentWriteChar(&fbtab[0], x, y, c, color);
}

