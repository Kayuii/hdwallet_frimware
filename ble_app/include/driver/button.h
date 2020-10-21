#ifndef BUTTON_H__
#define BUTTON_H__

#include <stdio.h>

#include "boards.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpio.h"

#include "app_error.h"
#include "app_util_platform.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

typedef void (*Handler)(void* context);
void button_set_confirm_on_press_handler(Handler handler, void* context);
void button_set_confirm_on_release_handler(Handler handler, void* context);
void button_set_confirm_on_longpush_handler(Handler handler, void* context);
void button_set_power_on_press_handler(Handler handler, void* context);
void button_set_power_on_release_handler(Handler handler, void* context);
void button_set_power_on_longpush_handler(Handler handler, void* context);

void button_init(void);
uint32_t button_enable(void);
uint32_t button_disable(void);

#endif
