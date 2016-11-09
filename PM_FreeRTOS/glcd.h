#define LCDWIDTH 128
#define LCDHEIGHT 64

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_chip.h"
#include "unb.h"
#include "em_usart.h"

#define _BV(x) (1<<x)
#define pgm_read_byte(x) (*((uint8_t*)x))

#define swap(a, b) { uint8_t t = a; a = b; b = t; }

extern uint8_t lcd_buffer[];

void display_clear_buffer (bool);
void setpixel (uint8_t x, uint8_t y, uint8_t color);
void drawrect (uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void drawline (uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);
void drawbitmap (uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color);
void drawchar (uint8_t x, uint8_t line, uint8_t c);
void drawcircle (uint8_t *buff, uint8_t x0, uint8_t y0, uint8_t r, uint8_t color);
void fillrect (uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);

void display_text (char *str, uint16_t x, uint16_t line);
void display_number (char *num, uint16_t x, uint16_t y, uint8_t w, uint8_t h);
