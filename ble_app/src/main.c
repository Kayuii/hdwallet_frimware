#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "app_error.h"
#include "app_timer.h"
#include "board/layout.h"
#include "boards.h"
#include "driver/battery.h"
#include "driver/button.h"
#include "driver/display.h"
#include "driver/gt24l24a2y_app.h"
#include "driver/power.h"
#include "driver/soter_ble.h"
#include "driver/usb.h"
#include "fds.h"
#include "firmware/fsm.h"
#include "firmware/storage.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_crypto_init.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_power.h"
#include "nrf_drv_wdt.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_mpu.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_freertos.h"
#include "nrf_sdh_soc.h"
#include "nrf_stack_guard.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#define DEAD_BEEF                                                          \
    0xDEADBEEF /**< Value used as error code on stack dump, can be used to \
                  identify stack location on stack unwind. */

#if NRF_LOG_ENABLED
static TaskHandle_t logger_task_handle; /**< Definition of Logger thread. */
/**
 * The size of the stack for the Logger task (in 32-bit words).
 * Logger uses sprintf internally so it is a rather stack hungry process.
 */
#define LOGGER_STACK_SIZE 512
#define LOGGER_PRIORITY 1
#endif

#define MAIN_TASK_STACK 512
#define MAIN_TASK_PRIO 2
TaskHandle_t main_task_handle;

uint8_t g_new_home_status = LAYOUT_HOME;
uint8_t g_cur_home_status = LAYOUT_HOME;
char g_product_id[10];

nrf_drv_wdt_channel_id m_channel_id;

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product.
 * You need to analyze how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name) {
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void) {
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**
 * @brief WDT events handler.
 */
void wdt_event_handler(void)
{
    //NOTE: The max amount of time we can spend in WDT interrupt is two cycles of 32768[Hz] clock - after that, reset occurs
}
/**@brief Function for initializing the nrf log module.
 */
static void wdt_init(void) {
	uint32_t err_code = NRF_SUCCESS;
    //Configure WDT.
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
    config.reload_value = 20000;//20s
    err_code = nrf_drv_wdt_init(&config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
    nrf_drv_wdt_enable();
}

#if NRF_LOG_ENABLED
/**@brief Thread for handling the logger.
 *
 * @details This thread is responsible for processing log entries if logs are
 * deferred. Thread flushes all log entries and suspends. It is resumed by idle
 * task hook.
 *
 * @param[in]   arg   Pointer used for passing some arbitrary information
 * (context) from the osThreadCreate() call to the thread.
 */
static void logger_task(void *arg) {
    UNUSED_PARAMETER(arg);

    while (1) {
        NRF_LOG_FLUSH();
        vTaskSuspend(NULL);
    }
}
#endif  // NRF_LOG_ENABLED

/**@brief A function which is hooked to idle task.
 * @note Idle hook must be enabled in FreeRTOS configuration
 * (configUSE_IDLE_HOOK).
 */
void vApplicationIdleHook(void) {
#if NRF_LOG_ENABLED
    vTaskResume(logger_task_handle);
#endif
}

static inline void stack_guard_init(void) {
    APP_ERROR_CHECK(nrf_mpu_init());
    APP_ERROR_CHECK(nrf_stack_guard_init());
}

void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                   signed char *pcTaskName) {
    NRF_LOG_ERROR("Task %s  Stack Overflow!!!", pcTaskName);
    NRF_LOG_FLUSH();
}

void vApplicationMallocFailedHook(void) {
    NRF_LOG_ERROR("Malloc Failed!!!");
    NRF_LOG_FLUSH();
}

static void ble_pairing_pin_handler(char *pin) { layout_ble_pairing_code(pin); }

static void power_down(void) {
    soter_ble_set_pairing_pin_handler(NULL);
    battery_set_state_change_handler(NULL);
    canvas_clear();
    layout_home_flush();
    bsp_board_led_off(LED_BLUE_INDEX);
    bsp_board_led_off(LED_ORANGE_INDEX);
    vTaskDelay(2000);
    while (app_button_is_pushed(BUTTON_POWER_INDEX)) {
        vTaskDelay(10);
    }
    power_off();
}
/**@brief Function for initializing the clock.
 */
static void power_clock_init(void) {
    ret_code_t err_code;
    err_code = nrf_drv_power_init(NULL);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
}

static void battery_update(uint16_t percent, uint16_t charging) {
    if (percent <= 10) {
        power_down();
    }
    layout_battery_icon(percent, charging);
    soter_ble_update_battery_level(percent);
    layout_commit(false);
}

static inline bool is_system_off_reset(void) {
    uint32_t rr;
    APP_ERROR_CHECK(sd_power_reset_reason_get(&rr));
    APP_ERROR_CHECK(sd_power_reset_reason_clr(rr));
    if (0 != (rr & NRF_POWER_RESETREAS_OFF_MASK)) {
        return true;
    }
    return false;
}

static void handle_power_long_push(void *context) { power_down(); }

void main_task(void *params) {
    NRF_LOG_DEBUG("Starting main task...");
    bool ble_connected = false;
    button_init();
    zk_init();
    layout_init(canvas_init());
    display_init();
    storage_init();
    fsm_init();
    battery_init();

    if (is_system_off_reset()) {
        while (xTaskGetTickCount() < 1000) {
            vTaskDelay(10);
        }
        if (!app_button_is_pushed(BUTTON_POWER_INDEX)) {
            power_off();
        }
    }
    
    layout_ble_icon(ble_connected);
    layout_home();
    soter_ble_set_pairing_pin_handler(&ble_pairing_pin_handler);
    battery_set_state_change_handler(battery_update);

    bsp_board_init(BSP_INIT_LEDS);
    bsp_board_led_on(LED_BLUE_INDEX);
    bsp_board_led_on(LED_ORANGE_INDEX);

    while (app_button_is_pushed(BUTTON_POWER_INDEX)) {
        vTaskDelay(10);
    }

    button_set_power_on_longpush_handler(&handle_power_long_push, NULL);

    while (1) {

		nrf_drv_wdt_channel_feed(m_channel_id);

        if (ble_connected != soter_ble_connected()) {
            ble_connected = soter_ble_connected();
            layout_ble_icon(ble_connected);
            if (!ble_connected) {
                layout_home_flush();
            } else {
                layout_commit(false);
            }
        }
		
        if (g_cur_home_status != g_new_home_status) {
            g_cur_home_status = g_new_home_status;
            switch (g_new_home_status) {
                case LAYOUT_HOME: {
                    layout_home();
                } break;
                case LAYOUT_INFO: {
                    layout_product_info(MAJOR_VERSION, MINOR_VERSION,
                                        PATCH_VERSION);
                } break;
                default: { layout_home(); } break;
            }
		}

        if (xTaskGetTickCount() - layout_get_home_tick() > 300000) {
            power_down();
        }

        vTaskDelay(TIME_TASK_DELAY);
    }
}

/**@brief Function for application main entry.
 */
int main(void) {
    ret_code_t err_code;
    // Initialize modules.
    log_init();
    power_clock_init();
    stack_guard_init();

    uint16_t id = (uint16_t)NRF_FICR->DEVICEADDR[1] ^
                  (uint16_t)NRF_FICR->DEVICEADDR[0] ^
                  (uint16_t)(NRF_FICR->DEVICEADDR[0] >> 16);
    (void)snprintf((char *)g_product_id, 10, "SOTW_%04" PRIX16, id);

    power_on();
    err_code = nrf_crypto_init();
    APP_ERROR_CHECK(err_code);

    // Do not start any interrupt that uses system functions before system
    // initialisation. The best solution is to start the OS before any other
    // initalisation.

    wdt_init();

#if NRF_LOG_ENABLED
    // Start execution.
    if (pdPASS != xTaskCreate(logger_task, "LOGGER", LOGGER_STACK_SIZE, NULL,
                              LOGGER_PRIORITY, &logger_task_handle)) {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
#endif

    if (pdPASS != xTaskCreate(main_task, "MAIN", MAIN_TASK_STACK, (void *)NULL,
                              MAIN_TASK_PRIO, &main_task_handle)) {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }

    NRF_LOG_INFO("app main task");


    usb_init();
    soter_ble_init((char *)g_product_id);

    nrf_delay_ms(10000);
    
    // Activate deep sleep mode.
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    // Start FreeRTOS scheduler.
    vTaskStartScheduler();

    NRF_LOG_INFO("app main run");

    for (;;) {
        APP_ERROR_HANDLER(NRF_ERROR_FORBIDDEN);
    }
}
