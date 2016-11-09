/*
 * ST7565 LCD library
 * Copyright (C) 2010 Limor Fried, Adafruit Industries
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stlcd.h>
#include <font.h>
#include "e_meter.h"
#include "bitmaps.h"

uint8_t is_reversed = 0;
uint8_t lcd_buffer[1024]=STRIJ_LOGO;

int pagemap[] = { 3, 2, 1, 0, 7, 6, 5, 4 };

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

/*****************************************************************************
 *
 *****************************************************************************/



void lcd_clear (bool all)
{
/*	uint16_t c;

	for (uint8_t p = 0; p < 8; p++) {
		st7565_command (CMD_SET_PAGE | p);
		for (c = 0; c < 129; c++) {
			st7565_command (CMD_SET_COLUMN_LOWER | (c & 0xf));
			st7565_command (CMD_SET_COLUMN_UPPER | ((c >> 4) & 0xf));
			st7565_data (0x0);
		}
	}*/
	display_clear_buffer (all);
}

/*****************************************************************************
 *
 *****************************************************************************/
void st7565_init (void)
{
//	set pin directions
  	CMU_ClockEnable (cmuClock_GPIO, true);

	GPIO_PinModeSet (ST765R_GPIO_CS_PORT,	ST765R_GPIO_CS, 	gpioModePushPull, 1);	// CS0  - ADC0
	GPIO_PinModeSet (ST765R_GPIO_CLK_PORT,	ST765R_GPIO_CLK, 	gpioModePushPull, 1);	//      - IO1
	GPIO_PinModeSet (ST765R_GPIO_A0_PORT,	ST765R_GPIO_A0, 	gpioModePushPull, 0);	//      - SDA
	GPIO_PinModeSet (ST765R_GPIO_MOSI_PORT,	ST765R_GPIO_MOSI, 	gpioModePushPull, 0);	//      - TIM2
//	GPIO_PinModeSet (ST765R_GPIO_RST_PORT,	ST765R_GPIO_RST, 	gpioModePushPull, 0);

	// toggle RST low to reset; CS low so it'll listen to us
	DelayNOP (500000);
//	GPIO_PinOutSet (ST765R_GPIO_RST_PORT, ST765R_GPIO_RST);
	ext_ctrl (PE_LCD_RES, PE_OFF);
	DelayNOP (500);
	ext_ctrl (PE_LCD_RES, PE_ON);
	DelayNOP (500);
	// LCD bias select
	st7565_command (CMD_SET_BIAS_7);
	// ADC select
	st7565_command (CMD_SET_ADC_NORMAL);
	// SHL select
	st7565_command (CMD_SET_COM_NORMAL);
	// Initial display line
	st7565_command (CMD_SET_DISP_START_LINE);

	// turn on voltage converter (VC=1, VR=0, VF=0)
	st7565_command (CMD_SET_POWER_CONTROL | 0x4);
	// wait for 50% rising
	DelayNOP (50);

	// turn on voltage regulator (VC=1, VR=1, VF=0)
	st7565_command (CMD_SET_POWER_CONTROL | 0x6);
	// wait >=50ms
	DelayNOP (50);

	// turn on voltage follower (VC=1, VR=1, VF=1)
	st7565_command (CMD_SET_POWER_CONTROL | 0x7);
	// wait
	DelayNOP (10);

	// set lcd operating voltage (regulator resistor, ref voltage resistor)
	st7565_command (CMD_SET_RESISTOR_RATIO | 0x6);
}

#define PAUSE() asm volatile("nop");//for(int p=0; p<10; p++)

/*****************************************************************************
 *
 *****************************************************************************/
inline void spiwrite (uint8_t c)
{
//	GPIO_DbgSWDClkEnable (false);
	GPIO_PinModeSet (ST765R_GPIO_CLK_PORT, ST765R_GPIO_CLK, gpioModePushPull, 1);
	GPIO_PinOutClear (ST765R_GPIO_CS_PORT, ST765R_GPIO_CS);
	PAUSE ();
	int8_t i;
	for (i=7; i>=0; i--)
	{
		if (c & _BV(i))
			GPIO_PinOutSet (ST765R_GPIO_MOSI_PORT, ST765R_GPIO_MOSI); //SID_PORT |= _BV(SID);
		else
			GPIO_PinOutClear (ST765R_GPIO_MOSI_PORT, ST765R_GPIO_MOSI); //SID_PORT &= ~_BV(SID);
		PAUSE ();
		GPIO_PinOutClear (ST765R_GPIO_CLK_PORT, ST765R_GPIO_CLK); //SCLK_PORT &= ~_BV(SCLK);
		PAUSE ();
		GPIO_PinOutSet (ST765R_GPIO_CLK_PORT, ST765R_GPIO_CLK); //SCLK_PORT |= _BV(SCLK);
		PAUSE ();
	}
	GPIO_PinOutSet (ST765R_GPIO_CS_PORT, ST765R_GPIO_CS);
//	GPIO_DbgSWDClkEnable (true);
	PAUSE ();
}

/*****************************************************************************
 *
 *****************************************************************************/
void st7565_command (uint8_t c)
{
  GPIO_PinOutClear (ST765R_GPIO_A0_PORT, ST765R_GPIO_A0); //A0_PORT &= ~_BV(A0);
  spiwrite (c);
}

/*****************************************************************************
 *
 *****************************************************************************/
void st7565_a0(uint8_t c)
{
      if (c)
      GPIO_PinOutSet (ST765R_GPIO_A0_PORT, ST765R_GPIO_A0);
    else
      GPIO_PinOutClear (ST765R_GPIO_A0_PORT, ST765R_GPIO_A0);
}

void st7565_data (uint8_t c)
{
  GPIO_PinOutSet (ST765R_GPIO_A0_PORT, ST765R_GPIO_A0); //A0_PORT |= _BV(A0);
  spiwrite (c);
}

/*****************************************************************************
 *
 *****************************************************************************/
void st7565_set_brightness (uint8_t val)
{
    st7565_command (CMD_SET_VOLUME_FIRST);
    st7565_command (CMD_SET_VOLUME_SECOND | (val & 0x3f));
}

/*****************************************************************************
 *
 *****************************************************************************/
void __display_write_buffer (void)
{
  uint8_t c, p;

  for (p = 0; p < 8; p++) {
    st7565_command (CMD_SET_PAGE | pagemap[p]);
    st7565_command (CMD_SET_COLUMN_LOWER | (0x0 & 0xf));
    st7565_command (CMD_SET_COLUMN_UPPER | ((0x0 >> 4) & 0xf));
    st7565_command (CMD_RMW);
    st7565_data (0x00);

    for (c=0; c <128; c++) {
      st7565_data (lcd_buffer[(128*p)+c]);
    }
  }
}
void drawchar (uint8_t x, uint8_t y, uint8_t ch)
{
	#if (FONTSIZE ==_5x8PCX)
	for (uint8_t i =0; i<5; i++ ) {
		lcd_buffer[x + (y*128)] = font5x8[(ch*5)+i];
		x++;
	}
	#elif (FONTSIZE ==_8x16PCX)
	uint16_t i=c*16;
	y *= 2;
	for (uint8_t cx=0; cx <8; cx++) {
		lcd_buffer[x + (y*128)]     = font[i]; i++;
		lcd_buffer[x + ((y+1)*128)] = font[i]; i++;
		x++;
	}
	#endif
}

void display_text (char *str, uint16_t x, uint16_t y)
{
	x = x * FONTSIZE_X /*+ strlen(str)*/;
	if (y <(LCDHEIGHT/FONTSIZE_Y)) {											// out of Y-space
		while (*str) {
			drawchar (x, y, *str++);
			x += FONTSIZE_X+1;
			if (x + (FONTSIZE_X+1) >=LCDWIDTH)
				x = LCDWIDTH - FONTSIZE_X;										// out of X-space
		}
	}
}

/*****************************************************************************
 *
 *****************************************************************************/
