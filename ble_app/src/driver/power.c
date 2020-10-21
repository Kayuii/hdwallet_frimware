#include "driver/power.h"
#include "app_error.h"
#include "boards.h"
#include "driver/button.h"
#include "driver/epd_wxx_hal.h"
#include "nrf_crypto_init.h"
#include "nrf_drv_power.h"
#include "nrf_gpio.h"
#include "nrfx_saadc.h"

const uint32_t REGOUT0 __attribute__((section(".uicr_regout0"))) =
    0xFFFFFFF0 | UICR_REGOUT0_VOUT_2V7;

void power_on(void) {
    nrf_gpio_cfg_output(ACC_POWER);
    nrf_gpio_pin_clear(ACC_POWER);
}

void power_off(void) {
    nrf_crypto_uninit();
    nrfx_saadc_uninit();
    epd_wxx_hal_uninit();

    button_disable();
    nrf_gpio_pin_sense_t sense =
        (BUTTONS_ACTIVE_STATE ? NRF_GPIO_PIN_SENSE_HIGH
                              : NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_set(BUTTON_POWER, sense);

    for (uint16_t p = 0; p < 48; p++) {
        if ((p != ACC_POWER) && (p != BUTTON_POWER)) {
            nrf_gpio_cfg_default(p);
        }
    }
    nrf_gpio_cfg_default(ACC_POWER);

    nrf_drv_power_uninit();
#ifdef DEBUG_NRF
    (void)sd_power_system_off();
    while (1)
        ;
#else
    APP_ERROR_CHECK(sd_power_system_off());
#endif  // DEBUG_NRF
}
