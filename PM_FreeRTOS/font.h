// 5x7 LCD font 'flipped' for the ST7565 - public domain
#ifndef __GLCDFONT__
#define __GLCDFONT__

#include <stdint.h>

#define _5x8PCX		1
#define _8x16PCX	2
#define FONTSIZE 	_5x8PCX

#if (FONTSIZE ==_5x8PCX)
#define FONTSIZE_X	5
#define FONTSIZE_Y	8
const uint8_t font5x8[] = {
  0x00, 0x00, 0x00, 0x00, 0x00,		// Ascii(00)
  0x7C, 0xDA, 0xF2, 0xDA, 0x7C,		// ASCII(01)
  0x7C, 0xD6, 0xF2, 0xD6, 0x7C,		// ASCII(02)
  0x38, 0x7C, 0x3E, 0x7C, 0x38,
  0x18, 0x3C, 0x7E, 0x3C, 0x18,
  0x38, 0xEA, 0xBE, 0xEA, 0x38,
  0x38, 0x7A, 0xFE, 0x7A, 0x38,
  0x00, 0x18, 0x3C, 0x18, 0x00,
  0xFF, 0xE7, 0xC3, 0xE7, 0xFF,
  0x00, 0x18, 0x24, 0x18, 0x00,
  0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
  0x0C, 0x12, 0x5C, 0x60, 0x70,
  0x64, 0x94, 0x9E, 0x94, 0x64,
  0x02, 0xFE, 0xA0, 0xA0, 0xE0,
  0x02, 0xFE, 0xA0, 0xA4, 0xFC,
  0x5A, 0x3C, 0xE7, 0x3C, 0x5A,
  0xFE, 0x7C, 0x38, 0x38, 0x10,
  0x10, 0x38, 0x38, 0x7C, 0xFE,
  0x28, 0x44, 0xFE, 0x44, 0x28,
  0xFA, 0xFA, 0x00, 0xFA, 0xFA,
  0x60, 0x90, 0xFE, 0x80, 0xFE,
  0x00, 0x66, 0x91, 0xA9, 0x56,
  0x06, 0x06, 0x06, 0x06, 0x06,
  0x29, 0x45, 0xFF, 0x45, 0x29,
  0x10, 0x20, 0x7E, 0x20, 0x10,
  0x08, 0x04, 0x7E, 0x04, 0x08,
  0x10, 0x10, 0x54, 0x38, 0x10,
  0x10, 0x38, 0x54, 0x10, 0x10,
  0x78, 0x08, 0x08, 0x08, 0x08,
  0x30, 0x78, 0x30, 0x78, 0x30,
  0x0C, 0x1C, 0x7C, 0x1C, 0x0C,
  0x60, 0x70, 0x7C, 0x70, 0x60,
  0x00, 0x00, 0x00, 0x00, 0x00,		// ASCII(32)
  0x00, 0x00, 0xFA, 0x00, 0x00,		// !
  0x00, 0xE0, 0x00, 0xE0, 0x00,		// "
  0x28, 0xFE, 0x28, 0xFE, 0x28,		// #
  0x24, 0x54, 0xFE, 0x54, 0x48,		// $
  0xC4, 0xC8, 0x10, 0x26, 0x46,		// %
  0x6C, 0x92, 0x6A, 0x04, 0x0A,		// &
  0x00, 0x10, 0xE0, 0xC0, 0x00,		// '
  0x00, 0x38, 0x44, 0x82, 0x00,		// (
  0x00, 0x82, 0x44, 0x38, 0x00,		// )
  0x54, 0x38, 0xFE, 0x38, 0x54,		// *
  0x10, 0x10, 0x7C, 0x10, 0x10,		// +
  0x00, 0x01, 0x0E, 0x0C, 0x00,		// ,
  0x10, 0x10, 0x10, 0x10, 0x10,		// -
  0x00, 0x00, 0x06, 0x06, 0x00,		// .
//
//	        X
//	      X
//	    X
//	  X
//	X
//
//
  0x04, 0x08, 0x10, 0x20, 0x40,		// /
  0x7C, 0x8A, 0x92, 0xA2, 0x7C,		// 0
  0x00, 0x42, 0xFE, 0x02, 0x00,		// 1
  0x4E, 0x92, 0x92, 0x92, 0x62,		// 2
  0x84, 0x82, 0x92, 0xB2, 0xCC,		// 3
  0x18, 0x28, 0x48, 0xFE, 0x08,		// 4
  0xE4, 0xA2, 0xA2, 0xA2, 0x9C,		// 5
  0x3C, 0x52, 0x92, 0x92, 0x8C,		// 6
  0x82, 0x84, 0x88, 0x90, 0xE0,		// 7
  0x6C, 0x92, 0x92, 0x92, 0x6C,		// 8
  0x62, 0x92, 0x92, 0x94, 0x78,		// 9
  0x00, 0x6C, 0x6C, 0x00, 0x00,		// :
  0x00, 0x02, 0x2C, 0x00, 0x00,		// ;
  0x00, 0x10, 0x28, 0x44, 0x82,		// <
  0x28, 0x28, 0x28, 0x28, 0x28,		// =
  0x00, 0x82, 0x44, 0x28, 0x10,		// >
  0x40, 0x80, 0x9A, 0x90, 0x60,		// ?
  0x7C, 0x82, 0xBA, 0x9A, 0x72,		// @
  0x3E, 0x48, 0x88, 0x48, 0x3E,		// A
  0xFE, 0x92, 0x92, 0x92, 0x6C,		// B
  0x7C, 0x82, 0x82, 0x82, 0x44,		// C
  0xFE, 0x82, 0x82, 0x82, 0x7C,		// D: 0xFE, 0x82, 0x82, 0x82, 0x7C
  0xFE, 0x92, 0x92, 0x92, 0x82,		// E
  0xFE, 0x90, 0x90, 0x90, 0x80,		// F: 0xFE, 0x90, 0x90, 0x90, 0x80
  0x7C, 0x82, 0x82, 0x8A, 0xCE,		// G
  0xFE, 0x10, 0x10, 0x10, 0xFE,		// H
  0x00, 0x82, 0xFE, 0x82, 0x00,		// I: 0x00, 0x82, 0xFE, 0x82, 0x00
  0x04, 0x02, 0x82, 0xFC, 0x80,		// J
  0xFE, 0x10, 0x28, 0x44, 0x82,		// K
  0xFE, 0x02, 0x02, 0x02, 0x02,		// L
  0xFE, 0x40, 0x38, 0x40, 0xFE,		// M
  0xFE, 0x20, 0x10, 0x08, 0xFE,		// N
  0x7C, 0x82, 0x82, 0x82, 0x7C,		// O
  0xFE, 0x90, 0x90, 0x90, 0x60,		// P
  0x7C, 0x82, 0x8A, 0x84, 0x7A,		// Q
  0xFE, 0x90, 0x98, 0x94, 0x62,		// R: 0xFE, 0x90, 0x98, 0x94, 0x62
  0x64, 0x92, 0x92, 0x92, 0x4C,		// S
  0xC0, 0x80, 0xFE, 0x80, 0xC0,		// T
  0xFC, 0x02, 0x02, 0x02, 0xFC,		// U
  0xF8, 0x04, 0x02, 0x04, 0xF8,		// V
  0xFC, 0x02, 0x1C, 0x02, 0xFC,		// W: 0xFC, 0x02, 0x1C, 0x02, 0xFC
  0xC6, 0x28, 0x10, 0x28, 0xC6,		// X
  0xC0, 0x20, 0x1E, 0x20, 0xC0,		// Y: 0xC0, 0x20, 0x1E, 0x20, 0xC0
  0x86, 0x9A, 0x92, 0xB2, 0xC2,		// Z
  0x00, 0xFE, 0x82, 0x82, 0x82,		// [
  0x40, 0x20, 0x10, 0x08, 0x04,		// back slash
  0x00, 0x82, 0x82, 0x82, 0xFE,		// ]
  0x20, 0x40, 0x80, 0x40, 0x20,		// ^
  0x02, 0x02, 0x02, 0x02, 0x02,		// _
  0x00, 0xC0, 0xE0, 0x10, 0x00,		// '
  0x04, 0x2A, 0x2A, 0x1E, 0x02,		// a
  0xFE, 0x14, 0x22, 0x22, 0x1C,		// b
  0x1C, 0x22, 0x22, 0x22, 0x14,		// c
  0x1C, 0x22, 0x22, 0x14, 0xFE,		// d
  0x1C, 0x2A, 0x2A, 0x2A, 0x18,		// e
  0x00, 0x10, 0x7E, 0x90, 0x40,		// f
  0x18, 0x25, 0x25, 0x39, 0x1E,		// g
  0xFE, 0x10, 0x20, 0x20, 0x1E,		// h
  0x00, 0x22, 0xBE, 0x02, 0x00,		// i
  0x04, 0x02, 0x02, 0xBC, 0x00,		// j
  0xFE, 0x08, 0x14, 0x22, 0x00,		// k
  0x00, 0x82, 0xFE, 0x02, 0x00,		// l
  0x3E, 0x20, 0x1E, 0x20, 0x1E,		// m
  0x3E, 0x10, 0x20, 0x20, 0x1E,		// n
  0x1C, 0x22, 0x22, 0x22, 0x1C,		// o
  0x3F, 0x18, 0x24, 0x24, 0x18,		// p
  0x18, 0x24, 0x24, 0x18, 0x3F,		// q
  0x3E, 0x10, 0x20, 0x20, 0x10,		// r
  0x12, 0x2A, 0x2A, 0x2A, 0x24,		// s
  0x20, 0x20, 0xFC, 0x22, 0x24,		// t
  0x3C, 0x02, 0x02, 0x04, 0x3E,		// u
  0x38, 0x04, 0x02, 0x04, 0x38,		// v
  0x3C, 0x02, 0x0C, 0x02, 0x3C,		// w
  0x22, 0x14, 0x08, 0x14, 0x22,		// x
  0x32, 0x09, 0x09, 0x09, 0x3E,		// y
  0x22, 0x26, 0x2A, 0x32, 0x22,		// z
  0x00, 0x10, 0x6C, 0x82, 0x00,		// {
  0x00, 0x00, 0xEE, 0x00, 0x00,		// |
  0x00, 0x82, 0x6C, 0x10, 0x00,		// }
  0x40, 0x80, 0x40, 0x20, 0x40,		// ~	126
  0x3C, 0x64, 0xC4, 0x64, 0x3C,		//		127
  0x78, 0x85, 0x85, 0x86, 0x48,		//		128
  0x5C, 0x02, 0x02, 0x04, 0x5E,		//		129
  0x1C, 0x2A, 0x2A, 0xAA, 0x9A,		//		130
  0x84, 0xAA, 0xAA, 0x9E, 0x82,		//		131
  0x84, 0x2A, 0x2A, 0x1E, 0x82,		//		132
  0x84, 0xAA, 0x2A, 0x1E, 0x02,		//		133
  0x04, 0x2A, 0xAA, 0x9E, 0x02,		//		134
  0x30, 0x78, 0x4A, 0x4E, 0x48,		//		135
  0x9C, 0xAA, 0xAA, 0xAA, 0x9A,		//		136
  0x9C, 0x2A, 0x2A, 0x2A, 0x9A,		//		137
  0x9C, 0xAA, 0x2A, 0x2A, 0x1A,		//		138
  0x00, 0x00, 0xA2, 0x3E, 0x82,		//		139
  0x00, 0x40, 0xA2, 0xBE, 0x42,		//		140
  0x00, 0x80, 0xA2, 0x3E, 0x02,		//
  0x0F, 0x94, 0x24, 0x94, 0x0F,		//
  0x0F, 0x14, 0xA4, 0x14, 0x0F,		//		143
  0x3E, 0x2A, 0xAA, 0xA2, 0x00,		//
  0x04, 0x2A, 0x2A, 0x3E, 0x2A,		//
  0x3E, 0x50, 0x90, 0xFE, 0x92,		//
  0x4C, 0x92, 0x92, 0x92, 0x4C,		//
  0x4C, 0x12, 0x12, 0x12, 0x4C,		//
  0x4C, 0x52, 0x12, 0x12, 0x0C,		//
  0x5C, 0x82, 0x82, 0x84, 0x5E,		//		150
  0x5C, 0x42, 0x02, 0x04, 0x1E,		//
  0x00, 0xB9, 0x05, 0x05, 0xBE,		//
  0x9C, 0x22, 0x22, 0x22, 0x9C,		//
  0xBC, 0x02, 0x02, 0x02, 0xBC,		//
  0x3C, 0x24, 0xFF, 0x24, 0x24,		//
  0x12, 0x7E, 0x92, 0xC2, 0x66,		//
  0xD4, 0xF4, 0x3F, 0xF4, 0xD4,		//
  0xFF, 0x90, 0x94, 0x6F, 0x04,		//
  0x03, 0x11, 0x7E, 0x90, 0xC0,		//
  0x04, 0x2A, 0x2A, 0x9E, 0x82,		//		160
  0x00, 0x00, 0x22, 0xBE, 0x82,		//
  0x0C, 0x12, 0x12, 0x52, 0x4C,		//
  0x1C, 0x02, 0x02, 0x44, 0x5E,		//
  0x00, 0x5E, 0x50, 0x50, 0x4E,		//
  0xBE, 0xB0, 0x98, 0x8C, 0xBE,		//
  0x64, 0x94, 0x94, 0xF4, 0x14,		//
  0x64, 0x94, 0x94, 0x94, 0x64,		//
  0x0C, 0x12, 0xB2, 0x02, 0x04,		//
  0x1C, 0x10, 0x10, 0x10, 0x10,		//
  0x10, 0x10, 0x10, 0x10, 0x1C,		//		170
  0xF4, 0x08, 0x13, 0x35, 0x5D,		//
  0xF4, 0x08, 0x14, 0x2C, 0x5F,		//
  0x00, 0x00, 0xDE, 0x00, 0x00,		//
  0x10, 0x28, 0x54, 0x28, 0x44,		//
  0x44, 0x28, 0x54, 0x28, 0x10,		//
//	  XX
//	 X  X
//	  XX
//
//
//
//
//
  0x00, 0x40, 0xA0, 0xA0, 0x40,		//		176
  0x55, 0xAA, 0x55, 0xAA, 0x55,		//
  0xAA, 0x55, 0xAA, 0x55, 0xAA,		//
  0x00, 0x00, 0x00, 0xFF, 0x00,		//
  0x08, 0x08, 0x08, 0xFF, 0x00,		//		180
  0x28, 0x28, 0x28, 0xFF, 0x00,		//
  0x08, 0x08, 0xFF, 0x00, 0xFF,		//
  0x08, 0x08, 0x0F, 0x08, 0x0F,		//
  0x28, 0x28, 0x28, 0x3F, 0x00,		//
  0x28, 0x28, 0xEF, 0x00, 0xFF,		//
  0x00, 0x00, 0xFF, 0x00, 0xFF,		//
  0x28, 0x28, 0x2F, 0x20, 0x3F,		//
  0x28, 0x28, 0xE8, 0x08, 0xF8,		//
  0x08, 0x08, 0xF8, 0x08, 0xF8,		//
  0x28, 0x28, 0x28, 0xF8, 0x00,		//190
  0x08, 0x08, 0x08, 0x0F, 0x00,		//
  0x3E, 0x48, 0x88, 0x48, 0x3E,		// �	.
  0xFE, 0x92, 0x92, 0x92, 0x8C,		// �	.
  0xFE, 0x92, 0x92, 0x92, 0x6C,		// �	.
//	X X X X X		- high bit (X.)
//	X
//	X
//	X
//	X
//	X
//	X
//
  0xFE, 0x80, 0x80, 0x80, 0x80,		// �	.
//	    X X
//	  X   X
//	  X   X
//	  X   X
//	  X   X
//	  X   X
//	X X X X X
//	X       X
  0x03, 0x7E, 0x82, 0xFE, 0x03,		// �	.
  0xFE, 0x92, 0x92, 0x92, 0x82,		// �	.
//	X   X   X
//	X   X   X
//	X   X   X
//	  X X X
//	X   X   X
//	X   X   X
//	X   X   X
//
  0xEE, 0x10, 0xFE, 0x10, 0xEE,		// �	.
//	  X X X
//	X       X
//	        X
//	      X
//	        X
//	X       X
//	  X X X
//
  0x44, 0x82, 0x82, 0x92, 0x6C,		// �	.
//	X       X
//	X       X
//	X     X X
//	X   X   X
//	X   X   X
//	X X     X
//	X       X
//
  0xFE, 0x04, 0x18, 0x20, 0xFE,		// �	.200
  0xFE, 0x04, 0x98, 0x20, 0xFE,		// �	.
  0xFE, 0x10, 0x28, 0x44, 0x82,		// �	.
  0x02, 0xFC, 0x80, 0x80, 0xFE,		// �	.
  0xFE, 0x40, 0x38, 0x40, 0xFE,		// �	.
  0xFE, 0x10, 0x10, 0x10, 0xFE,		// �	.
  0x7C, 0x82, 0x82, 0x82, 0x7C,		// �	.
  0xFE, 0x80, 0x80, 0x80, 0xFE,		// �	.
  0xFE, 0x90, 0x90, 0x90, 0x60,		// �	.
  0x7C, 0x82, 0x82, 0x82, 0x44,		// �	.
  0xC0, 0x80, 0xFE, 0x80, 0xC0,		// �	.210
//	X       X
//	X       X
//	X       X
//	  X X X X
//	        X
//	        X
//	  X X X
//
  0xE0, 0x12, 0x12, 0x12, 0xFC,		// �	.
//	    X			- high bit (X.)
//	  X X X
//	X   X   X
//	X   X   X
//	X   X   X
//	  X X X
//	    X
//
  0x38, 0x44, 0xFE, 0x44, 0x38,		// �	.
  0xC6, 0x28, 0x10, 0x28, 0xC6,		// �	.
//	X     X			- high bit (X.)
//	X     X
//	X     X
//	X     X
//	X     X
//	X     X
//	X X X X X
//          X
  0xFE, 0x02, 0x02, 0xFE, 0x03,		// �	.
//	X       X		- high bit (X.)
//	X       X
//	X       X
//	X       X
//	  X X X X
//	        X
//	        X
//
  0xF0, 0x08, 0x08, 0x08, 0xFE,		// �	.
  0xFE, 0x02, 0xFE, 0x02, 0xFE,		// �	.
  0xFE, 0x02, 0xFE, 0x02, 0xFF,		// �	.
  0x00, 0x00, 0x00, 0x0F, 0x08,		// �
//	X       X
//	X       X
//	X       X
//	X X X   X
//	X     X X
//	X     X X
//	X X X   X
//
  0xFE, 0x12, 0x12, 0x0C, 0xFE,		// �	.
  0xFE, 0x12, 0x12, 0x12, 0x0E,		// �	.220
//	  X X X
//	X       X
//	        X
//	  X X X X
//	        X
//	X       X
//	  X X X
//
  0x44, 0x92, 0x92, 0x92, 0x7C,		// �	.
  0x00, 0x00, 0x00, 0xFF, 0xFF,		// �
//	  X X X X		- high bit (X.)
//	X       X
//	X       X
//	  X X X X
//	    X   X		- high bit (.X)
//	  X     X
//	X       X
//
  0x62, 0x94, 0x98, 0x90, 0xFE,		// �	.
  0x04, 0x2A, 0x2A, 0x1E, 0x02,		// �	.
  0x3E, 0x54, 0x54, 0x7C, 0x28,		// �
//
//
//	X X X
//	X     X
//	X X X
//	X     X
//	X X X
//
  0x3E, 0x2A, 0x2A, 0x14, 0x00,		// �	.
  0x40, 0x7E, 0x40, 0x7E, 0x40,		// �
//
//
//	    X X
//	  X   X
//	  X   X
//	  X   X
//	X X X X X
//  X       X
  0x03, 0x1E, 0x22, 0x3E, 0x03,		// �	.
  0x1C, 0x2A, 0x2A, 0x2A, 0x18,		// e	.
//
//
//	X   X   X
//	X   X   X
//	  X X X
//	X   X   X
//	X   X   X
//
  0x36, 0x08, 0x3E, 0x08, 0x36,		// �	.230
//
//
//	  X X X
//	        X
//	    X X
//	        X
//	X X X X
//
  0x02, 0x22, 0x2A, 0x2A, 0x14,		// �	.
//
//
//	X     X
//	X     X
//	X   X X
//	X X   X
//	X     X
//
  0x3E, 0x04, 0x08, 0x3E, 0x00,		// �	.
  0x3E, 0x84, 0x88, 0x3E, 0x00,		// �	.
  0x3E, 0x08, 0x14, 0x22, 0x00,		// �	.
  0x0C, 0x52, 0xB2, 0xB2, 0x0C,		// �
  0x0C, 0x12, 0x1E, 0x12, 0x0C,		// �
  0x3D, 0x46, 0x5A, 0x62, 0xBC,		// �
  0x7C, 0x92, 0x92, 0x92, 0x00,		// �
//
//
//	X X X X
//	X     X
//	X     X
//	X     X
//	X     X
//
  0x3E, 0x20, 0x20, 0x3E, 0x00,		// �	.
//
//
//	X X X
//	X     X
//	X     X
//	X X X
//	X
//	X
  0x3F, 0x24, 0x24, 0x18, 0x00,		// �	.240
  0x1C, 0x22, 0x22, 0x22, 0x14,		// �	.
  0x20, 0x20, 0x3E, 0x20, 0x20,		// �	.
  0x02, 0x22, 0x52, 0x8A, 0x02,		// �
  0x00, 0x00, 0xFF, 0x80, 0xC0,		// �
  0x22, 0x14, 0x08, 0x14, 0x22,		// �	.
  0x10, 0x10, 0xD6, 0xD6, 0x10,		// �
  0x38, 0x08, 0x08, 0x3E, 0x00,		// �	.
  0x60, 0xF0, 0x90, 0xF0, 0x60,		// �
  0x00, 0x00, 0x18, 0x18, 0x00,		// �
  0x00, 0x00, 0x08, 0x08, 0x00,		// �	250
  0x0C, 0x02, 0xFF, 0x80, 0x80,		// �
//
//
//	X
//	X
//	X X X
//	X     X
//	X X X
//
  0x3E, 0x0A, 0x0A, 0x04, 0x00,		// �	.
  0x00, 0x98, 0xB8, 0xE8, 0x48,		// �
  0x00, 0x3C, 0x3C, 0x3C, 0x3C,		// �
  0x00, 0x00, 0x00, 0x00, 0x00,};	// �
#elif (FONTSIZE ==_8x16PCX)
#define FONTSIZE_X	8
#define FONTSIZE_Y	16
const uint8_t font[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// ASCII(00)
0x00,0x00,0x7e,0x81,0xa5,0x81,0x81,0xbd,0x99,0x81,0x81,0x7e,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x7e,0xff,0xdb,0xff,0xff,0xc3,0xe7,0xff,0xff,0x7e,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x00,0x00,0x6c,0xfe,0xfe,0xfe,0xfe,0x7c,0x38,0x10,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x00,0x00,0x10,0x38,0x7c,0xfe,0x7c,0x38,0x10,0x00,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x00,0x18,0x3c,0x3c,0xe7,0xe7,0xe7,0x18,0x18,0x3c,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x00,0x18,0x3c,0x7e,0xff,0xff,0x7e,0x18,0x18,0x3c,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x3c,0x3c,0x18,0x00,0x00,0x00,0x00,0x00,0x00,	//
0xff,0xff,0xff,0xff,0xff,0xff,0xe7,0xc3,0xc3,0xe7,0xff,0xff,0xff,0xff,0xff,0xff,	//
0x00,0x00,0x00,0x00,0x00,0x3c,0x66,0x42,0x42,0x66,0x3c,0x00,0x00,0x00,0x00,0x00,	//
0xff,0xff,0xff,0xff,0xff,0xc3,0x99,0xbd,0xbd,0x99,0xc3,0xff,0xff,0xff,0xff,0xff,	//
0x00,0x00,0x1e,0x0e,0x1a,0x32,0x78,0xcc,0xcc,0xcc,0xcc,0x78,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x3c,0x66,0x66,0x66,0x66,0x3c,0x18,0x7e,0x18,0x18,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x3f,0x33,0x3f,0x30,0x30,0x30,0x30,0x70,0xf0,0xe0,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x7f,0x63,0x7f,0x63,0x63,0x63,0x63,0x67,0xe7,0xe6,0xc0,0x00,0x00,0x00,	//
0x00,0x00,0x00,0x18,0x18,0xdb,0x3c,0xe7,0x3c,0xdb,0x18,0x18,0x00,0x00,0x00,0x00,	// ASCII(15)
0x00,0x80,0xc0,0xe0,0xf0,0xf8,0xfe,0xf8,0xf0,0xe0,0xc0,0x80,0x00,0x00,0x00,0x00,	//
0x00,0x02,0x06,0x0e,0x1e,0x3e,0xfe,0x3e,0x1e,0x0e,0x06,0x02,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x18,0x3c,0x7e,0x18,0x18,0x18,0x7e,0x3c,0x18,0x00,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x66,0x66,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x7f,0xdb,0xdb,0xdb,0x7b,0x1b,0x1b,0x1b,0x1b,0x1b,0x00,0x00,0x00,0x00,	//
0x00,0x7c,0xc6,0x60,0x38,0x6c,0xc6,0xc6,0x6c,0x38,0x0c,0xc6,0x7c,0x00,0x00,0x00,	//
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xfe,0xfe,0xfe,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x18,0x3c,0x7e,0x18,0x18,0x18,0x7e,0x3c,0x18,0x7e,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x18,0x3c,0x7e,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x7e,0x3c,0x18,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x00,0x00,0x00,0x18,0x0c,0xfe,0x0c,0x18,0x00,0x00,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x00,0x00,0x00,0x30,0x60,0xfe,0x60,0x30,0x00,0x00,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x00,0x00,0x00,0x28,0x6c,0xfe,0x6c,0x28,0x00,0x00,0x00,0x00,0x00,0x00,	// ASCII(29)
0x00,0x00,0x00,0x00,0x10,0x38,0x38,0x7c,0x7c,0xfe,0xfe,0x00,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x00,0x00,0xfe,0xfe,0x7c,0x7c,0x38,0x38,0x10,0x00,0x00,0x00,0x00,0x00,	//
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// ASCII(32)
0x00,0x00,0x18,0x3c,0x3c,0x3c,0x18,0x18,0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00,	// !
0x00,0x66,0x66,0x66,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// "
0x00,0x00,0x00,0x6c,0x6c,0xfe,0x6c,0x6c,0x6c,0xfe,0x6c,0x6c,0x00,0x00,0x00,0x00,
0x18,0x18,0x7c,0xc6,0xc2,0xc0,0x7c,0x06,0x06,0x86,0xc6,0x7c,0x18,0x18,0x00,0x00,
0x00,0x00,0x00,0x00,0xc2,0xc6,0x0c,0x18,0x30,0x60,0xc6,0x86,0x00,0x00,0x00,0x00,
0x00,0x00,0x38,0x6c,0x6c,0x38,0x76,0xdc,0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,
0x00,0x30,0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x0c,0x18,0x30,0x30,0x30,0x30,0x30,0x30,0x18,0x0c,0x00,0x00,0x00,0x00,
0x00,0x00,0x30,0x18,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x18,0x30,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x66,0x3c,0xff,0x3c,0x66,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x7e,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x30,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x06,0x0c,0x18,0x30,0x60,0xc0,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x38,0x6c,0xc6,0xc6,0xd6,0xd6,0xc6,0xc6,0x6c,0x38,
0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x38,0x78,0x18,0x18,0x18,0x18,0x18,0x18,
0x7e,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0xc6,0x06,0x0c,0x18,0x30,0x60,0xc0,
0xc6,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0xc6,0x06,0x06,0x3c,0x06,0x06,
0x06,0xc6,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0x1c,0x3c,0x6c,0xcc,0xfe,
0x0c,0x0c,0x0c,0x1e,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xc0,0xc0,0xc0,0xfc,
0x06,0x06,0x06,0xc6,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x60,0xc0,0xc0,
0xfc,0xc6,0xc6,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xc6,0x06,
0x06,0x0c,0x18,0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0xc6,
0xc6,0xc6,0x7c,0xc6,0xc6,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,
0xc6,0xc6,0xc6,0x7e,0x06,0x06,0x06,0x0c,0x78,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x30,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x06,0x0c,0x18,0x30,0x60,0x30,0x18,0x0c,0x06,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x00,0x00,0x7e,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x60,0x30,0x18,0x0c,0x06,0x0c,0x18,0x30,0x60,0x00,
0x00,0x00,0x00,0x00,0x00,0x7c,0xc6,0xc6,0x0c,0x18,0x18,0x18,0x00,0x18,0x18,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0xc6,0xc6,0xde,0xde,0xde,0xdc,0xc0,
0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x38,0x6c,0xc6,0xc6,0xfe,0xc6,0xc6,
0xc6,0xc6,0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0x66,0x66,0x66,0x7c,0x66,0x66,
0x66,0x66,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x66,0xc2,0xc0,0xc0,0xc0,
0xc0,0xc2,0x66,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0x6c,0x66,0x66,0x66,
0x66,0x66,0x66,0x6c,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x66,0x62,0x68,
0x78,0x68,0x60,0x62,0x66,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x66,0x62,
0x68,0x78,0x68,0x60,0x60,0x60,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x66,
0xc2,0xc0,0xc0,0xde,0xc6,0xc6,0x66,0x3a,0x00,0x00,0x00,0x00,0x00,0x00,0xc6,
0xc6,0xc6,0xc6,0xfe,0xc6,0xc6,0xc6,0xc6,0xc6,0x00,0x00,0x00,0x00,0x00,0x00,
0x3c,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x3c,0x00,0x00,0x00,0x00,0x00,
0x00,0x1e,0x0c,0x0c,0x0c,0x0c,0x0c,0xcc,0xcc,0xcc,0x78,0x00,0x00,0x00,0x00,
0x00,0x00,0xe6,0x66,0x66,0x6c,0x78,0x78,0x6c,0x66,0x66,0xe6,0x00,0x00,0x00,
0x00,0x00,0x00,0xf0,0x60,0x60,0x60,0x60,0x60,0x60,0x62,0x66,0xfe,0x00,0x00,
0x00,0x00,0x00,0x00,0xc6,0xee,0xfe,0xfe,0xd6,0xc6,0xc6,0xc6,0xc6,0xc6,0x00,
0x00,0x00,0x00,0x00,0x00,0xc6,0xe6,0xf6,0xfe,0xde,0xce,0xc6,0xc6,0xc6,0xc6,
0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,
0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0x66,0x66,0x66,0x7c,0x60,0x60,0x60,
0x60,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,
0xd6,0xde,0x7c,0x0c,0x0e,0x00,0x00,0x00,0x00,0xfc,0x66,0x66,0x66,0x7c,0x6c,
0x66,0x66,0x66,0xe6,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0xc6,0xc6,0x60,0x38,
0x0c,0x06,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x7e,0x5a,0x18,
0x18,0x18,0x18,0x18,0x18,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0xc6,0xc6,0xc6,
0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0xc6,0xc6,
0xc6,0xc6,0xc6,0xc6,0xc6,0x6c,0x38,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0xc6,
0xc6,0xc6,0xc6,0xd6,0xd6,0xd6,0xfe,0xee,0x6c,0x00,0x00,0x00,0x00,0x00,0x00,
0xc6,0xc6,0x6c,0x7c,0x38,0x38,0x7c,0x6c,0xc6,0xc6,0x00,0x00,0x00,0x00,0x00,
0x00,0x66,0x66,0x66,0x66,0x3c,0x18,0x18,0x18,0x18,0x3c,0x00,0x00,0x00,0x00,
0x00,0x00,0xfe,0xc6,0x86,0x0c,0x18,0x30,0x60,0xc2,0xc6,0xfe,0x00,0x00,0x00,
0x00,0x00,0x00,0x3c,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x3c,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0xc0,0xe0,0x70,0x38,0x1c,0x0e,0x06,0x02,0x00,
0x00,0x00,0x00,0x00,0x00,0x3c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x3c,
0x00,0x00,0x00,0x00,0x10,0x38,0x6c,0xc6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xff,0x00,0x00,0x00,0x30,0x18,0x0c,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x0c,0x7c,
0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0x60,0x60,0x78,0x6c,
0x66,0x66,0x66,0x66,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,
0xc6,0xc0,0xc0,0xc0,0xc6,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0x0c,0x0c,
0x3c,0x6c,0xcc,0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x7c,0xc6,0xfe,0xc0,0xc0,0xc6,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,
0x36,0x32,0x30,0x78,0x30,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x76,0xcc,0xcc,0xcc,0xcc,0xcc,0x7c,0x0c,0xcc,0x78,0x00,0x00,
0x00,0xe0,0x60,0x60,0x6c,0x76,0x66,0x66,0x66,0x66,0xe6,0x00,0x00,0x00,0x00,
0x00,0x00,0x18,0x18,0x00,0x38,0x18,0x18,0x18,0x18,0x18,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x06,0x06,0x00,0x0e,0x06,0x06,0x06,0x06,0x06,0x06,0x66,0x66,
0x3c,0x00,0x00,0x00,0xe0,0x60,0x60,0x66,0x6c,0x78,0x78,0x6c,0x66,0xe6,0x00,
0x00,0x00,0x00,0x00,0x00,0x38,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x3c,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xfe,0xd6,0xd6,0xd6,0xd6,
0xc6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xdc,0x66,0x66,0x66,0x66,
0x66,0x66,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0xc6,0xc6,0xc6,
0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xdc,0x66,0x66,
0x66,0x66,0x66,0x7c,0x60,0x60,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x76,0xcc,
0xcc,0xcc,0xcc,0xcc,0x7c,0x0c,0x0c,0x1e,0x00,0x00,0x00,0x00,0x00,0x00,0xdc,
0x76,0x66,0x60,0x60,0x60,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x7c,0xc6,0x60,0x38,0x0c,0xc6,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x30,
0x30,0xfc,0x30,0x30,0x30,0x30,0x36,0x1c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xc6,0xc6,0xc6,0xc6,0xc6,0x6c,0x38,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xc6,0xc6,0xd6,0xd6,0xd6,0xfe,0x6c,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xc6,0x6c,0x38,0x38,0x38,0x6c,0xc6,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,0x7e,0x06,0x0c,
0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xcc,0x18,0x30,0x60,0xc6,0xfe,0x00,
0x00,0x00,0x00,0x00,0x00,0x0e,0x18,0x18,0x18,0x70,0x18,0x18,0x18,0x18,0x0e,
0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x18,0x18,0x18,0x0e,0x18,0x18,0x18,
0x18,0x70,0x00,0x00,0x00,0x00,0x00,0x76,0xdc,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x38,0x6c,0xc6,
0xc6,0xc6,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x66,0xc2,0xc0,0xc0,
0xc0,0xc0,0xc2,0x66,0x3c,0x18,0x70,0x00,0x00,0x00,0x00,0xcc,0x00,0x00,0xcc,
0xcc,0xcc,0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,0x00,0x0c,0x18,0x30,0x00,
0x7c,0xc6,0xfe,0xc0,0xc0,0xc6,0x7c,0x00,0x00,0x00,0x00,0x00,0x10,0x38,0x6c,
0x00,0x78,0x0c,0x7c,0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,0x00,0x00,0xcc,
0x00,0x00,0x78,0x0c,0x7c,0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,0x00,0x60,
0x30,0x18,0x00,0x78,0x0c,0x7c,0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,0x00,
0x38,0x6c,0x38,0x00,0x78,0x0c,0x7c,0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7c,0xc6,0xc0,0xc0,0xc0,0xc6,0x7c,0x18,0x70,0x00,
0x00,0x00,0x10,0x38,0x6c,0x00,0x7c,0xc6,0xfe,0xc0,0xc0,0xc6,0x7c,0x00,0x00,
0x00,0x00,0x00,0x00,0xc6,0x00,0x00,0x7c,0xc6,0xfe,0xc0,0xc0,0xc6,0x7c,0x00,
0x00,0x00,0x00,0x00,0x60,0x30,0x18,0x00,0x7c,0xc6,0xfe,0xc0,0xc0,0xc6,0x7c,
0x00,0x00,0x00,0x00,0x00,0x00,0x66,0x00,0x00,0x38,0x18,0x18,0x18,0x18,0x18,
0x3c,0x00,0x00,0x00,0x00,0x00,0x18,0x3c,0x66,0x00,0x38,0x18,0x18,0x18,0x18,
0x18,0x3c,0x00,0x00,0x00,0x00,0x00,0x60,0x30,0x18,0x00,0x38,0x18,0x18,0x18,
0x18,0x18,0x3c,0x00,0x00,0x00,0x00,0x00,0xc6,0x00,0x10,0x38,0x6c,0xc6,0xc6,
0xfe,0xc6,0xc6,0xc6,0x00,0x00,0x00,0x00,0x38,0x6c,0x38,0x10,0x38,0x6c,0xc6,
0xfe,0xc6,0xc6,0xc6,0xc6,0x00,0x00,0x00,0x00,0x0c,0x18,0x00,0xfe,0x66,0x62,
0x68,0x78,0x68,0x62,0x66,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xec,0x36,0x36,0x7e,0xd8,0xd8,0x6e,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x6c,
0xcc,0xcc,0xfe,0xcc,0xcc,0xcc,0xcc,0xce,0x00,0x00,0x00,0x00,0x00,0x10,0x38,
0x6c,0x00,0x7c,0xc6,0xc6,0xc6,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,
0xc6,0x00,0x00,0x7c,0xc6,0xc6,0xc6,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,0x00,
0x60,0x30,0x18,0x00,0x7c,0xc6,0xc6,0xc6,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,
0x00,0x30,0x78,0xcc,0x00,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,
0x00,0x00,0x60,0x30,0x18,0x00,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0x76,0x00,0x00,
0x00,0x00,0x00,0x00,0xc6,0x00,0x00,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,0x7e,0x06,
0x0c,0x78,0x00,0x00,0xc6,0x00,0x7c,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,0x7c,
0x00,0x00,0x00,0x00,0x00,0xc6,0x00,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,
0x7c,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x7c,0xc6,0xc0,0xc0,0xc0,0xc6,0x7c,
0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x38,0x6c,0x64,0x60,0xf0,0x60,0x60,0x60,
0x60,0xe6,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x3c,0x18,0x7e,0x18,
0x7e,0x18,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0xf8,0xcc,0xcc,0xf8,0xc4,0xcc,
0xde,0xcc,0xcc,0xcc,0xc6,0x00,0x00,0x00,0x00,0x00,0x0e,0x1b,0x18,0x18,0x18,
0x7e,0x18,0x18,0x18,0xd8,0x70,0x00,0x00,0x00,0x00,0x00,0x18,0x30,0x60,0x00,
0x78,0x0c,0x7c,0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,0x00,0x0c,0x18,0x30,
0x00,0x38,0x18,0x18,0x18,0x18,0x18,0x3c,0x00,0x00,0x00,0x00,0x00,0x18,0x30,
0x60,0x00,0x7c,0xc6,0xc6,0xc6,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,0x00,0x18,
0x30,0x60,0x00,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,0x00,
0x00,0x76,0xdc,0x00,0xdc,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x00,
0x76,0xdc,0x00,0xc6,0xe6,0xf6,0xfe,0xde,0xce,0xc6,0xc6,0xc6,0x00,0x00,0x00,
0x00,0x00,0x00,0x3c,0x6c,0x6c,0x3e,0x00,0x7e,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x38,0x6c,0x6c,0x38,0x00,0x7c,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x30,0x30,0x60,0xc0,0xc6,0xc6,0x7c,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xc0,0xc0,0xc0,0xc0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x06,0x06,0x06,
0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0xe0,0x62,0x66,0x6c,0x18,0x30,0x60,
0xdc,0x86,0x0c,0x18,0x3e,0x00,0x00,0x00,0x60,0xe0,0x62,0x66,0x6c,0x18,0x30,
0x66,0xce,0x9a,0x3f,0x06,0x06,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x18,0x18,
0x18,0x3c,0x3c,0x3c,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x36,
0x6c,0xd8,0x6c,0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xd8,0x6c,0x36,0x6c,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x44,0x11,0x44,
0x11,0x44,0x11,0x44,0x11,0x44,0x11,0x44,0x11,0x44,0x11,0x44,0x55,0xaa,0x55,
0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0xdd,0x77,
0xdd,0x77,0xdd,0x77,0xdd,0x77,0xdd,0x77,0xdd,0x77,0xdd,0x77,0xdd,0x77,0x18,
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xf8,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
0x18,0x18,0x18,0x18,0x18,0x18,0xf8,0x18,0xf8,0x18,0x18,0x18,0x18,0x18,0x18,
0x18,0x18,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0xf6,0x36,0x36,0x36,0x36,0x36,
0x36,0x36,0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x36,0x36,0x36,0x36,
0x36,0x36,0x36,0x36,0x00,0x00,0x00,0x00,0x00,0xf8,0x18,0xf8,0x18,0x18,0x18,
0x18,0x18,0x18,0x18,0x18,0x36,0x36,0x36,0x36,0x36,0xf6,0x06,0xf6,0x36,0x36,
0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x00,0x00,0x00,0x00,0x00,0xfe,0x06,0xf6,
0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0xf6,0x06,
0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x36,0x36,0x36,0x36,0x36,
0x36,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x18,0x18,
0xf8,0x18,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xf8,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
0x18,0x18,0x18,0x18,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,
0x18,0x18,0x18,0x18,0x18,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x1f,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xff,0x18,0x18,0x18,0x18,0x18,
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x1f,0x18,0x1f,0x18,0x18,0x18,0x18,
0x18,0x18,0x18,0x18,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x37,0x36,0x36,0x36,
0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x37,0x30,0x3f,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x30,0x37,0x36,
0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0xf7,0x00,0xff,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,
0xf7,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x37,
0x30,0x37,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x00,0x00,0x00,0x00,0x00,
0xff,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x36,0x36,0x36,
0x36,0xf7,0x00,0xf7,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x18,0x18,0x18,
0x18,0x18,0xff,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x36,
0x36,0x36,0x36,0x36,0x36,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xff,0x00,0xff,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x1f,0x18,0x1f,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x18,0x1f,0x18,0x18,0x18,0x18,
0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x36,0x36,0x36,
0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0xff,0x36,0x36,
0x36,0x36,0x36,0x36,0x36,0x36,0x18,0x18,0x18,0x18,0x18,0xff,0x18,0xff,0x18,
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xf8,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x1f,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0xf0,0xf0,0xf0,
0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0x0f,0x0f,0x0f,
0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x76,0xdc,0xd8,0xd8,0xd8,0xdc,0x76,0x00,0x00,0x00,0x00,
0x00,0x00,0x78,0xcc,0xcc,0xcc,0xd8,0xcc,0xc6,0xc6,0xc6,0xcc,0x00,0x00,0x00,
0x00,0x00,0x00,0xfe,0xc6,0xc6,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x6c,0x6c,0x6c,0x6c,0x6c,0x6c,0x00,
0x00,0x00,0x00,0x00,0x00,0xfe,0xc6,0x60,0x30,0x18,0x18,0x30,0x60,0xc6,0xfe,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0xd8,0xd8,0xd8,0xd8,0xd8,
0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,
0x66,0x7c,0x60,0x60,0xc0,0x00,0x00,0x00,0x00,0x00,0x76,0xdc,0x18,0x18,0x18,
0x18,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x18,0x3c,0x66,0x66,0x66,
0x66,0x3c,0x18,0x7e,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x6c,0xc6,0xc6,0xfe,
0xc6,0xc6,0xc6,0x6c,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x6c,0xc6,0xc6,
0xc6,0x6c,0x6c,0x6c,0x6c,0xee,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x30,0x18,
0x0c,0x3e,0x66,0x66,0x66,0x66,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x7e,0xdb,0xdb,0xdb,0x7e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x03,0x06,0x7e,0xdb,0xdb,0xf3,0x7e,0x60,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
0x1c,0x30,0x60,0x60,0x7c,0x60,0x60,0x60,0x30,0x1c,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x7c,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xfe,0x00,0x00,0xfe,0x00,0x00,0xfe,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x7e,0x18,0x18,0x00,0x00,0x7e,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x30,0x18,0x0c,0x06,0x0c,0x18,0x30,0x00,0x7e,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0x18,0x30,0x60,0x30,0x18,0x0c,0x00,0x7e,
0x00,0x00,0x00,0x00,0x00,0x00,0x0e,0x1b,0x1b,0x18,0x18,0x18,0x18,0x18,0x18,
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xd8,
0xd8,0xd8,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x7e,0x00,
0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x76,0xdc,0x00,
0x76,0xdc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x6c,0x6c,0x38,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0c,0x0c,
0x0c,0x0c,0x0c,0xec,0x6c,0x6c,0x3c,0x1c,0x00,0x00,0x00,0x00,0x00,0x6c,0x36,
0x36,0x36,0x36,0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,
0x66,0x0c,0x18,0x32,0x7e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,
};
#endif
#endif