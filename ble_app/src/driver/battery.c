#include "driver/battery.h"
#include "FreeRTOS.h"
#include "app_timer.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#define VOLTAGE_BUFFER_SIZE 4
static int16_t voltage_buffer[VOLTAGE_BUFFER_SIZE] = {0};
static uint8_t voltage_buffer_index = 0;

#define ADC_TIMER_INTERVAL_MS 250

APP_TIMER_DEF(m_adc_timer); // 定义指向定时器的      ID

#define BATTERY_VOLTAGE_PERCENTAGE_TABLE_SIZE 11
static const uint16_t
    BATTERY_VOLTAGE_PERCENTAGE[BATTERY_VOLTAGE_PERCENTAGE_TABLE_SIZE][2] = {
        {4200, 100}, {4100, 90}, {4000, 80}, {3900, 70}, {3800, 60}, {3700, 50}, {3600, 40}, {3550, 30}, {3500, 20}, {3400, 10}, {3300, 1}};

static nrf_saadc_value_t m_buffer;

static int16_t battery_voltage = 0;
static int16_t battery_percent = 0;
static bool charging = false;
static uint32_t last_charging_tick = 0;

static battery_state_change_handler_t battery_state_change_handler = NULL;

void battery_set_state_change_handler(battery_state_change_handler_t handler)
{
    battery_state_change_handler = handler;
}

static inline uint16_t battery_get_percent(uint16_t voltage)
{
    for (uint16_t i = 0; i < BATTERY_VOLTAGE_PERCENTAGE_TABLE_SIZE; i++)
    {
        if (voltage > BATTERY_VOLTAGE_PERCENTAGE[i][0])
        {
            if (i == 0)
            {
                return BATTERY_VOLTAGE_PERCENTAGE[0][1];
            }
            else
            {
                return BATTERY_VOLTAGE_PERCENTAGE[i - 1][1];
            }
        }
    }
    return BATTERY_VOLTAGE_PERCENTAGE[BATTERY_VOLTAGE_PERCENTAGE_TABLE_SIZE - 1]
                                     [1];
}

static void saadc_callback(nrf_drv_saadc_evt_t const *p_event)
{
    uint16_t new_percent;
    uint16_t new_charging;
    uint16_t need_update = 0;
    uint32_t sum;
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        ret_code_t err_code;

        err_code = nrfx_saadc_buffer_convert(p_event->data.done.p_buffer, 1);
        APP_ERROR_CHECK(err_code);

        voltage_buffer[voltage_buffer_index] =
            p_event->data.done.p_buffer[0] * 3600 * 28 / 1024 / 20;

        voltage_buffer_index++;
        if (voltage_buffer_index == VOLTAGE_BUFFER_SIZE)
        {
            voltage_buffer_index = 0;
            sum = 0;
            for (uint16_t i = 0; i < VOLTAGE_BUFFER_SIZE; i++)
            {
                sum += voltage_buffer[i];
            }
            battery_voltage = (int16_t)(sum / VOLTAGE_BUFFER_SIZE);
            new_percent = battery_get_percent(battery_voltage);
            if (new_percent != battery_percent)
            {
                need_update = true;
                battery_percent = new_percent;
            }

            new_charging = nrf_gpio_pin_read(CHG_STAT);
            if (charging != new_charging)
            {
                if (!new_charging)
                {
                    if (xTaskGetTickCount() - last_charging_tick > 2000)
                    {
                        need_update = true;
                        charging = false;
                    }
                }
                else
                {
                    need_update = true;
                    charging = true;
                    last_charging_tick = xTaskGetTickCount();
                }
            }
            if (need_update && battery_state_change_handler)
            {
                battery_state_change_handler(battery_percent, charging);
            }
        }
    }
}

static void adc_timer_handler(void *p_context)
{
    ret_code_t err_code = nrfx_saadc_sample();
    APP_ERROR_CHECK(err_code);
}

void battery_init(void)
{
    ret_code_t err_code;
    nrfx_saadc_config_t config = NRFX_SAADC_DEFAULT_CONFIG;
    nrf_saadc_channel_config_t channel_7_config =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AINx);
    channel_7_config.gain = NRF_SAADC_GAIN1_6;
    channel_7_config.reference = NRF_SAADC_REFERENCE_INTERNAL;
    err_code = nrfx_saadc_init(&config, saadc_callback);
    APP_ERROR_CHECK(err_code);
    err_code = nrfx_saadc_channel_init(ADC_CHN_BATTERY, &channel_7_config);
    APP_ERROR_CHECK(err_code);
    err_code = nrfx_saadc_buffer_convert(&m_buffer, 1);
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_create(&m_adc_timer, APP_TIMER_MODE_REPEATED,
                                (void *)&adc_timer_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_adc_timer,
                               APP_TIMER_TICKS(ADC_TIMER_INTERVAL_MS), NULL);
    APP_ERROR_CHECK(err_code);

    nrf_gpio_cfg_input(CHG_STAT, GPIO_PIN_CNF_PULL_Disabled);
    charging = nrf_gpio_pin_read(CHG_STAT);
}

uint16_t battery_is_charging(void) { return charging; }

uint16_t battery_get_voltage(void) { return battery_voltage; }
