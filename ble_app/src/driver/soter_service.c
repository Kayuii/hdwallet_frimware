#include "driver/soter_service.h"
#include "app_util.h"
#include "ble_srv_common.h"
#include "nordic_common.h"
#include "nrf_log.h"

#define NRF_LOG_MODULE_NAME soter_service
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

// 69996001-e8b3-11e8-9f32-f2801f1b9fd1
#define BLE_UUID_SOTER_SERVICE_BASE_UUID                                  \
    {                                                                     \
        0xd1, 0x9f, 0x1b, 0x1f, 0x80, 0xf2, 0x32, 0x9f, 0xe8, 0x11, 0xb3, \
            0xe8, 0x01, 0x60, 0x99, 0x69                                  \
    }
#define BLE_UUID_SOTER_SERVICE_UUID 0x6001

// 69996002-e8b3-11e8-9f32-f2801f1b9fd1
#define BLE_UUID_SOTER_RX_CHAR_BASE_UUID                                  \
    {                                                                     \
        0xd1, 0x9f, 0x1b, 0x1f, 0x80, 0xf2, 0x32, 0x9f, 0xe8, 0x11, 0xb3, \
            0xe8, 0x02, 0x60, 0x99, 0x69                                  \
    }
#define BLE_UUID_SOTER_RX_CHAR_UUID 0x6002

// 69996003-e8b3-11e8-9f32-f2801f1b9fd1
#define BLE_UUID_SOTER_TX_CHAR_BASE_UUID                                  \
    {                                                                     \
        0xd1, 0x9f, 0x1b, 0x1f, 0x80, 0xf2, 0x32, 0x9f, 0xe8, 0x11, 0xb3, \
            0xe8, 0x03, 0x60, 0x99, 0x69                                  \
    }
#define BLE_UUID_SOTER_TX_CHAR_UUID 0x6003

static void on_connect(ble_soter_service_t *p_soter_service,
                       ble_evt_t const *p_ble_evt) {
    ret_code_t err_code;
    ble_soter_service_evt_t evt;
    ble_gatts_value_t gatts_val;
    uint8_t cccd_value[2];
    ble_soter_client_context_t *p_client = NULL;

    err_code = blcm_link_ctx_get(p_soter_service->p_link_ctx_storage,
                                 p_ble_evt->evt.gap_evt.conn_handle,
                                 (void *)&p_client);
    if (err_code != NRF_SUCCESS) {
        NRF_LOG_ERROR(
            "Link context for 0x%02X connection handle could not be fetched.",
            p_ble_evt->evt.gap_evt.conn_handle);
    }

    /* Check the hosts CCCD value to inform of readiness to send data using the
     * RX characteristic */
    memset(&gatts_val, 0, sizeof(ble_gatts_value_t));
    gatts_val.p_value = cccd_value;
    gatts_val.len = sizeof(cccd_value);
    gatts_val.offset = 0;

    err_code = sd_ble_gatts_value_get(p_ble_evt->evt.gap_evt.conn_handle,
                                      p_soter_service->rx_handles.cccd_handle,
                                      &gatts_val);

    if ((err_code == NRF_SUCCESS) && (p_soter_service->data_handler != NULL) &&
        ble_srv_is_notification_enabled(gatts_val.p_value)) {
        if (p_client != NULL) {
            p_client->is_notification_enabled = true;
        }

        memset(&evt, 0, sizeof(ble_soter_service_evt_t));
        evt.type = BLE_SOTER_EVT_COMM_STARTED;
        evt.p_soter = p_soter_service;
        evt.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        evt.p_link_ctx = p_client;

        p_soter_service->data_handler(&evt);
    }
}

static void on_disconnect(ble_soter_service_t *p_soter_service,
                          ble_evt_t const *p_ble_evt) {
    UNUSED_PARAMETER(p_ble_evt);
    NRF_LOG_INFO("on disconnect\r\n");
}

static void on_write(ble_soter_service_t *p_soter_service,
                     ble_evt_t const *p_ble_evt) {
    ret_code_t err_code;
    ble_soter_service_evt_t evt;
    ble_soter_client_context_t *p_client;
    ble_gatts_evt_write_t const *p_evt_write =
        &p_ble_evt->evt.gatts_evt.params.write;

    err_code = blcm_link_ctx_get(p_soter_service->p_link_ctx_storage,
                                 p_ble_evt->evt.gatts_evt.conn_handle,
                                 (void *)&p_client);
    if (err_code != NRF_SUCCESS) {
        NRF_LOG_ERROR(
            "Link context for 0x%02X connection handle could not be fetched.",
            p_ble_evt->evt.gatts_evt.conn_handle);
    }

    memset(&evt, 0, sizeof(ble_soter_service_evt_t));
    evt.p_soter = p_soter_service;
    evt.conn_handle = p_ble_evt->evt.gatts_evt.conn_handle;
    evt.p_link_ctx = p_client;

    if ((p_evt_write->handle == p_soter_service->tx_handles.cccd_handle) &&
        (p_evt_write->len == 2)) {
        if (p_client != NULL) {
            if (ble_srv_is_notification_enabled(p_evt_write->data)) {
                p_client->is_notification_enabled = true;
                evt.type = BLE_SOTER_EVT_COMM_STARTED;
            } else {
                p_client->is_notification_enabled = false;
                evt.type = BLE_SOTER_EVT_COMM_STOPPED;
            }

            if (p_soter_service->data_handler != NULL) {
                p_soter_service->data_handler(&evt);
            }
        }
    } else if ((p_evt_write->handle ==
                p_soter_service->rx_handles.value_handle) &&
               (p_soter_service->data_handler != NULL)) {
        evt.type = BLE_SOTER_EVT_RX_DATA;
        evt.params.rx_data.p_data = p_evt_write->data;
        evt.params.rx_data.length = p_evt_write->len;

        p_soter_service->data_handler(&evt);
    } else {
        // Do Nothing. This event is not relevant for this service.
    }
}

static void on_hvx_tx_complete(ble_soter_service_t *p_soter,
                               ble_evt_t const *p_ble_evt) {
    ret_code_t err_code;
    ble_soter_service_evt_t evt;
    ble_soter_client_context_t *p_client;

    err_code = blcm_link_ctx_get(p_soter->p_link_ctx_storage,
                                 p_ble_evt->evt.gatts_evt.conn_handle,
                                 (void *)&p_client);
    if (err_code != NRF_SUCCESS) {
        NRF_LOG_ERROR(
            "Link context for 0x%02X connection handle could not be fetched.",
            p_ble_evt->evt.gatts_evt.conn_handle);
        return;
    }

    if (p_client->is_notification_enabled) {
        memset(&evt, 0, sizeof(ble_soter_service_evt_t));
        evt.type = BLE_SOTER_EVT_TX_RDY;
        evt.p_soter = p_soter;
        evt.conn_handle = p_ble_evt->evt.gatts_evt.conn_handle;
        evt.p_link_ctx = p_client;

        p_soter->data_handler(&evt);
    }
}

void ble_soter_service_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context) {
    if ((p_context == NULL) || (p_ble_evt == NULL)) {
        return;
    }

    ble_soter_service_t *p_soter_service = (ble_soter_service_t *)p_context;

    if (p_soter_service == NULL || p_ble_evt == NULL) {
        return;
    }

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_soter_service, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_soter_service, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_soter_service, p_ble_evt);
            break;

        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            on_hvx_tx_complete(p_soter_service, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}

uint32_t ble_soter_service_init(
    ble_soter_service_t *p_soter_service,
    const ble_soter_service_init_t *p_soter_service_init) {
    uint32_t err_code;
    ble_uuid_t ble_uuid;
    ble_add_char_params_t add_char_params;

    VERIFY_PARAM_NOT_NULL(p_soter_service);
    VERIFY_PARAM_NOT_NULL(p_soter_service_init);

    // Initialize service structure
    p_soter_service->data_handler = p_soter_service_init->data_handler;

    // Add service
    ble_uuid128_t base_uuid = {BLE_UUID_SOTER_SERVICE_BASE_UUID};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_soter_service->uuid_type);
    APP_ERROR_CHECK(err_code);

    ble_uuid.type = p_soter_service->uuid_type;
    ble_uuid.uuid = BLE_UUID_SOTER_SERVICE_UUID;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid,
                                        &p_soter_service->service_handle);
    APP_ERROR_CHECK(err_code);

    // Add the RX Characteristic.
    ble_uuid128_t rx_base_uuid = {BLE_UUID_SOTER_RX_CHAR_BASE_UUID};
    err_code = sd_ble_uuid_vs_add(&rx_base_uuid, &p_soter_service->uuid_type);
    APP_ERROR_CHECK(err_code);

    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid = BLE_UUID_SOTER_RX_CHAR_UUID;
    add_char_params.uuid_type = p_soter_service->uuid_type;
    add_char_params.max_len = BLE_SOTER_MAX_DATA_LEN;
    add_char_params.init_len = sizeof(uint8_t);
    add_char_params.is_var_len = true;
    add_char_params.char_props.write = 1;
    add_char_params.char_props.write_wo_resp = 1;

    add_char_params.read_access = SEC_JUST_WORKS;
    add_char_params.write_access = SEC_JUST_WORKS;

    err_code =
        characteristic_add(p_soter_service->service_handle, &add_char_params,
                           &p_soter_service->rx_handles);
    APP_ERROR_CHECK(err_code);

    // Add the TX Characteristic.
    ble_uuid128_t tx_base_uuid = {BLE_UUID_SOTER_TX_CHAR_BASE_UUID};
    err_code = sd_ble_uuid_vs_add(&tx_base_uuid, &p_soter_service->uuid_type);
    APP_ERROR_CHECK(err_code);

    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid = BLE_UUID_SOTER_TX_CHAR_UUID;
    add_char_params.uuid_type = p_soter_service->uuid_type;
    add_char_params.max_len = BLE_SOTER_MAX_DATA_LEN;
    add_char_params.init_len = sizeof(uint8_t);
    add_char_params.is_var_len = true;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access = SEC_JUST_WORKS;
    add_char_params.write_access = SEC_JUST_WORKS;
    add_char_params.cccd_write_access = SEC_JUST_WORKS;

    return characteristic_add(p_soter_service->service_handle, &add_char_params,
                              &p_soter_service->tx_handles);
    APP_ERROR_CHECK(err_code);

    return NRF_SUCCESS;
}

uint32_t ble_soter_service_data_send(ble_soter_service_t *p_soter_service,
                                     uint8_t *p_data, uint16_t *p_length,
                                     uint16_t conn_handle) {
    ret_code_t err_code;
    ble_gatts_hvx_params_t hvx_params;
    ble_soter_client_context_t *p_client;

    VERIFY_PARAM_NOT_NULL(p_soter_service);

    err_code = blcm_link_ctx_get(p_soter_service->p_link_ctx_storage,
                                 conn_handle, (void *)&p_client);
    VERIFY_SUCCESS(err_code);

    if ((conn_handle == BLE_CONN_HANDLE_INVALID) || (p_client == NULL)) {
        return NRF_ERROR_NOT_FOUND;
    }

    if (!p_client->is_notification_enabled) {
        return NRF_ERROR_INVALID_STATE;
    }

    if (*p_length > BLE_SOTER_MAX_DATA_LEN) {
        return NRF_ERROR_INVALID_PARAM;
    }

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = p_soter_service->tx_handles.value_handle;
    hvx_params.p_data = p_data;
    hvx_params.p_len = p_length;
    hvx_params.type = BLE_GATT_HVX_NOTIFICATION;

    return sd_ble_gatts_hvx(conn_handle, &hvx_params);
}
