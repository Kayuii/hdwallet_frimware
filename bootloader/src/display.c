#include "sdk_common.h"
#include "display.h"

static uint8_t display_buffer[EPD_RES_ROW * EPD_RES_WIDTH];

extern const unsigned char gImage_dfu[];
extern const unsigned char gImage_soterwallet[];

void display_clear(void) {
    epd_init_full();
    for (uint16_t i = 0; i < EPD_FULLBUF_SIZE; i++) {
        display_buffer[i] = 0x00;
#if (DOT_WHITE_VALUE == 1)
        display_buffer[i] = ~display_buffer[i];
#endif
    }
    epd_refresh_full(display_buffer);
    epd_init_part();
}


void display_init(void) {
    epd_wxx_hal_init();
    display_clear();
    epd_refresh_area(32, 142, gImage_dfu, 32, 24);
    epd_refresh_area(66, 190, gImage_soterwallet, 130, 24);
    epd_part_update();
}

// static void displaydfuimage(void)
// {
//     displayclear();
//     epd_refresh_area(32, 142, gImage_dfu, 32, 24);
//     //epd_refresh_area(0, 250, gImage_dfu, 162, 24);
//     epd_refresh_area(66, 190, gImage_soterwallet, 130, 24);

    
//     // epd_refresh_area(99, 130, gImage_ccc, 65, 16);

    
//     // epd_refresh_area(0, 250, logo_data, 111, 14);

//     // 顶高   54     左宽      190        显示长      130   显示行数   24
//     // epd_refresh_area(66, 190, gImage_soterwallet, 130, 24);
//     //epd_refresh_area(32, 250, logo_data, 125, 22);
//     //epd_refresh_area(0, 232, gImage_aaa, 44, 24);
//     // epd_refresh_partial(gImage_aaa);
//     //gImage_fullscreen
    
//     epd_part_update();
// }

// static void displayclear(void)
// {
//     uint16_t i;
//     epd_wxx_hal_init();
//     epd_init_full();
//     //	epd_init_part();
//     for (i = 0; i < EPD_FULLBUF_SIZE; i++)
//         gImage_full[i] = 0x00;
//     epd_refresh_full(gImage_full);
//     epd_init_part();
//     epd_refresh_partial(gImage_full);
//     // epd_part_update();
// }