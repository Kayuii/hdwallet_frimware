#include "driver/epd_w21_c02.h"

//#if (EPD_W21_C02)

char vcom = 0x55;

const unsigned char LUT_DATA[] = {
    0x80, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00,  // LUT0: BB:     VS 0 ~7
    0x10, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00,  // LUT1: BW:     VS 0 ~7
    0x80, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00,  // LUT2: WB:     VS 0 ~7
    0x10, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00,  // LUT3: WW:     VS 0 ~7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT4: VCOM:   VS 0 ~7

    0x03, 0x03, 0x00, 0x00, 0x02,  // TP0 A~D RP0
    0x09, 0x09, 0x00, 0x00, 0x02,  // TP1 A~D RP1
    0x03, 0x03, 0x00, 0x00, 0x02,  // TP2 A~D RP2
    0x00, 0x00, 0x00, 0x00, 0x00,  // TP3 A~D RP3
    0x00, 0x00, 0x00, 0x00, 0x00,  // TP4 A~D RP4
    0x00, 0x00, 0x00, 0x00, 0x00,  // TP5 A~D RP5
    0x00, 0x00, 0x00, 0x00, 0x00,  // TP6 A~D RP6

    0x15, 0x41, 0xA8, 0x32, 0x30, 0x0A,
};

const unsigned char LUT_DATA_PART[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT0: BB:     VS 0 ~7
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT1: BW:     VS 0 ~7
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT2: WB:     VS 0 ~7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT3: WW:     VS 0 ~7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // LUT4: VCOM:   VS 0 ~7

    0x0A, 0x00, 0x00, 0x00, 0x00,  // TP0 A~D RP0
    0x00, 0x00, 0x00, 0x00, 0x00,  // TP1 A~D RP1
    0x00, 0x00, 0x00, 0x00, 0x00,  // TP2 A~D RP2
    0x00, 0x00, 0x00, 0x00, 0x00,  // TP3 A~D RP3
    0x00, 0x00, 0x00, 0x00, 0x00,  // TP4 A~D RP4
    0x00, 0x00, 0x00, 0x00, 0x00,  // TP5 A~D RP5
    0x00, 0x00, 0x00, 0x00, 0x00,  // TP6 A~D RP6

    0x15, 0x41, 0xA8, 0x32, 0x30, 0x0A,
};

static void wait(void) {
    for (uint16_t i = 0; i < 4000; i++) {
        if (isEPD_Wxx_BUSY == 0) {
            return;
        }
        epd_wxx_hal_delay_ms(1);
    }
}

static inline void epd_update(void) {
    epd_wxx_hal_write_cmd(0x22);
    epd_wxx_hal_write_data_char(0xC7);
    epd_wxx_hal_write_cmd(0x20);
    wait();
}

static inline void epd_part_update(void) {
    epd_wxx_hal_write_cmd(0x22);
    epd_wxx_hal_write_data_char(0x0C);
    epd_wxx_hal_write_cmd(0x20);
    wait();
}

void epd_select_lut(unsigned char *wave_data) {
    unsigned char count;
    epd_wxx_hal_write_cmd(0x32);
    for (count = 0; count < 70; count++)
        epd_wxx_hal_write_data_char(wave_data[count]);
}

void EPD_DeepSleep(void) {
    epd_wxx_hal_write_cmd(0x22);  // POWER OFF
    epd_wxx_hal_write_data_char(0xC3);
    epd_wxx_hal_write_cmd(0x20);

    epd_wxx_hal_write_cmd(0x10);  // enter deep sleep
    epd_wxx_hal_write_data_char(0x01);
    epd_wxx_hal_delay_ms(100);
}

void epd_refresh_full(const unsigned char *data) {
    const unsigned char *data_flag;
    data_flag = data;
    epd_wxx_hal_write_cmd(0x24);  // Write Black and White image to RAM
    epd_wxx_hal_write_data_buffer(data, ALLSCREEN_GRAGHBYTES);
    data = data_flag;
    epd_wxx_hal_write_cmd(0x26);  // Write Read image to RAM
    epd_wxx_hal_write_data_buffer(data, ALLSCREEN_GRAGHBYTES);
    epd_update();
}

void epd_refresh_area(unsigned int x_start, unsigned int y_start,
                      const unsigned char *datas, unsigned int PART_COLUMN,
                      unsigned int PART_LINE) {
    uint16_t x_end, y_start1, y_start2, y_end1, y_end2;
    x_start = x_start >> 3;
    x_end = x_start + (PART_LINE >> 3) - 1;

    y_start1 = y_start >> 8;
    y_start2 = y_start & 0xff;
    y_end1 = (y_start + PART_COLUMN) >> 8;
    y_end2 = (y_start + PART_COLUMN) & 0xff;

    epd_wxx_hal_write_cmd(0x44);  // set RAM x address start/end, in page 35
    epd_wxx_hal_write_data_char(x_start);  // RAM x address start at 00h;
    epd_wxx_hal_write_data_char(
        x_end);                   // RAM x address end at 0fh(15+1)*8->128
    epd_wxx_hal_write_cmd(0x45);  // set RAM y address start/end, in page 35
    epd_wxx_hal_write_data_char(y_start2);  // RAM y address start at 0127h;
    epd_wxx_hal_write_data_char(y_start1);  // RAM y address start at 0127h;
    epd_wxx_hal_write_data_char(y_end2);    // RAM y address end at 00h;
    epd_wxx_hal_write_data_char(y_end1);    // ????=0

    epd_wxx_hal_write_cmd(0x4E);  // set RAM x address count to 0;
    epd_wxx_hal_write_data_char(x_start);
    epd_wxx_hal_write_cmd(0x4F);  // set RAM y address count to 0X127;
    epd_wxx_hal_write_data_char(y_start2);
    epd_wxx_hal_write_data_char(y_start1);

    epd_wxx_hal_write_cmd(0x24);  // Write Black and White image to RAM
    epd_wxx_hal_write_data_buffer(datas, PART_COLUMN * PART_LINE / 8);
    epd_part_update();
}

void epd_refresh_partial(unsigned char *gnDisBuf) {
    epd_refresh_area(0, 249, gnDisBuf, 250, 128);
}

void epd_init_full(void) {
    EPD_Wxx_RST_0;
    epd_wxx_hal_delay_ms(1);
    EPD_Wxx_RST_1;  // hard reset
    epd_wxx_hal_delay_ms(1);

    wait();
    epd_wxx_hal_write_cmd(0x12);  // soft reset
    wait();

    epd_wxx_hal_write_cmd(0x74);  // set analog block control
    epd_wxx_hal_write_data_char(0x54);
    epd_wxx_hal_write_cmd(0x7E);  // set digital block control
    epd_wxx_hal_write_data_char(0x3B);

    epd_wxx_hal_write_cmd(0x01);  // Driver output control
    epd_wxx_hal_write_data_char(0xF9);
    epd_wxx_hal_write_data_char(0x00);
    epd_wxx_hal_write_data_char(0x00);  // 00

    epd_wxx_hal_write_cmd(0x11);        // data entry mode
    epd_wxx_hal_write_data_char(0x01);  // 1

    epd_wxx_hal_write_cmd(0x44);  // set Ram-X address start/end position
    epd_wxx_hal_write_data_char(0x00);
    epd_wxx_hal_write_data_char(0x0F);  // 0x0C-->(15+1)*8=128

    epd_wxx_hal_write_cmd(0x45);        // set Ram-Y address start/end position
    epd_wxx_hal_write_data_char(0xF9);  // 0xF9-->(249+1)=250
    epd_wxx_hal_write_data_char(0x00);
    epd_wxx_hal_write_data_char(0x00);
    epd_wxx_hal_write_data_char(0x00);

    epd_wxx_hal_write_cmd(0x3C);  // BorderWavefrom
    epd_wxx_hal_write_data_char(0x03);

    epd_wxx_hal_write_cmd(0x2C);        // VCOM Voltage
    epd_wxx_hal_write_data_char(vcom);  //

    epd_wxx_hal_write_cmd(0x03);  //
    epd_wxx_hal_write_data_char(LUT_DATA[70]);

    epd_wxx_hal_write_cmd(0x04);  //
    epd_wxx_hal_write_data_char(LUT_DATA[71]);
    epd_wxx_hal_write_data_char(LUT_DATA[72]);
    epd_wxx_hal_write_data_char(LUT_DATA[73]);

    epd_wxx_hal_write_cmd(0x3A);  // Dummy Line
    epd_wxx_hal_write_data_char(LUT_DATA[74]);
    epd_wxx_hal_write_cmd(0x3B);  // Gate time
    epd_wxx_hal_write_data_char(LUT_DATA[75]);

    epd_select_lut((unsigned char *)LUT_DATA);  // LUT

    epd_wxx_hal_write_cmd(0x4E);  // set RAM x address count to 0;
    epd_wxx_hal_write_data_char(0x00);
    epd_wxx_hal_write_cmd(0x4F);        // set RAM y address count to 0X127;
    epd_wxx_hal_write_data_char(0xF9);  // F9
    epd_wxx_hal_write_data_char(0x00);
    wait();
}

void epd_init_part(void) {
    epd_wxx_hal_write_cmd(0x2C);        // VCOM Voltage
    epd_wxx_hal_write_data_char(0x26);  // 0x26

    wait();
    epd_select_lut((unsigned char *)LUT_DATA_PART);
    epd_wxx_hal_write_cmd(0x37);
    epd_wxx_hal_write_data_char(0x00);
    epd_wxx_hal_write_data_char(0x00);
    epd_wxx_hal_write_data_char(0x00);
    epd_wxx_hal_write_data_char(0x00);
    epd_wxx_hal_write_data_char(0x40);
    epd_wxx_hal_write_data_char(0x00);
    epd_wxx_hal_write_data_char(0x00);

    epd_wxx_hal_write_cmd(0x22);
    epd_wxx_hal_write_data_char(0xC0);
    epd_wxx_hal_write_cmd(0x20);
    wait();

    epd_wxx_hal_write_cmd(0x3C);  // BorderWavefrom
    epd_wxx_hal_write_data_char(0x01);
}

//#endif
