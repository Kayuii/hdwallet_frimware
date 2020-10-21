#ifndef DISPLAY_MAIN_H
#define DISPLAY_MAIN_H

#include "epd_wxx_hal.h"


#if (EPD_W21)
#include "epd_w21.h"
#elif (EPD_W26)
#include "epd_w26.h"
#elif (EPD_W21_C02)
#include "epd_w21_c02.h"
#endif

void display_init(void);
void display_refresh(uint8_t flush);

#endif //DISPLAY_MAIN_H
