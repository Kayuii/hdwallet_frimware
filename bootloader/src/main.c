/**
 * Copyright (c) 2016 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup bootloader_secure_ble main.c
 * @{
 * @ingroup dfu_bootloader_api
 * @brief Bootloader project main file for secure DFU.
 *
 */

#include <stdint.h>
#include "boards.h"
#include "custom_board.h"
#include "nrf_mbr.h"
#include "nrf_bootloader.h"
#include "nrf_bootloader_app_start.h"
#include "nrf_bootloader_dfu_timers.h"
#include "nrf_dfu.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_error.h"
#include "app_error_weak.h"
#include "nrf_bootloader_info.h"
#include "nrf_delay.h"
#include "app_timer.h"

#include "epd_w21_c02.h"

extern const unsigned char gImage_dfu[];
extern const unsigned char gImage_soterwallet[];

unsigned char gImage_full[EPD_FULLBUF_SIZE];

// #define X_LEDS_LIST                        \
//     {                                      \
//         BSP_BOARD_LED_0, BSP_BOARD_LED_1, BSP_BOARD_LED_3, BSP_BOARD_LED_2 \
//     }

// static const uint8_t x_board_led_list[LEDS_NUMBER] = X_LEDS_LIST;
static uint16_t delay_t = 500;

static void on_error(void)
{
    NRF_LOG_FINAL_FLUSH();

#if NRF_MODULE_ENABLED(NRF_LOG_BACKEND_RTT)
    // To allow the buffer to be flushed by the host.
    nrf_delay_ms(100);
#endif
#ifdef NRF_DFU_DEBUG_VERSION
    NRF_BREAKPOINT_COND;
#endif
    NVIC_SystemReset();
}

void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t *p_file_name)
{
    NRF_LOG_ERROR("%s:%d", p_file_name, line_num);
    on_error();
}

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    NRF_LOG_ERROR("Received a fault! id: 0x%08x, pc: 0x%08x, info: 0x%08x", id, pc, info);
    on_error();
}

void app_error_handler_bare(uint32_t error_code)
{
    NRF_LOG_ERROR("Received an error: 0x%08x!", error_code);
    on_error();
}

static void led_flash(void)
{
    bsp_board_led_invert(BSP_BOARD_LED_0);
    bsp_board_led_invert(BSP_BOARD_LED_1);
}

// void marquee(void)
// {
//     for (int i = 0; i < LEDS_NUMBER; i++)
//     {
//         //bsp_board_leds_off();
//         //nrf_gpio_pin_toggle(x_board_led_list[i]);

//         if(bsp_board_led_state_get(x_board_led_list[i]))
//         {
//             bsp_board_led_off(x_board_led_list[i]);
//         }else{
//             bsp_board_led_on(x_board_led_list[i]);
//         }

//         //bsp_board_led_on(x_board_led_list[i]);
//         nrf_delay_ms(delay_t);
//         //bsp_board_led_off(x_board_led_list[i]);
//     }
// }

void displaydfuimage(void)
{
    displayclear();
    epd_refresh_area(32, 142, gImage_dfu, 32, 24);
    epd_refresh_area(66, 190, gImage_soterwallet, 130, 24);
    epd_part_update();
}

void displayclear(void)
{
    uint16_t i;
    epd_wxx_hal_init();
    epd_init_full();
    //	epd_init_part();
    for (i = 0; i < EPD_FULLBUF_SIZE; i++)
        gImage_full[i] = 0xFF;
    epd_refresh_full(gImage_full);
    epd_init_part();
    epd_refresh_partial(gImage_full);
    // epd_part_update();
}

/**
 * @brief Function notifies certain events in DFU process.
 */
static void dfu_observer(nrf_dfu_evt_type_t evt_type)
{
    switch (evt_type)
    {
    case NRF_DFU_EVT_DFU_FAILED:
    case NRF_DFU_EVT_DFU_ABORTED:
    case NRF_DFU_EVT_DFU_INITIALIZED:
        /**< Starting DFU. */
        // bsp_board_init(BSP_INIT_LEDS);
        //bsp_board_led_on(BSP_BOARD_LED_0);
        //bsp_board_led_on(BSP_BOARD_LED_1);
        nrf_bootloader_wdt_feed_timer_start(NRF_BOOTLOADER_MS_TO_TICKS(200), led_flash);

        nrf_gpio_cfg_output(ACC_POWER);
        nrf_gpio_pin_clear(ACC_POWER);
        nrf_delay_ms(100);

        displaydfuimage();

        // delay_t = 500;
        // marquee();
        //nrf_bootloader_wdt_feed_timer_start(NRF_BOOTLOADER_MS_TO_TICKS(200),
        //                                    marquee);
        //bsp_board_led_on(BSP_BOARD_LED_0);
        //bsp_board_led_on(BSP_BOARD_LED_1);
        //bsp_board_led_on(BSP_BOARD_LED_2);
        //bsp_board_led_on(BSP_BOARD_LED_3);
        //bsp_board_led_off(BSP_BOARD_LED_2);
        break;
    case NRF_DFU_EVT_TRANSPORT_ACTIVATED:
        /**< Transport activated (e.g. BLE connected, USB plugged in). */
        nrf_bootloader_wdt_feed_timer_start(NRF_BOOTLOADER_MS_TO_TICKS(400), led_flash);
        // delay_t = 200;
        // marquee();
        //nrf_bootloader_wdt_feed_timer_start(NRF_BOOTLOADER_MS_TO_TICKS(400),
        //                                    marquee);
        //bsp_board_led_off(BSP_BOARD_LED_1);
        //bsp_board_led_on(BSP_BOARD_LED_2);
        break;
    case NRF_DFU_EVT_TRANSPORT_DEACTIVATED:
        nrf_bootloader_wdt_feed_timer_start(NRF_BOOTLOADER_MS_TO_TICKS(200),
                                            led_flash);
        /**< Transport deactivated (e.g. BLE disconnected, USB plugged out). */
        // delay_t = 200;
        // marquee();
        break;
    case NRF_DFU_EVT_DFU_STARTED:
        break;
    default:
        break;
    }
}

/**@brief Function for application main entry. */
int main(void)
{
    uint32_t ret_val;

    bsp_board_voltage_init();
    bsp_board_init(BSP_INIT_LEDS);
    bsp_board_led_on(BSP_BOARD_LED_0);
    bsp_board_led_on(BSP_BOARD_LED_1);

    // Protect MBR and bootloader code from being overwritten.
    ret_val = nrf_bootloader_flash_protect(0, MBR_SIZE, false);
    APP_ERROR_CHECK(ret_val);
    ret_val = nrf_bootloader_flash_protect(BOOTLOADER_START_ADDR, BOOTLOADER_SIZE, false);
    APP_ERROR_CHECK(ret_val);

    (void)NRF_LOG_INIT(nrf_bootloader_dfu_timer_counter_get);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("Inside main");
    UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());

    // epd_wxx_hal_init();
    // epd_init_full();
    // epd_refresh_full(gImage_dfu);

    ret_val = nrf_bootloader_init(dfu_observer);
    APP_ERROR_CHECK(ret_val);

    // Either there was no DFU functionality enabled in this project or the DFU module detected
    // no ongoing DFU operation and found a valid main application.
    // Boot the main application.
    nrf_bootloader_app_start();

    // led_flash();

    // Should never be reached.
    NRF_LOG_INFO("After main");
}

/**
 * @}
 */
