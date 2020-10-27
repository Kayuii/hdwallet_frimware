#ifndef EPD_WXX_HAL_H
#define EPD_WXX_HAL_H

#include <string.h>
#include "stdint.h"

#ifdef FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#endif

#include "boards.h"

#include "nrf_delay.h"
#include "nrf_gpio.h"

#include "app_error.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

//#define EPD_Wxx_SPI_SW

#define EPD_Wxx_MOSI_0 nrf_gpio_pin_clear(EPD_Wxx_MOSI_PIN)
#define EPD_Wxx_MOSI_1 nrf_gpio_pin_set(EPD_Wxx_MOSI_PIN)

#define EPD_Wxx_CLK_0 nrf_gpio_pin_clear(EPD_Wxx_CLK_PIN)
#define EPD_Wxx_CLK_1 nrf_gpio_pin_set(EPD_Wxx_CLK_PIN)

#define EPD_Wxx_CS_0 nrf_gpio_pin_clear(EPD_Wxx_CS_PIN)
#define EPD_Wxx_CS_1 nrf_gpio_pin_set(EPD_Wxx_CS_PIN)
#define EPD_Wxx_CS_Set(x) nrf_gpio_pin_write(EPD_Wxx_CS_PIN, x)

#define EPD_Wxx_DC_C nrf_gpio_pin_clear(EPD_Wxx_DC_PIN)
#define EPD_Wxx_DC_D nrf_gpio_pin_set(EPD_Wxx_DC_PIN)
#define EPD_Wxx_DC_Set(x) nrf_gpio_pin_write(EPD_Wxx_DC_PIN, x)

#define EPD_Wxx_RST_0 nrf_gpio_pin_clear(EPD_Wxx_RST_PIN)
#define EPD_Wxx_RST_1 nrf_gpio_pin_set(EPD_Wxx_RST_PIN)
#define EPD_Wxx_RST_Set(x) nrf_gpio_pin_write(EPD_Wxx_RST_PIN, x)

#define EPD_Wxx_BS_0 nrf_gpio_pin_clear(EPD_Wxx_BS_PIN)

#define isEPD_Wxx_BUSY nrf_gpio_pin_read(EPD_Wxx_BS_PIN)

#define EPD_Wxx_WRITE_DATA 1
#define EPD_Wxx_WRITE_CMD 0

void epd_wxx_hal_init(void);
void epd_wxx_hal_uninit(void);

void epd_wxx_hal_write_data_char(unsigned char command);
void epd_wxx_hal_write_cmd(unsigned char command);
void epd_wxx_hal_write_cmd_p1(unsigned char command, unsigned char para);
void epd_wxx_hal_write_cmd_p2(unsigned char command, unsigned char para1,
                              unsigned char para2);
void epd_wxx_hal_write_data_buffer(const uint8_t *buf, uint16_t len);
void epd_wxx_hal_delay_us(unsigned int xus);
void epd_wxx_hal_delay_ms(unsigned int xms);

#endif  //#ifndef EPD_WXX_HAL_H
