#ifndef BATTERY_H
#define BATTERY_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nrf.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_saadc.h"
#include "nrfx_ppi.h"

#include "app_error.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define NRF_SAADC_INPUT_AINx NRF_SAADC_INPUT_AIN7

typedef void (*battery_state_change_handler_t)(uint16_t percent,
                                               uint16_t charging);

void battery_init(void);
uint16_t battery_is_charging(void);
uint16_t battery_get_voltage(void);
void battery_set_state_change_handler(battery_state_change_handler_t handler);

#endif  // BATTERY_H