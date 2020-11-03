#ifndef _EPD_W21_C02_H_
#define _EPD_W21_C02_H_

#include "epd_wxx_hal.h"

#define EPD_RES_WIDTH 250
#define EPD_RES_HIGHT 122
#define EPD_RES_ROW 16

#define EPD_FULLBUF_SIZE (EPD_RES_WIDTH * EPD_RES_ROW)

#define MONOMSB_MODE 1
#define MONOLSB_MODE 2
#define RED_MODE 3

#define MAX_LINE_BYTES 16  // =128/8
#define MAX_COLUMN_BYTES 250

#define ALLSCREEN_GRAGHBYTES 4000

// 界面颜色反转 0 为白色  1 为黑色
#define DOT_WHITE_VALUE		0

void epd_init_full(void);
void epd_refresh_full(const unsigned char* data);
void epd_init_part(void);
void epd_refresh_partial(unsigned char *gnDisBuf);

#endif //_EPD_W21_C02_H_
