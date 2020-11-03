#ifndef SOTTER_WALLET_V100_H
#define SOTTER_WALLET_V100_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

//LED
#define LEDS_NUMBER 2
#define LED_ORANGE NRF_GPIO_PIN_MAP(1, 2)
#define LED_BLUE NRF_GPIO_PIN_MAP(1, 4)
#define LEDS_ACTIVE_STATE 0
#define LEDS_LIST                                                              \
  { LED_ORANGE, LED_BLUE }
#define LEDS_INV_MASK LEDS_MASK

//Button
#define BUTTONS_NUMBER 2
#define BUTTON_CONFIRM NRF_GPIO_PIN_MAP(0, 4)
#define BUTTON_POWER NRF_GPIO_PIN_MAP(0, 6)
#define BUTTON_PULL NRF_GPIO_PIN_PULLUP
#define BUTTONS_ACTIVE_STATE 0
#define BUTTONS_LIST                                                           \
  { BUTTON_CONFIRM, BUTTON_POWER }

#define BUTTON_CONFIRM_INDEX 0
#define BUTTON_POWER_INDEX 1

//Display
#define EPD_W21_C02 1

#define EPD_Wxx_MOSI_PIN    NRF_GPIO_PIN_MAP(0, 12)
#define EPD_Wxx_CLK_PIN     NRF_GPIO_PIN_MAP(1, 9)
#define EPD_Wxx_CS_PIN      NRF_GPIO_PIN_MAP(0, 8)
#define EPD_Wxx_DC_PIN      NRF_GPIO_PIN_MAP(0, 13)
#define EPD_Wxx_RST_PIN     NRF_GPIO_PIN_MAP(0, 15)
#define EPD_Wxx_BS_PIN      NRF_GPIO_PIN_MAP(0, 17)

//ADC
#define ADC_CHN_BATTERY 7

//Crypto
#define  ECC608_I2C_SCL NRF_GPIO_PIN_MAP(0, 2)
#define  ECC608_I2C_SDA NRF_GPIO_PIN_MAP(0, 29)

//Font Library
#define  FONT_CS NRF_GPIO_PIN_MAP(1, 0)
#define  FONT_SCLK NRF_GPIO_PIN_MAP(0, 22)
#define  FONT_SO NRF_GPIO_PIN_MAP(0, 24)
#define  FONT_SI NRF_GPIO_PIN_MAP(0, 20)

//Charger Status
#define  CHG_STAT NRF_GPIO_PIN_MAP(1, 10)

//Accessories Power
#define  ACC_POWER NRF_GPIO_PIN_MAP(0, 26)


#ifdef __cplusplus
}
#endif

#endif // SOTTER_WALLET_V100_H
