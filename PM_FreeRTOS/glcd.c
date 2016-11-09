/*
 * ST7565 LCD library
 * Copyright (C) 2010 Limor Fried, Adafruit Industries
 */

#include <stdlib.h>
#include <string.h>
#include "glcd.h"


/*****************************************************************************
 * the most basic function, set a single pixel
 *****************************************************************************/
void setpixel (uint8_t x, uint8_t y, uint8_t color)
{
	if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
		return;

	// x is which column
	//if (color)
		lcd_buffer[x+ (y/8)*128] |= _BV(7-(y%8));
	//else
	//	lcd_buffer[x+ (y/8)*128] &= ~_BV(7-(y%8));
        //__display_write_buffer();
        
}
void set8pixel (uint8_t x, uint8_t y, uint8_t color, uint8_t dir)
{
        int i;
        if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
		return;
        i=dir;
        if (dir==0)
                for (i=0;i<8;i++)
          {
            lcd_buffer[x+ (y/8)*128] |= _BV(7-(y%8));
  y++;
}
        
	// x is which column
	//if (color)
		//lcd_buffer[x+ (y/8)*128] |= _BV(7-(y%8));
	//else
	//	lcd_buffer[x+ (y/8)*128] &= ~_BV(7-(y%8));
       // __display_write_buffer();
}
/*****************************************************************************
 * the most basic function, clear a single pixel
 *****************************************************************************/
void clearpixel (uint8_t *buff, uint8_t x, uint8_t y)
{
	// x is which column
	buff[x+ (y/8)*128] &= ~_BV(7-(y%8));
}

/*****************************************************************************
 *
 *****************************************************************************/
void drawbitmap (uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color)
{
	for (uint8_t j=0; j<h; j++) {
		for (uint8_t i=0; i<w; i++ ) {
			if (pgm_read_byte(bitmap + i + (j/8)*w) & _BV(j%8)) {
				setpixel (x+i, y+j, color);
			}
		}
	}
}

/*****************************************************************************
 *
 *****************************************************************************/
//void drawchar (uint8_t x, uint8_t y, uint8_t ch)
//{
//	#if (FONTSIZE ==_5x8PCX)
//	for (uint8_t i =0; i<5; i++ ) {
//		lcd_buffer[x + (y*128)] = font5x8[(ch*5)+i];
//		x++;
//	}
//	#elif (FONTSIZE ==_8x16PCX)
//	uint16_t i=c*16;
//	y *= 2;
//	for (uint8_t cx=0; cx <8; cx++) {
//		lcd_buffer[x + (y*128)]     = font[i]; i++;
//		lcd_buffer[x + ((y+1)*128)] = font[i]; i++;
//		x++;
//	}
//	#endif
//}

/*****************************************************************************
 *
 *****************************************************************************/
//void display_text (char *str, uint16_t x, uint16_t y)
//{
//	x = x * FONTSIZE_X /*+ strlen(str)*/;
//	if (y <(LCDHEIGHT/FONTSIZE_Y)) {											// out of Y-space
//		while (*str) {
//			drawchar (x, y, *str++);
//			x += FONTSIZE_X+1;
//			if (x + (FONTSIZE_X+1) >=LCDWIDTH)
//				x = LCDWIDTH - FONTSIZE_X;										// out of X-space
//		}
//	}
//}

/*****************************************************************************
 * bresenham's algorithm - the wikipedia
 *****************************************************************************/
void drawline (uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color)
{
	if (y0 ==y1) {
		for (uint8_t x=x0; x<=x1; x++)
			setpixel (x, y0, color);
	}
	else
	if (x0 ==x1) {
		for (uint8_t y=y0; y<=y1; y++)
			setpixel (x0, y, color);
	}
#if (0)
	else {
		uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
		if (steep) {
			swap (x0, y0);
			swap (x1, y1);
		}
		if (x0 >x1) {
			swap (x0, x1);
			swap (y0, y1);
		}
		uint8_t dx = x1 - x0;
		uint8_t dy = abs (y1 - y0);
		int8_t err = dx / 2;
		int8_t ystep;

		if (y0 <y1)
			ystep = 1;
		else
			ystep = -1;

		for ( ; x0 <x1; x0++) {
			if (steep)
				setpixel (y0, x0, color);
			else
				setpixel (x0, y0, color);
			err -= dy;
			if (err <0) {
				y0  += ystep;
				err += dx;
			}
		}
	}
#endif
}

/*****************************************************************************
 * filled rectangle
 *****************************************************************************/
void fillrect (uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
	// stupidest version - just pixels - but fast with internal buffer!
	for (uint8_t i=x; i <x+w; i++) {
		for (uint8_t j=y; j <y+h; j++) {
			setpixel (i, j, color);
		}
	}
}

/*****************************************************************************
 * draw a rectangle
 *****************************************************************************/
void drawrect (uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
	// stupidest version - just pixels - but fast with internal buffer!
	for (uint8_t i=x; i <x+w; i++) {
		setpixel (i, y, color);
		setpixel (i, y+h-1, color);
	}
	for (uint8_t i=y; i<y+h; i++) {
		setpixel (x, i, color);
		setpixel (x+w-1, i, color);
	}
}

/*****************************************************************************
 * draw a circle
 *****************************************************************************/
void drawcircle (uint8_t *buff, uint8_t x0, uint8_t y0, uint8_t r, uint8_t color)
{
	int8_t f = 1 - r;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * r;
	int8_t x = 0;
	int8_t y = r;

	setpixel (x0, y0+r, color);
	setpixel (x0, y0-r, color);
	setpixel (x0+r, y0, color);
	setpixel (x0-r, y0, color);

	while (x <y) {
		if (f >=0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;

		setpixel (x0 + x, y0 + y, color);
		setpixel (x0 - x, y0 + y, color);
		setpixel (x0 + x, y0 - y, color);
		setpixel (x0 - x, y0 - y, color);

		setpixel (x0 + y, y0 + x, color);
		setpixel (x0 - y, y0 + x, color);
		setpixel (x0 + y, y0 - x, color);
		setpixel (x0 - y, y0 - x, color);
	}
}

/*****************************************************************************
 * draw a circle
 *****************************************************************************/
void fillcircle (uint8_t *buff, uint8_t x0, uint8_t y0, uint8_t r, uint8_t color)
{
	int8_t f = 1 - r;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * r;
	int8_t x = 0;
	int8_t y = r;

	for (uint8_t i=y0-r; i <=y0+r; i++)
		setpixel(x0, i, color);

	while (x <y) {
		if (f >=0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;

		for (uint8_t i=y0-y; i <=y0+y; i++) {
			setpixel (x0+x, i, color);
			setpixel (x0-x, i, color);
		}
		for (uint8_t i=y0-x; i <=y0+x; i++) {
			setpixel (x0+y, i, color);
			setpixel (x0-y, i, color);
		}
	}
}

/*****************************************************************************
 * clear everything
 *****************************************************************************/
void display_clear_buffer (bool all)
{
//	memset(buff, 0, 1024);
	for (int i =all?0:128; i <1024; i++)
	{
		lcd_buffer[i] = 0x00;
	}
}

/*****************************************************************************
 *
 *****************************************************************************/
