#include "driver/button.h"
#include "app_button.h"
#include "app_timer.h"

#define BUTTON_DEBOUNCE_DELAY 50
#define BUTTON_ACTION_PUSH (APP_BUTTON_PUSH)
#define BUTTON_ACTION_RELEASE (APP_BUTTON_RELEASE)
#define BUTTON_ACTION_LONG_PUSH (2)
#define BUTTON_LONG_PUSH_TIMEOUT_MS (2200)

static void usr_button_event_handler(uint8_t pin_no, uint8_t button_action);
static void button_timer_handler(void *p_context);

static const app_button_cfg_t user_buttons[BUTTONS_NUMBER] = {
    {BUTTON_CONFIRM, BUTTONS_ACTIVE_STATE, BUTTON_PULL,
     usr_button_event_handler},
    {BUTTON_POWER, BUTTONS_ACTIVE_STATE, BUTTON_PULL, usr_button_event_handler},
};

APP_TIMER_DEF(m_button_timer);

static Handler button_confirm_on_press_handler = NULL;
static Handler button_confirm_on_release_handler = NULL;
static Handler button_confirm_on_longpush_handler = NULL;

static Handler button_power_on_press_handler = NULL;
static Handler button_power_on_release_handler = NULL;
static Handler button_power_on_longpush_handler = NULL;

static void *button_confirm_on_release_handler_context = NULL;
static void *button_confirm_on_press_handler_context = NULL;
static void *button_confirm_on_longpush_handler_context = NULL;

static void *button_power_on_release_handler_context = NULL;
static void *button_power_on_press_handler_context = NULL;
static void *button_power_on_longpush_handler_context = NULL;

static uint8_t m_current_long_push_pin_no;

void button_init(void) {
    uint32_t err_code = NRF_SUCCESS;

    button_confirm_on_press_handler = NULL;
    button_confirm_on_press_handler_context = NULL;

    button_confirm_on_release_handler = NULL;
    button_confirm_on_release_handler_context = NULL;

    button_confirm_on_longpush_handler = NULL;
    button_confirm_on_longpush_handler_context = NULL;

    err_code = app_button_init(user_buttons,
                               sizeof(user_buttons) / sizeof(app_button_cfg_t),
                               BUTTON_DEBOUNCE_DELAY);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_button_timer, APP_TIMER_MODE_SINGLE_SHOT,
                                button_timer_handler);
    APP_ERROR_CHECK(err_code);
    app_button_enable();
}

uint32_t button_enable(void) { return app_button_enable(); }

uint32_t button_disable(void) { return app_button_disable(); }

/**@brief Handle events from button timer.
 *
 * @param[in]   p_context   parameter registered in timer start function.
 */
static void button_timer_handler(void *p_context) {
    usr_button_event_handler(*(uint8_t *)p_context, BUTTON_ACTION_LONG_PUSH);
}

/*
 * Handler to be called when button is pushed.
 * param[in]   pin_no             The pin number where the event is genereated
 * param[in]   button_action     Is the button pushed or released
 */
static void usr_button_event_handler(uint8_t pin_no, uint8_t button_action) {
    uint32_t err_code = NRF_SUCCESS;

    switch (button_action) {
        case BUTTON_ACTION_PUSH:
            err_code = app_timer_start(
                m_button_timer, APP_TIMER_TICKS(BUTTON_LONG_PUSH_TIMEOUT_MS),
                (void *)&m_current_long_push_pin_no);

            if (err_code == NRF_SUCCESS) {
                m_current_long_push_pin_no = pin_no;
            }

            if (pin_no == BUTTON_CONFIRM) {
                if (button_confirm_on_press_handler) {
                    button_confirm_on_press_handler(
                        button_confirm_on_press_handler_context);
                }
            } else if (pin_no == BUTTON_POWER) {
                if (button_power_on_press_handler) {
                    button_power_on_press_handler(
                        button_power_on_press_handler_context);
                }
            }
            break;
        case BUTTON_ACTION_RELEASE:
            err_code = app_timer_stop(m_button_timer);
            if (pin_no == BUTTON_CONFIRM) {
                if (button_confirm_on_release_handler) {
                    button_confirm_on_release_handler(
                        button_confirm_on_release_handler_context);
                }
            } else if (pin_no == BUTTON_POWER) {
                if (button_power_on_release_handler) {
                    button_power_on_release_handler(
                        button_power_on_release_handler_context);
                }
            }
            break;
        case BUTTON_ACTION_LONG_PUSH:
            if (pin_no == BUTTON_CONFIRM) {
                NRF_LOG_DEBUG("confirm button long pushed.");
                if (button_confirm_on_longpush_handler) {
                    button_confirm_on_longpush_handler(
                        button_confirm_on_longpush_handler_context);
                }
            } else if (pin_no == BUTTON_POWER) {
                if (button_power_on_longpush_handler) {
                    button_power_on_longpush_handler(
                        button_power_on_longpush_handler_context);
                }
            }
            break;
        default:
            break;
    }
}

void button_set_confirm_on_press_handler(Handler handler, void *context) {
    button_confirm_on_press_handler = handler;
    button_confirm_on_press_handler_context = context;
}

void button_set_confirm_on_release_handler(Handler handler, void *context) {
    button_confirm_on_release_handler = handler;
    button_confirm_on_release_handler_context = context;
}

void button_set_confirm_on_longpush_handler(Handler handler, void *context) {
    button_confirm_on_longpush_handler = handler;
    button_confirm_on_longpush_handler_context = context;
}

void button_set_power_on_press_handler(Handler handler, void *context) {
    button_power_on_press_handler = handler;
    button_power_on_press_handler_context = context;
}

void button_set_power_on_release_handler(Handler handler, void *context) {
    button_power_on_release_handler = handler;
    button_power_on_release_handler_context = context;
}

void button_set_power_on_longpush_handler(Handler handler, void *context) {
    button_power_on_longpush_handler = handler;
    button_power_on_longpush_handler_context = context;
}
