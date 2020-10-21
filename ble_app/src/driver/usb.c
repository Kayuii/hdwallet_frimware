#include "driver/usb.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "app_usbd.h"
#include "app_usbd_core.h"
#include "app_usbd_serial_num.h"
#include "app_usbd_hid_generic.h"
#include "app_util_platform.h"
#include "nrf.h"
#include "nrf_drv_usbd.h"

#define NRF_LOG_MODULE_NAME usb_driver
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

/**
 * @brief Enable USB power detection
 */
#ifndef USBD_POWER_DETECTION
#define USBD_POWER_DETECTION false
#endif

/**
 * @brief HID generic class interface number.
 * */

#define USB_INTERFACE_INDEX_MAIN 0
#define USB_INTERFACE_INDEX_U2F 1

/**
 * @brief HID generic class endpoints count.
 * */
#define HID_GENERIC_EP_COUNT 2

/**
 * @brief List of HID generic class endpoints.
 * */
#define HID_ENDPOINT_LIST() (ENDPOINT_ADDRESS_IN, ENDPOINT_ADDRESS_OUT)

#if USE_U2F
#define HID_U2F_ENDPOINT_LIST() \
    (ENDPOINT_ADDRESS_U2F_IN, ENDPOINT_ADDRESS_U2F_OUT)
#endif
/**
 * @brief Number of reports defined in report descriptor.
 */
#define REPORT_IN_QUEUE_SIZE 1

/**
 * @brief Size of maximum output report. HID generic class will reserve
 *        this buffer size + 1 memory space.
 *
 * Maximum value of this define is 63 bytes. Library automatically adds
 * one byte for report ID. This means that output report size is limited
 * to 64 bytes.
 */
#define REPORT_OUT_MAXSIZE 63

/**
 * @brief FIDO HID report descriptors.
 */
#define APP_USBD_HID_REPORT_DSC                             \
    {                                                       \
        0x06, 0x00, 0xff, /* USAGE_PAGE (Vendor Defined)	*/ \
        0x09, 0x01,       /* USAGE (1)       */             \
        0xa1, 0x01,       /* COLLECTION (Application)		*/   \
        0x09, 0x20,       /* USAGE (Input Report Data)		*/  \
        0x15, 0x00,       /* LOGICAL_MINIMUM (0)			*/       \
        0x26, 0xff, 0x00, /* LOGICAL_MAXIMUM (255)			*/     \
        0x75, 0x08,       /* REPORT_SIZE (8)				*/          \
        0x95, 0x40,       /* REPORT_COUNT (64)				*/        \
        0x81, 0x02,       /* INPUT (Data,Var,Abs)			*/      \
        0x09, 0x21,       /* USAGE (Output Report Data)	*/  \
        0x15, 0x00,       /* LOGICAL_MINIMUM (0)			*/       \
        0x26, 0xff, 0x00, /* LOGICAL_MAXIMUM (255)			*/     \
        0x75, 0x08,       /* REPORT_SIZE (8)				*/          \
        0x95, 0x40,       /* REPORT_COUNT (64)				*/        \
        0x91, 0x02,       /* OUTPUT (Data,Var,Abs)			*/     \
        0xc0              /* END_COLLECTION				*/           \
    };

#if USE_U2F
#define APP_USBD_HID_U2F_REPORT_DSC                                           \
    {                                                                         \
        0x06, 0xd0, 0xf1,     /*     Usage Page (FIDO Alliance),           */ \
            0x09, 0x01,       /*     Usage (U2F HID Authenticator Device), */ \
            0xa1, 0x01,       /*     Collection (Application),             */ \
            0x09, 0x20,       /*     Usage (Input Report Data),            */ \
            0x15, 0x00,       /*     Logical Minimum (0),                  */ \
            0x26, 0xff, 0x00, /*     Logical Maximum (255),                */ \
            0x75, 0x08,       /*     Report Size (8),                      */ \
            0x95, 0x40,       /*     Report Count (64),                    */ \
            0x81, 0x02,       /*     Input (Data, Variable, Absolute)      */ \
            0x09, 0x21,       /*     Usage (Output Report Data),           */ \
            0x15, 0x00,       /*     Logical Minimum (0),                  */ \
            0x26, 0xff, 0x00, /*     Logical Maximum (255),                */ \
            0x75, 0x08,       /*     Report Size (8),                      */ \
            0x95, 0x40,       /*     Report Count (64),                    */ \
            0x91, 0x02,       /*     Output (Data, Variable, Absolute)     */ \
            0xc0              /*     End Collection,                       */ \
    }
#endif

#define USBD_STACK_SIZE 1024
#define USBD_PRIORITY 2
#define USB_THREAD_MAX_BLOCK_TIME portMAX_DELAY

static TaskHandle_t usb_task_handle;

static volatile bool u2f_transport = false;

void usb_set_u2f_transport(void) { u2f_transport = true; }

void usb_set_hid_transport(void) { u2f_transport = false; }

bool usb_is_u2f_transport(void) { return u2f_transport; }

static volatile char tiny = 0;
char usbTiny(char set) {
    char old = tiny;
    tiny = set;
    return old;
}

static transport_message_t *rx_message = NULL;
static SemaphoreHandle_t *rx_semaphore = NULL;

/**
 * @brief User event handler.
 * */
static void hid_user_ev_handler(app_usbd_class_inst_t const *p_inst,
                                app_usbd_hid_user_event_t event);

/**
 * @brief Reuse HID mouse report descriptor for HID generic class
 */
APP_USBD_HID_GENERIC_SUBCLASS_REPORT_DESC(hid_desc, APP_USBD_HID_REPORT_DSC);
static const app_usbd_hid_subclass_desc_t *hid_reports[] = {&hid_desc};

#if USE_U2F
APP_USBD_HID_GENERIC_SUBCLASS_REPORT_DESC(hid_u2f_desc,
                                          APP_USBD_HID_U2F_REPORT_DSC);
static const app_usbd_hid_subclass_desc_t *hid_u2f_reports[] = {&hid_u2f_desc};
#endif

/*lint -save -e26 -e64 -e123 -e505 -e651*/

/**
 * @brief Global HID generic instance
 */
APP_USBD_HID_GENERIC_GLOBAL_DEF(m_app_hid, USB_INTERFACE_INDEX_MAIN,
                                hid_user_ev_handler, HID_ENDPOINT_LIST(),
                                hid_reports, REPORT_IN_QUEUE_SIZE,
                                REPORT_OUT_MAXSIZE, APP_USBD_HID_SUBCLASS_NONE,
                                APP_USBD_HID_PROTO_GENERIC);

#if USE_U2F
APP_USBD_HID_GENERIC_GLOBAL_DEF(m_app_hid_u2f, USB_INTERFACE_INDEX_U2F,
                                hid_user_ev_handler, HID_U2F_ENDPOINT_LIST(),
                                hid_u2f_reports, REPORT_IN_QUEUE_SIZE,
                                REPORT_OUT_MAXSIZE, APP_USBD_HID_SUBCLASS_NONE,
                                APP_USBD_HID_PROTO_GENERIC);
#endif
/*lint -restore*/

/**
 * @brief Mark the ongoing transmission
 * Marks that the report buffer is busy and cannot be used until
 * transmission finishes or invalidates (by USB reset or suspend event).
 */
static bool m_report_pending[16] = {false};

/**
 * @brief Mark a report received.
 **/
static bool m_report_received[16] = {false};

static bool m_usb_init_stat = false;

void usb_set_rx_parameters(transport_message_t *msg,
                           SemaphoreHandle_t *semaphore) {
    rx_message = msg;
    rx_semaphore = semaphore;
}

/**
 * @brief Class specific event handler.
 *
 * @param p_inst    Class instance.
 * @param event     Class specific event.
 * */
static void hid_user_ev_handler(app_usbd_class_inst_t const *p_inst,
                                app_usbd_hid_user_event_t event) {
    uint8_t interface_idx = p_inst->iface.config[0];
    switch (event) {
        case APP_USBD_HID_USER_EVT_OUT_REPORT_READY: {
            m_report_received[interface_idx] = true;
            break;
        }
        case APP_USBD_HID_USER_EVT_IN_REPORT_DONE: {
            m_report_pending[interface_idx] = false;
            break;
        }
        case APP_USBD_HID_USER_EVT_SET_BOOT_PROTO: {
            UNUSED_RETURN_VALUE(hid_generic_clear_buffer(p_inst));
            NRF_LOG_INFO("SET_BOOT_PROTO");
            break;
        }
        case APP_USBD_HID_USER_EVT_SET_REPORT_PROTO: {
            UNUSED_RETURN_VALUE(hid_generic_clear_buffer(p_inst));
            NRF_LOG_INFO("SET_REPORT_PROTO");
            break;
        }
        default:
            break;
    }
}

/**
 * @brief USBD library specific event handler.
 *
 * @param event     USBD library event.
 * */
static void usbd_user_ev_handler(app_usbd_event_type_t event) {
    switch (event) {
        case APP_USBD_EVT_DRV_SOF:
            break;
        case APP_USBD_EVT_DRV_RESET:
            memset(m_report_pending, 0, sizeof(m_report_pending));
            break;
        case APP_USBD_EVT_DRV_SUSPEND:
            memset(m_report_pending, 0, sizeof(m_report_pending));
            app_usbd_suspend_req();
            break;
        case APP_USBD_EVT_DRV_RESUME:
            memset(m_report_pending, 0, sizeof(m_report_pending));
            break;
        case APP_USBD_EVT_STARTED:
            m_usb_init_stat = true;
            memset(m_report_pending, 0, sizeof(m_report_pending));
            break;
        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
            m_usb_init_stat = false;
            break;
        case APP_USBD_EVT_POWER_DETECTED:
            NRF_LOG_INFO("USB power detected");
            if (!nrf_drv_usbd_is_enabled()) {
                app_usbd_enable();
            }
            break;
        case APP_USBD_EVT_POWER_REMOVED:
            NRF_LOG_INFO("USB power removed");
            app_usbd_stop();
            m_usb_init_stat = false;
            break;
        case APP_USBD_EVT_POWER_READY:
            NRF_LOG_INFO("USB ready");
            m_usb_init_stat = true;
            app_usbd_start();
            break;
        default:
            break;
    }
}

/**
 * @brief handler for idle reports.
 *
 * @param p_inst      Class instance.
 * @param report_id   Number of report ID that needs idle transfer.
 * */
static ret_code_t idle_handle(app_usbd_class_inst_t const *p_inst,
                              uint8_t report_id) {
    if (report_id == 0) {
        uint8_t interface_idx = p_inst->iface.config[0];
        uint8_t report[] = {0xBE, 0xEF};
        switch (interface_idx) {
            case USB_INTERFACE_INDEX_MAIN:
                return app_usbd_hid_generic_idle_report_set(&m_app_hid, report,
                                                            sizeof(report));
                break;
#if USE_U2F
            case USB_INTERFACE_INDEX_U2F:
                return app_usbd_hid_generic_idle_report_set(
                    &m_app_hid_u2f, report, sizeof(report));
                break;
#endif

            default:
                return NRF_ERROR_NOT_SUPPORTED;
        }
    } else {
        return NRF_ERROR_NOT_SUPPORTED;
    }

    return NRF_SUCCESS;
}

/*
 * hid_rx_callback() - Callback function to process received packet from USB
 * host
 *
 * INPUT
 *     - dev: pointer to USB device handler
 *     - ep: unused
 * OUTPUT
 *     none
 */
static void hid_rx_callback(uint8_t *pkt, uint8_t len) {
    /* Receive into the message buffer. */
    if (pkt != NULL && len > 0 && rx_message) {
        rx_message->len = len;
        rx_message->link_type = LINK_TYPE_USB;
        memcpy(rx_message->message, pkt, len);
        usb_set_hid_transport();
        xSemaphoreGive(*rx_semaphore);
    }
}

#if USE_U2F
static void hid_u2f_rx_callback(uint8_t *pkt, uint8_t len) {
    if (len != USB_SEGMENT_SIZE) {
        return;
    }

    static uint8_t buf[64] __attribute__((aligned(4)));
    if (pkt != NULL && len > 0) {
        memcpy(buf, pkt, len);
        usb_set_u2f_transport();
        // u2fhid_read(tiny, (const U2FHID_FRAME *)(void*)buf);
    }
}
#endif

static inline uint8_t hid_recv() {
    uint8_t *p_recv_buf = NULL;
    size_t recv_size = 0;

    if (m_report_received[USB_INTERFACE_INDEX_MAIN]) {
        m_report_received[USB_INTERFACE_INDEX_MAIN] = false;
        p_recv_buf = (uint8_t *)app_usbd_hid_generic_out_report_get(&m_app_hid,
                                                                    &recv_size);
        if (recv_size > 0) {
            // hid recv callback;
            hid_rx_callback(p_recv_buf, recv_size);
        }
    }

#if USE_U2F
    if (m_report_received[USB_INTERFACE_INDEX_U2F]) {
        m_report_received[USB_INTERFACE_INDEX_U2F] = false;
        p_recv_buf = (uint8_t *)app_usbd_hid_generic_out_report_get(
            &m_app_hid_u2f, &recv_size);
        if (recv_size > 0) {
            // u2f recv callback
            hid_u2f_rx_callback(p_recv_buf, recv_size);
        }
    }
#endif

    return NRF_SUCCESS;
}

void usb_new_event_isr_handler(app_usbd_internal_evt_t const *const p_event,
                               bool queued) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    UNUSED_PARAMETER(p_event);
    UNUSED_PARAMETER(queued);
    ASSERT(usb_task_handle != NULL);
    /* Release the semaphore */
    vTaskNotifyGiveFromISR(usb_task_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void usb_task(void *arg) {
    UNUSED_PARAMETER(arg);
    NRF_LOG_DEBUG("Starting usb task...");
    ret_code_t ret;
    static const app_usbd_config_t usbd_config = {
        .ev_isr_handler = usb_new_event_isr_handler,
        .ev_state_proc = usbd_user_ev_handler};

    ret = app_usbd_init(&usbd_config);
    APP_ERROR_CHECK(ret);

    app_usbd_class_inst_t const *class_inst_hid;
    class_inst_hid = app_usbd_hid_generic_class_inst_get(&m_app_hid);

    ret = hid_generic_idle_handler_set(class_inst_hid, idle_handle);
    APP_ERROR_CHECK(ret);

    ret = app_usbd_class_append(class_inst_hid);
    APP_ERROR_CHECK(ret);

#if USE_U2F
    app_usbd_class_inst_t const *class_inst_hid_u2f;
    class_inst_hid_u2f = app_usbd_hid_generic_class_inst_get(&m_app_hid_u2f);

    ret = hid_generic_idle_handler_set(class_inst_hid_u2f, idle_handle);
    APP_ERROR_CHECK(ret);

    ret = app_usbd_class_append(class_inst_hid_u2f);
    APP_ERROR_CHECK(ret);
#endif
    if (USBD_POWER_DETECTION) {
        ret = app_usbd_power_events_enable();
        APP_ERROR_CHECK(ret);
    } else {
        NRF_LOG_INFO("No USB power detection enabled. Starting USB now");

        app_usbd_enable();
        app_usbd_start();
    }

    // Set the first event to make sure that USB queue is processed after it is
    // started
    UNUSED_RETURN_VALUE(xTaskNotifyGive(xTaskGetCurrentTaskHandle()));
    // Enter main loop.

    for (;;) {
        /* Waiting for event */
        UNUSED_RETURN_VALUE(
            ulTaskNotifyTake(pdTRUE, USB_THREAD_MAX_BLOCK_TIME));
        usb_poll();
    }
}

void usb_poll(void) {
    while (app_usbd_event_queue_process()) {
        /* Nothing to do */
    }
    hid_recv();
}

void usb_init(void) {
    app_usbd_serial_num_generate();
    if (pdPASS != xTaskCreate(usb_task, "USBD", USBD_STACK_SIZE, NULL,
                              USBD_PRIORITY, &usb_task_handle)) {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
}

bool get_usb_init_stat() { return m_usb_init_stat; }

static inline bool usb_tx_helper(uint8_t *message, uint16_t len,
                                 uint8_t endpoint) {
    const app_usbd_hid_generic_t *hid_generic = NULL;
    uint8_t interface_index = USB_INTERFACE_INDEX_MAIN;
    switch (endpoint) {
        case ENDPOINT_ADDRESS_IN:
            hid_generic = &m_app_hid;
            interface_index = USB_INTERFACE_INDEX_MAIN;
            break;

#if USE_U2F
        case ENDPOINT_ADDRESS_U2F_IN:
            hid_generic = &m_app_hid_u2f;
            interface_index = USB_INTERFACE_INDEX_U2F;
            break;
#endif

        default:
            return false;
    }

    uint32_t pos = 1;
    /* Chunk out message */
    while (pos < len) {
        uint8_t tmp_buffer[USB_SEGMENT_SIZE] = {0};
        tmp_buffer[0] = '?';
        memcpy(tmp_buffer + 1, message + pos, USB_SEGMENT_SIZE - 1);

        while (m_report_pending[interface_index]) {
            while (app_usbd_event_queue_process()) {
                /* Nothing to do */
            }
        }

        while (app_usbd_hid_generic_in_report_set(
                   hid_generic, tmp_buffer, USB_SEGMENT_SIZE) != NRF_SUCCESS) {
            // do nothing
        };

        m_report_pending[interface_index] = true;

        pos += USB_SEGMENT_SIZE - 1;
    }

    return true;
}

#if USE_U2F
bool usb_u2f_tx_helper(uint8_t *message, uint16_t len, uint8_t endpoint) {
    const app_usbd_hid_generic_t *hid_generic = NULL;
    uint8_t interface_index = USB_INTERFACE_INDEX_MAIN;

    switch (endpoint) {
        case ENDPOINT_ADDRESS_IN:
            hid_generic = &m_app_hid;
            interface_index = USB_INTERFACE_INDEX_MAIN;
            break;
        case ENDPOINT_ADDRESS_U2F_IN:
            hid_generic = &m_app_hid_u2f;
            interface_index = USB_INTERFACE_INDEX_U2F;
            break;
        default:
            return false;
    }

    uint32_t pos = 0;
    /* Chunk out message */
    while (pos < len) {
        uint8_t tmp_buffer[USB_SEGMENT_SIZE] = {0};
        memcpy(tmp_buffer, message + pos, USB_SEGMENT_SIZE);
        while (m_report_pending[interface_index]) {
            while (app_usbd_event_queue_process()) {
                /* Nothing to do */
            }
        }

        while (app_usbd_hid_generic_in_report_set(
                   hid_generic, tmp_buffer, USB_SEGMENT_SIZE) != NRF_SUCCESS) {
            // do nothing
        };

        m_report_pending[interface_index] = true;
        pos += USB_SEGMENT_SIZE;
    }
    return (true);
}
#endif

bool usb_tx(uint8_t *message, uint16_t len) {
    if (usb_is_u2f_transport()) {
        memcpy(message + len, "\x00\x00\x90\x00", 4);
        // send_u2f_msg(message, len + 4);
        return true;
    } else {
        return usb_tx_helper(message, len, ENDPOINT_ADDRESS_IN);
    }
}