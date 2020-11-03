#ifndef CUSTOM_BOARD_DK_H
#define CUSTOM_BOARD_DK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

//Display
#define EPD_W21_C02 1

//#define EPD_Wxx_MOSI_PIN          NRF_GPIO_PIN_MAP(0, 12)
//#define EPD_Wxx_CLK_PIN           NRF_GPIO_PIN_MAP(1, 9)
//#define EPD_Wxx_CS_PIN            NRF_GPIO_PIN_MAP(0, 8)
//#define EPD_Wxx_DC_PIN            NRF_GPIO_PIN_MAP(0, 13)
//#define EPD_Wxx_RST_PIN           NRF_GPIO_PIN_MAP(0, 15)
//#define EPD_Wxx_BS_PIN            NRF_GPIO_PIN_MAP(0, 17)

//#define EPD_Wxx_MOSI_PIN            ARDUINO_11_PIN
//#define EPD_Wxx_CLK_PIN             ARDUINO_13_PIN
//#define EPD_Wxx_CS_PIN              ARDUINO_10_PIN
//#define EPD_Wxx_DC_PIN              ARDUINO_9_PIN
//#define EPD_Wxx_RST_PIN             ARDUINO_8_PIN
//#define EPD_Wxx_BS_PIN              ARDUINO_7_PIN

//#define EPD_Wxx_MOSI_PIN            23
//#define EPD_Wxx_CLK_PIN             25
//#define EPD_Wxx_CS_PIN              20
//#define EPD_Wxx_DC_PIN              18
//#define EPD_Wxx_RST_PIN             19
//#define EPD_Wxx_BS_PIN              17

#define EPD_Wxx_MOSI_PIN            NRF_GPIO_PIN_MAP(0,21)
#define EPD_Wxx_CLK_PIN             NRF_GPIO_PIN_MAP(0,23)
#define EPD_Wxx_CS_PIN              NRF_GPIO_PIN_MAP(0,20)
#define EPD_Wxx_DC_PIN              NRF_GPIO_PIN_MAP(0,18)
#define EPD_Wxx_RST_PIN             NRF_GPIO_PIN_MAP(0,19)
#define EPD_Wxx_BS_PIN              NRF_GPIO_PIN_MAP(0,17)

//#define PAPER_RST_PIN 19
//#define PAPER_DC_PIN  18
//#define PAPER_BUSY_PIN 17

//#define PAPER_SPI_INSTANCE 0
//#define PAPER_SPI_SS_PIN   20
//#define PAPER_SPI_MOSI_PIN 23
//#define PAPER_SPI_SCK_PIN  25


#define  ACC_POWER NRF_GPIO_PIN_MAP(0, 16)

    //nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    //spi_config.ss_pin = NRF_DRV_SPI_PIN_NOT_USED;
    //spi_config.mosi_pin = EPD_Wxx_MOSI_PIN;
    //spi_config.sck_pin = EPD_Wxx_CLK_PIN;
    //spi_config.frequency = SPIM_FREQUENCY_FREQUENCY_M16;

#ifdef __cplusplus
}
#endif

#endif // CUSTOM_BOARD_DK_H
