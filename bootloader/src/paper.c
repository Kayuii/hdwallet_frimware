#include "paper.h"

#include <string.h>

#include "boards.h""
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"

// #define PAPER_RST_PIN 13
// #define PAPER_DC_PIN  14

// #define PAPER_SPI_INSTANCE 0
// #define PAPER_SPI_SS_PIN   29
// #define PAPER_SPI_MOSI_PIN 4
// #define PAPER_SPI_SCK_PIN  3

// #define PAPER_RST_PIN 19
// #define PAPER_DC_PIN  18
// #define PAPER_SPI_INSTANCE 0
// #define PAPER_SPI_SS_PIN   20
// #define PAPER_SPI_MOSI_PIN 23
// #define PAPER_SPI_SCK_PIN  25

//#define EPD_Wxx_MOSI_PIN          NRF_GPIO_PIN_MAP(0, 12)
//#define EPD_Wxx_CLK_PIN           NRF_GPIO_PIN_MAP(1, 9)
//#define EPD_Wxx_CS_PIN            NRF_GPIO_PIN_MAP(0, 8)
//#define EPD_Wxx_DC_PIN            NRF_GPIO_PIN_MAP(0, 13)
//#define EPD_Wxx_RST_PIN           NRF_GPIO_PIN_MAP(0, 15)
//#define EPD_Wxx_BS_PIN            NRF_GPIO_PIN_MAP(0, 17)

#define PAPER_RST_PIN             EPD_Wxx_RST_PIN
#define PAPER_DC_PIN              EPD_Wxx_DC_PIN
#define PAPER_SPI_INSTANCE        1
#define PAPER_SPI_SS_PIN          EPD_Wxx_CS_PIN
#define PAPER_SPI_MOSI_PIN        EPD_Wxx_MOSI_PIN
#define PAPER_SPI_SCK_PIN         EPD_Wxx_CLK_PIN



#define PAPER_BYTE_WIDTH ((PAPER_PIXEL_WIDTH % 8 == 0) ? PAPER_PIXEL_WIDTH / 8 : PAPER_PIXEL_WIDTH / 8 + 1)
#define PAPER_BYTE_HEIGHT PAPER_PIXEL_HEIGHT
#define PAPER_BUFLEN PAPER_BYTE_WIDTH * PAPER_BYTE_HEIGHT

#define PAPER_CMD_PWR  0x01
#define PAPER_CMD_BTST 0x06
#define PAPER_CMD_PON  0x04
#define PAPER_CMD_PSR  0x00
#define PAPER_CMD_PLL  0x30
#define PAPER_CMD_TRES 0x61
#define PAPER_CMD_VDCD 0x82
#define PAPER_CMD_VCOM 0x50
#define PAPER_CMD_LUT_VCDC 0x20
#define PAPER_CMD_LUT_WW 0x21
#define PAPER_CMD_LUT_BW 0x22
#define PAPER_CMD_LUT_WB 0x23
#define PAPER_CMD_LUT_BB 0x24
#define PAPER_CMD_DISP 0x12

static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(PAPER_SPI_INSTANCE);

static uint8_t paper_cmd_pwr[] = {
  PAPER_CMD_PWR,
  0x03, 0x00, 0x2b, 0x2b, 0x03,
};

static uint8_t paper_cmd_btst[] = {
  PAPER_CMD_BTST,
  0x17, 0x17, 0x17,
};

static uint8_t paper_cmd_pon[] = {
  PAPER_CMD_PON,
};

static uint8_t paper_cmd_psr[] = {
  PAPER_CMD_PSR,
  0xbf, 0x0d,
};

static uint8_t paper_cmd_pll[] = {
  PAPER_CMD_PLL,
  0x3a,
};

static uint8_t paper_cmd_tres[] = {
  PAPER_CMD_TRES,
  PAPER_PIXEL_WIDTH,
  (PAPER_PIXEL_HEIGHT >> 8) & 0xff,
  PAPER_PIXEL_HEIGHT & 0xff,
};

static uint8_t paper_cmd_vdcd[] = {
  PAPER_CMD_VDCD,
  0x28,
};

static uint8_t paper_cmd_vcom[] = {
  PAPER_CMD_VCOM,
  0x97
};

static uint8_t paper_cmd_lut_vcdc[] = {
  PAPER_CMD_LUT_VCDC,
  0x00, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x60, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00,
};

static uint8_t paper_cmd_lut_ww[] = {
  PAPER_CMD_LUT_WW,
  0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
  0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static uint8_t paper_cmd_lut_bw[] = {
  PAPER_CMD_LUT_BW,
  0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x0F, 0x0F, 0x00, 0x00, 0x03,
  0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
  0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static uint8_t paper_cmd_lut_wb[] = {
  PAPER_CMD_LUT_WB,
  0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static uint8_t paper_cmd_lut_bb[] = {
  PAPER_CMD_LUT_BB,
  0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static uint8_t paper_cmd_disp[] = {
  PAPER_CMD_DISP,
};

static uint8_t paper_buffer[PAPER_BUFLEN];
static uint8_t paper_prev_buffer[PAPER_BUFLEN];

static void paper_begin_command(void) {
  nrf_gpio_pin_clear(PAPER_DC_PIN);
}

static void paper_begin_data(void) {
  nrf_gpio_pin_set(PAPER_DC_PIN);
}

static ret_code_t paper_tx_cmd(uint8_t *cmd, uint8_t length) {
  ret_code_t ret;

  paper_begin_command();
  ret = nrf_drv_spi_transfer(&spi, cmd, 1, NULL, 0);
  if (ret != NRF_SUCCESS) return ret;

  if (length - 1 > 0) {
    paper_begin_data();
    ret = nrf_drv_spi_transfer(&spi, cmd + 1, length - 1, NULL, 0);
    if (ret != NRF_SUCCESS) return ret;
  }

  return ret;
}

static void paper_reset(void) {
  nrf_gpio_pin_clear(PAPER_RST_PIN);
  nrf_delay_ms(200);
  nrf_gpio_pin_set(PAPER_RST_PIN);
  nrf_delay_ms(200);
}

static ret_code_t paper_tx_lut(void) {
  ret_code_t ret;

  ret = paper_tx_cmd(paper_cmd_vcom, sizeof(paper_cmd_vcom));
  if (ret != NRF_SUCCESS) return ret;

  ret = paper_tx_cmd(paper_cmd_lut_vcdc, sizeof(paper_cmd_lut_vcdc));
  if (ret != NRF_SUCCESS) return ret;

  ret = paper_tx_cmd(paper_cmd_lut_ww, sizeof(paper_cmd_lut_ww));
  if (ret != NRF_SUCCESS) return ret;

  ret = paper_tx_cmd(paper_cmd_lut_bw, sizeof(paper_cmd_lut_bw));
  if (ret != NRF_SUCCESS) return ret;

  ret = paper_tx_cmd(paper_cmd_lut_wb, sizeof(paper_cmd_lut_wb));
  if (ret != NRF_SUCCESS) return ret;

  ret = paper_tx_cmd(paper_cmd_lut_bb, sizeof(paper_cmd_lut_bb));
  if (ret != NRF_SUCCESS) return ret;

  return ret;
}

static ret_code_t _paper_display(void) {
  ret_code_t ret;

  /*
   * TODO transfer chunks of 256 bytes rather than calling nrf_drv_spi_transfer
   * for each byte.
   */

  uint8_t command[1];

  command[0] = 0x10;
  ret = paper_tx_cmd(command, 1);
  if (ret != NRF_SUCCESS) return ret;

  paper_begin_data();
  for (int i = 0; i < PAPER_BUFLEN; i++) {
    ret = nrf_drv_spi_transfer(&spi, paper_prev_buffer + i, 1, NULL, 0);
    if (ret != NRF_SUCCESS) return ret;
  }

  nrf_delay_ms(10);

  /*
   * TODO i'm not yet sure about the purpose of sending the previous buffer too.
   * just sending 0x00s works pretty good. when sending the actual previous
   * image buffer, it looks like sections that have not changed become kinda
   * dirty / greyish (which could be useful because one can use three colors).
   */

  // memcpy(paper_prev_buffer, paper_buffer, PAPER_BUFLEN);

  command[0] = 0x13;
  ret = paper_tx_cmd(command, 1);
  if (ret != NRF_SUCCESS) return ret;

  paper_begin_data();
  for (int i = 0; i < PAPER_BUFLEN; i++) {
    ret = nrf_drv_spi_transfer(&spi, paper_buffer + i, 1, NULL, 0);
    if (ret != NRF_SUCCESS) return ret;
  }

  nrf_delay_ms(10);

  ret = paper_tx_lut();
  if (ret != NRF_SUCCESS) return ret;

  ret = paper_tx_cmd(paper_cmd_disp, sizeof(paper_cmd_disp));
  if (ret != NRF_SUCCESS) return ret;

  nrf_delay_ms(100); // TODO use busy pin rather than arbitrary delays

  return ret;
}

ret_code_t paper_init(void) {
  ret_code_t ret;

  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
  spi_config.ss_pin   = PAPER_SPI_SS_PIN;
  spi_config.mosi_pin = PAPER_SPI_MOSI_PIN;
  spi_config.sck_pin  = PAPER_SPI_SCK_PIN;

  ret = nrf_drv_spi_init(&spi, &spi_config, NULL, NULL);
  if (ret != NRF_SUCCESS) return ret;

  nrf_gpio_cfg_output(PAPER_RST_PIN);
  nrf_gpio_cfg_output(PAPER_DC_PIN);

  paper_reset();

  ret = paper_tx_cmd(paper_cmd_pwr, sizeof(paper_cmd_pwr));
  if (ret != NRF_SUCCESS) return ret;

  ret = paper_tx_cmd(paper_cmd_btst, sizeof(paper_cmd_btst));
  if (ret != NRF_SUCCESS) return ret;

  ret = paper_tx_cmd(paper_cmd_pon, sizeof(paper_cmd_pon));
  if (ret != NRF_SUCCESS) return ret;

  ret = paper_tx_cmd(paper_cmd_psr, sizeof(paper_cmd_psr));
  if (ret != NRF_SUCCESS) return ret;

  ret = paper_tx_cmd(paper_cmd_pll, sizeof(paper_cmd_pll));
  if (ret != NRF_SUCCESS) return ret;

  ret = paper_tx_cmd(paper_cmd_tres, sizeof(paper_cmd_tres));
  if (ret != NRF_SUCCESS) return ret;

  ret = paper_tx_cmd(paper_cmd_vdcd, sizeof(paper_cmd_vdcd));
  if (ret != NRF_SUCCESS) return ret;

  memset(paper_buffer, 0xff, PAPER_BUFLEN);
  memset(paper_prev_buffer, 0x00, PAPER_BUFLEN);

  return ret;
}

void paper_uninit(void) {
  // TODO uninit spi and free buffers
}

void paper_pixel_draw(uint16_t x, uint16_t y, uint32_t color) {
  ASSERT(x < PAPER_PIXEL_WIDTH);
  ASSERT(y < PAPER_PIXEL_HEIGHT);

  int bitn = y * PAPER_PIXEL_WIDTH + x;
  int byten = bitn / 8;
  bitn = 8 - bitn % 8 - 1;

  if (color == 0) paper_buffer[byten] |= 1UL << (bitn);
  else paper_buffer[byten] &= ~(1UL << (bitn));
}

void paper_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  ASSERT(x + width < PAPER_PIXEL_WIDTH);
  ASSERT(y + height < PAPER_PIXEL_HEIGHT);

  for (uint16_t _x = x; _x < x + width; _x++)
    for (uint16_t _y = y; _y < y + height; _y++)
      paper_pixel_draw(_x, _y, color);
}

void paper_display(void) {
  APP_ERROR_CHECK(_paper_display());
}

void paper_rotation_set(nrf_lcd_rotation_t rotation) {
  UNUSED_PARAMETER(rotation);

  // TODO
}

void paper_display_invert(bool invert) {
  UNUSED_PARAMETER(invert);

  // TODO
}
