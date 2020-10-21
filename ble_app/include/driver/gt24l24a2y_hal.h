#ifndef GT24L24A2Y_HAL_H
#define GT24L24A2Y_HAL_H

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

#define ZK_SPI_SW

#ifdef ZK_SPI_HW

#ifdef FREERTOS
extern SemaphoreHandle_t xSemaphore_gt24l24a2y_spi;
#endif

// V1.0

//#define GT24L24A2YL_CS_HW

#define GT24L24A2YL_SPI_SPEED SPIM_FREQUENCY_FREQUENCY_M4

#ifndef GT24L24A2YL_CS_HW
#define GT24L24A2Y_CS_0 nrf_gpio_pin_clear(FONT_CS)
#define GT24L24A2Y_CS_1 nrf_gpio_pin_set(FONT_CS)
#define GT24L24A2Y_CS_Set(x) nrf_gpio_pin_write(FONT_CS, x)
#else
#define GT24L24A2Y_CS_0
#define GT24L24A2Y_CS_1
#define GT24L24A2Y_CS_Set(x)
#endif

//#define EPD_Wxx_WRITE_DATA 1
//#define EPD_Wxx_WRITE_CMD 0

uint8_t gt24l24a2y_hal_SPI_WriteRead(uint8_t *wbuf, uint16_t wlen,
                                     uint8_t *rbuf, uint16_t rlen);

void gt24l24a2y_hal_init(void);
void gt24l24a2y_hal_Deinit(void);

void gt24l24a2y_hal_write_data_char(unsigned char command);
void gt24l24a2y_hal_write_cmd(unsigned char command);
void gt24l24a2y_hal_write_cmd_p1(unsigned char command, unsigned char para);
void gt24l24a2y_hal_write_cmd_p2(unsigned char command, unsigned char para1,
                                 unsigned char para2);
void gt24l24a2y_hal_write_data_buffer(uint8_t *buf, uint16_t len);
void gt24l24a2y_hal_delay_us(unsigned int xus);
void gt24l24a2y_hal_delay_ms(unsigned int xms);

void gt24l24a2y_hal_Test(void);

#elif defined(ZK_SPI_SW)

#define ZK_MOSI_PIN FONT_SI
#define ZK_SCK_PIN FONT_SCLK
#define ZK_CS_PIN FONT_CS
#define ZK_MISO_PIN FONT_SO

#define Rom_csH nrf_gpio_pin_write(ZK_CS_PIN, 1)
#define Rom_csL nrf_gpio_pin_write(ZK_CS_PIN, 0)
#define MOSIH nrf_gpio_pin_write(ZK_MOSI_PIN, 1)
#define MOSIL nrf_gpio_pin_write(ZK_MOSI_PIN, 0)
#define Rom_sckH nrf_gpio_pin_write(ZK_SCK_PIN, 1)
#define Rom_sckL nrf_gpio_pin_write(ZK_SCK_PIN, 0)

#define MISO nrf_gpio_pin_read(ZK_MISO_PIN)

void zk_init(void);

#endif

#endif  //#ifndef GT24L24A2Y_HAL_H
