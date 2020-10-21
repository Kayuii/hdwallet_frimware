#ifndef _GT24L24A2Y_H__
#define _GT24L24A2Y_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gt24l24a2y_hal.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

extern void r_dat_bat(unsigned long TAB_addr, unsigned int Num,
                      unsigned char *p_arr);

void decompressbmp16(BYTE *dst, int h);

#define ASCII_5X7 1
#define ASCII_7X8 2
#define ASCII_6X12 3
#define ASCII_12_B_A 4
#define ASCII_12_B_T 5
#define ASCII_8X16 6
#define ASCII_16_A 7
#define ASCII_16_T 8
#define ASCII_12X24 9
#define ASCII_24_B 10
#define ASCII_16X32 11
#define ASCII_32_B 12

#define B_11X16_A 13
#define B_18X24_A 14
#define B_22X32_A 15
#define B_34X48_A 16
#define B_40X64_A 17
#define B_11X16_T 18
#define B_18X24_T 19
#define B_22X32_T 20
#define B_34X48_T 21
#define B_40X64_T 22
#define T_FONT_20X24 23
#define T_FONT_24X32 24
#define T_FONT_34X48 25
#define T_FONT_48X64 26
#define F_FONT_816 27
#define F_FONT_1624 28
#define F_FONT_1632 29
#define F_FONT_2448 30
#define F_FONT_3264 31
#define KCD_UI_32 32

#define SEL_GB 33
#define SEL_JIS 34
#define SEL_KSC 35

unsigned char ASCII_GetData(unsigned char ASCIICode, unsigned long ascii_kind,
                            unsigned char *DZ_Data);
unsigned long gt(unsigned int gb, unsigned int gbex);
unsigned long GB2312_24_GetData(unsigned int gb);
unsigned long JIS_16_GetData(unsigned int gb);
unsigned long KSC_16_GetData(unsigned int gb);
unsigned long shift_h_GetData(unsigned char CODE, unsigned char *DZ_Data);
unsigned long LATIN_GetData(unsigned int FontCode, unsigned char *DZ_Data);
unsigned long CYRILLIC_GetData(unsigned int FontCode, unsigned char *DZ_Data);
unsigned long GREECE_GetData(unsigned int FontCode, unsigned char *DZ_Data);
unsigned long HEBREW_GetData(unsigned int FontCode, unsigned char *DZ_Data);
unsigned long LATIN_B_GetData(unsigned int FontCode, unsigned char *DZ_Data);
unsigned long CYRILLIC_B_GetData(unsigned int FontCode, unsigned char *DZ_Data);
unsigned long GREECE_B_GetData(unsigned int FontCode, unsigned char *DZ_Data);
unsigned long ALB_B_GetData(unsigned int unicode_alb, unsigned char *DZ_Data);
unsigned long THAILAND_GetData(unsigned int FontCode, unsigned char *DZ_Data);
// Unicode to GB18030
unsigned int U2G(unsigned int UN_CODE);
unsigned int BIG52GBK(unsigned char h, unsigned char l);
unsigned int U2J(WORD Unicode);
unsigned int U2K(WORD Unicode);
unsigned int SJIS2JIS(WORD code);
void zz_zf(unsigned char Sequence, unsigned char kind, unsigned char *DZ_Data);

#define ntohs(s)

void hzbmp16(unsigned char charset, unsigned int code, unsigned int codeex,
             unsigned char size, unsigned char *buf);
void hzbmp24(unsigned char charset, unsigned int code, unsigned int codeex,
             unsigned char size, unsigned char *buf);
#endif
