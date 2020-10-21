#include "driver/soter_ble.h"
#include "nrf_ble_lesc.h"

#define NRF_LOG_MODULE_NAME soter_ble
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define MANUFACTURER_NAME "Digbig Co., Ltd." /**< Manufacturer.  */
#define MODEL_NUMBER "V1"                    /**< Model Number string. */
#define MANUFACTURER_ID 0x1600161800         /**< Manufacturer ID. */
#define ORG_UNIQUE_ID 0x161816               /**< Organisation Unique ID. */

#define APP_BLE_OBSERVER_PRIO                                                \
    3 /**< Application's BLE observer priority. You shouldn't need to modify \
         this value. */
#define APP_BLE_CONN_CFG_TAG \
    1 /**< A tag identifying the SoftDevice BLE configuration. */

#define APP_ADV_INTERVAL                                               \
    40 /**< The advertising interval (in units of 0.625 ms. This value \
          corresponds to 25 ms). */
#define APP_ADV_DURATION                                             \
    30000 /**< The advertising duration (300 seconds) in units of 10 \
             milliseconds. */
#define MIN_CONN_INTERVAL \
    MSEC_TO_UNITS(        \
        10,               \
        UNIT_1_25_MS) /**< Minimum acceptable connection interval (10 ms). */
#define MAX_CONN_INTERVAL \
    MSEC_TO_UNITS(        \
        100,              \
        UNIT_1_25_MS)   /**< Maximum acceptable connection interval (100 ms) */
#define SLAVE_LATENCY 0 /**< Slave latency. */
#define CONN_SUP_TIMEOUT \
    MSEC_TO_UNITS(       \
        4000, UNIT_10_MS) /**< Connection supervisory timeout (4 seconds). */
#define FIRST_CONN_PARAMS_UPDATE_DELAY                                       \
    APP_TIMER_TICKS(                                                         \
        5000) /**< Time from initiating event (connect or start of           \
                 notification) to first time sd_ble_gap_conn_param_update is \
                 called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY                                      \
    APP_TIMER_TICKS(                                                       \
        30000) /**< Time between each call to sd_ble_gap_conn_param_update \
                  after the first call (30 seconds). */
#define MAX_CONN_PARAM_UPDATE_COUNT                                     \
    3 /**< Number of attempts before giving up the connection parameter \
         negotiation. */

#define LESC_DEBUG_MODE                                                    \
    0 /**< Set to 1 to use LESC debug keys, allows you to use a sniffer to \
         inspect traffic. */

#define SEC_PARAM_BOND 1 /**< Perform bonding. */
#define SEC_PARAM_MITM                                                    \
    1 /**< Man In The Middle protection required (applicable when display \
         module is detected). */
#define SEC_PARAM_LESC 1     /**< LE Secure Connections enabled. */
#define SEC_PARAM_KEYPRESS 0 /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES \
    BLE_GAP_IO_CAPS_DISPLAY_ONLY  /**< Display I/O capabilities. */
#define SEC_PARAM_OOB 0           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE 7  /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE 16 /**< Maximum encryption key size. */

#define PASSKEY_LENGTH \
    6 /**< Length of pass-key received by the stack for display. */

#define SOTER_BLE_STACK_SIZE 1024
#define SOTER_BLE_PRIORITY 2

static char *m_device_name;

TaskHandle_t soter_ble_task_handle;

BLE_BAS_DEF(m_bas);                 /**< Battery service instance. */
NRF_BLE_GATT_DEF(m_gatt);           /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);             /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising); /**< Advertising module instance. */
BLE_SOTER_DEF(m_soter);             /**< BLE Soter service instance. */

static transport_message_t *rx_message = NULL;
static SemaphoreHandle_t *rx_semaphore = NULL;

static pm_peer_id_t m_peer_to_be_deleted = PM_PEER_ID_INVALID;
static uint16_t m_conn_handle =
    BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */

static bool erase_bonds;

static ble_uuid_t
    m_adv_uuids[] = /**< Universally unique service identifiers. */
    {{BLE_UUID_BATTERY_SERVICE, BLE_UUID_TYPE_BLE},
     {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}};

uint16_t soter_service_max_data_len = BLE_SEGMENT_SIZE;

static volatile m_pending_tx = false;

static void advertising_start(void *p_erase_bonds);
static void soter_service_data_handler(ble_soter_service_evt_t *p_evt);

soter_ble_pairing_pin_handler_t soter_ble_pairing_pin_handler = NULL;

void soter_ble_set_rx_parameters(transport_message_t *msg,
                                 SemaphoreHandle_t *semaphore) {
    rx_message = msg;
    rx_semaphore = semaphore;
}

void soter_ble_set_pairing_pin_handler(
    soter_ble_pairing_pin_handler_t handler) {
    soter_ble_pairing_pin_handler = handler;
}

bool soter_ble_tx(uint8_t *message, uint16_t len) {
    uint16_t pos = 1;
    uint8_t *tmp_buffer = (uint8_t *)malloc(soter_service_max_data_len);
    /* Chunk out message */
    while (pos < len) {
        memset(tmp_buffer, 0x00, soter_service_max_data_len);
        tmp_buffer[0] = '?';
        memcpy(tmp_buffer + 1, message + pos, soter_service_max_data_len - 1);
        m_pending_tx = true;
        ble_soter_service_data_send(&m_soter, tmp_buffer,
                                    &soter_service_max_data_len, m_conn_handle);
        pos += soter_service_max_data_len - 1;
        while (m_pending_tx) {
            vTaskDelay(1);
        }
    }
    free(tmp_buffer);
    return true;
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const *p_evt) {
    ret_code_t err_code;

    pm_handler_on_pm_evt(p_evt);
    pm_handler_disconnect_on_sec_failure(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id) {
        case PM_EVT_BONDED_PEER_CONNECTED:
            NRF_LOG_INFO("PM_EVT_BONDED_PEER_CONNECTED peer_id %d",
                         p_evt->peer_id);
            break;
        case PM_EVT_CONN_SEC_START:
            NRF_LOG_INFO("PM_EVT_CONN_SEC_START");
            break;
        case PM_EVT_CONN_SEC_SUCCEEDED: {
            pm_conn_sec_status_t conn_sec_status;
            // Check if the link is authenticated (meaning at least MITM).
            err_code =
                pm_conn_sec_status_get(p_evt->conn_handle, &conn_sec_status);
            APP_ERROR_CHECK(err_code);
            if (conn_sec_status.mitm_protected) {
                NRF_LOG_INFO(
                    "Link secured. Role: %d. conn_handle: %d, Procedure: %d",
                    ble_conn_state_role(p_evt->conn_handle), p_evt->conn_handle,
                    p_evt->params.conn_sec_succeeded.procedure);
            } else {
                // The peer did not use MITM, disconnect.
                NRF_LOG_INFO("Collector did not use MITM, disconnecting");
                err_code = pm_peer_id_get(m_conn_handle, &m_peer_to_be_deleted);
                APP_ERROR_CHECK(err_code);
                err_code = sd_ble_gap_disconnect(
                    m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
                APP_ERROR_CHECK(err_code);
            }
        } break;
        case PM_EVT_CONN_SEC_FAILED:
            NRF_LOG_INFO("PM_EVT_CONN_SEC_FAILED");
            if (p_evt->params.conn_sec_failed.error ==
                PM_CONN_SEC_ERROR_PIN_OR_KEY_MISSING) {
                // Rebond if one party has lost its keys.
                err_code = pm_conn_secure(p_evt->conn_handle, true);

                if (err_code != NRF_ERROR_BUSY) {
                    APP_ERROR_CHECK(err_code);
                }
            }
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;
        case PM_EVT_CONN_SEC_PARAMS_REQ:
            NRF_LOG_INFO("PM_EVT_CONN_SEC_PARAMS_REQ");
            break;
        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            NRF_LOG_INFO("PM_EVT_PEERS_DELETE_SUCCEEDED");
            advertising_start(false);
            break;
        case PM_EVT_CONN_SEC_CONFIG_REQ: {
            NRF_LOG_INFO("PM_EVT_CONN_SEC_CONFIG_REQ");
            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = true};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
        } break;
        default:
            NRF_LOG_INFO("Unhandled PM Evt %d", p_evt->evt_id);
            break;
    }
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile)
 * parameters of the device including the device name, appearance, and the
 * preferred connection parameters.
 */
static void gap_params_init(void) {
    ret_code_t err_code;
    ble_gap_conn_params_t gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(
        &sec_mode, (const uint8_t *)m_device_name, strlen(m_device_name));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void) {
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may
 * need to inform the application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went
 * wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error) {
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing services that will be used by the
 * application.
 */
static void services_init() {
    ret_code_t err_code;
    ble_bas_init_t bas_init;
    ble_dis_init_t dis_init;
    ble_soter_service_init_t soter_service_init;
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Battery Service.
    memset(&bas_init, 0, sizeof(bas_init));

    // Here the sec level for the Battery Service can be changed/increased.
    bas_init.bl_rd_sec = SEC_OPEN;
    bas_init.bl_cccd_wr_sec = SEC_OPEN;
    bas_init.bl_report_rd_sec = SEC_OPEN;
    bas_init.evt_handler = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref = NULL;
    bas_init.initial_batt_level = 100;
    err_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Device Information Service.
    memset(&dis_init, 0, sizeof(dis_init));
    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str,
                          (char *)MANUFACTURER_NAME);
    ble_srv_ascii_to_utf8(&dis_init.serial_num_str, MODEL_NUMBER);
    ble_dis_sys_id_t system_id;
    system_id.manufacturer_id = MANUFACTURER_ID;
    system_id.organizationally_unique_id = ORG_UNIQUE_ID;
    dis_init.p_sys_id = &system_id;

    dis_init.dis_char_rd_sec = SEC_OPEN;

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);

    memset(&soter_service_init, 0, sizeof(soter_service_init));
    soter_service_init.data_handler = soter_service_data_handler;
    err_code = ble_soter_service_init(&m_soter, &soter_service_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection
 * Parameters Module which are passed to the application.
 *          @note All this function does is to disconnect. This could have been
 * done by simply setting the disconnect_on_fail config parameter, but instead
 * we use the event handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t *p_evt) {
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED) {
        err_code = sd_ble_gap_disconnect(m_conn_handle,
                                         BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went
 * wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error) {
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void) {
    ret_code_t err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAM_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = on_conn_params_evt;
    cp_init.error_handler = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed
 * to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt) {
    switch (ble_adv_evt) {
        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("Fast advertising.");
            break;
        case BLE_ADV_EVT_IDLE:
            NRF_LOG_INFO("Advertising timeout, restarting.")
            advertising_start(NULL);
            break;
        default:
            break;
    }
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context) {
    ret_code_t err_code = NRF_SUCCESS;

    pm_handler_secure_on_connection(p_ble_evt);

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected");
            m_peer_to_be_deleted = PM_PEER_ID_INVALID;
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected");
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            // Check if the last connected peer had not used MITM, if so, delete
            // its bond information.
            if (m_peer_to_be_deleted != PM_PEER_ID_INVALID) {
                err_code = pm_peer_delete(m_peer_to_be_deleted);
                APP_ERROR_CHECK(err_code);
                NRF_LOG_DEBUG("Collector's bond deleted");
                m_peer_to_be_deleted = PM_PEER_ID_INVALID;
            }
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys = {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle,
                                             &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(
                p_ble_evt->evt.gattc_evt.conn_handle,
                BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_EVT_USER_MEM_REQUEST:
            NRF_LOG_DEBUG("BLE_EVT_USER_MEM_REQUEST");
            err_code = sd_ble_user_mem_reply(
                p_ble_evt->evt.gattc_evt.conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(
                p_ble_evt->evt.gatts_evt.conn_handle,
                BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            NRF_LOG_DEBUG("BLE_GAP_EVT_SEC_PARAMS_REQUEST");
            break;

        case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST: {
            ble_gap_data_length_params_t dl_params;

            // Clearing the struct will effectively set members to @ref
            // BLE_GAP_DATA_LENGTH_AUTO.
            memset(&dl_params, 0, sizeof(ble_gap_data_length_params_t));
            err_code = sd_ble_gap_data_length_update(
                p_ble_evt->evt.gap_evt.conn_handle, &dl_params, NULL);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            NRF_LOG_DEBUG("BLE_GATTS_EVT_SYS_ATTR_MISSING");
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_PASSKEY_DISPLAY: {
            NRF_LOG_DEBUG("BLE_GAP_EVT_PASSKEY_DISPLAY");
            char passkey[PASSKEY_LENGTH + 1];
            memcpy(passkey,
                   p_ble_evt->evt.gap_evt.params.passkey_display.passkey,
                   PASSKEY_LENGTH);
            passkey[PASSKEY_LENGTH] = 0;
            if (soter_ble_pairing_pin_handler) {
                soter_ble_pairing_pin_handler(passkey);
            }
            NRF_LOG_INFO("Passkey: %s", nrf_log_push(passkey));
        } break;

        case BLE_GAP_EVT_AUTH_KEY_REQUEST:
            NRF_LOG_INFO("BLE_GAP_EVT_AUTH_KEY_REQUEST");
            break;

        case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
            NRF_LOG_INFO("BLE_GAP_EVT_LESC_DHKEY_REQUEST");
            break;

        case BLE_GAP_EVT_AUTH_STATUS:
            NRF_LOG_INFO(
                "BLE_GAP_EVT_AUTH_STATUS: status=0x%x bond=0x%x lv4: %d "
                "kdist_own:0x%x kdist_peer:0x%x",
                p_ble_evt->evt.gap_evt.params.auth_status.auth_status,
                p_ble_evt->evt.gap_evt.params.auth_status.bonded,
                p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv4,
                *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status
                      .kdist_own),
                *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status
                      .kdist_peer));
            break;

        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void) {
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler,
                         NULL);
}

/**@brief Function for the Peer Manager initialization.
 */
static void peer_manager_init(void) {
    ble_gap_sec_params_t sec_param;
    ret_code_t err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));
    // Security parameters to be used for all security procedures.
    sec_param.bond = SEC_PARAM_BOND;
    sec_param.mitm = SEC_PARAM_MITM;
    sec_param.lesc = SEC_PARAM_LESC;
    sec_param.keypress = SEC_PARAM_KEYPRESS;
    sec_param.io_caps = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob = SEC_PARAM_OOB;
    sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc = 1;
    sec_param.kdist_own.id = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id = 1;
    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);
    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}

/**@brief Clear bond information from persistent storage.
 */
static void delete_bonds(void) {
    ret_code_t err_code;

    NRF_LOG_INFO("Erase bonds!");

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void) {
    ret_code_t err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = true;
    init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt =
        sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids = m_adv_uuids;

    init.config.ble_adv_fast_enabled = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout = APP_ADV_DURATION;

    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**@brief Function for starting advertising.
 */
static void advertising_start(void *p_erase_bonds) {
    bool erase_bonds = *(bool *)p_erase_bonds;

    if (erase_bonds == true) {
        delete_bonds();
        // Advertising is started by PM_EVT_PEERS_DELETED_SUCEEDED event
    } else {
        ret_code_t err_code =
            ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(err_code);
    }
}

static void soter_service_data_handler(ble_soter_service_evt_t *p_evt) {
    switch (p_evt->type) {
        case BLE_SOTER_EVT_RX_DATA: {
            if (rx_message) {
                rx_message->link_type = LINK_TYPE_BLE;
                rx_message->len = p_evt->params.rx_data.length;
                memcpy(rx_message->message, p_evt->params.rx_data.p_data,
                       rx_message->len);
                xSemaphoreGive(*rx_semaphore);
            }

        } break;
        case BLE_SOTER_EVT_TX_RDY:
            m_pending_tx = false;
            NRF_LOG_DEBUG("BLE_SOTER_EVT_TX_RDY");
            break;
        case BLE_SOTER_EVT_COMM_STARTED:
            NRF_LOG_DEBUG("BLE_SOTER_EVT_COMM_STARTED");
            break;
        case BLE_SOTER_EVT_COMM_STOPPED:
            NRF_LOG_DEBUG("BLE_SOTER_EVT_COMM_STOPPED");
            break;
    }
}

/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler(nrf_ble_gatt_t *p_gatt, nrf_ble_gatt_evt_t const *p_evt) {
    if ((m_conn_handle == p_evt->conn_handle) &&
        (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)) {
        uint16_t max_data_len =
            p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        if (max_data_len <= BLE_SEGMENT_SIZE) {
            soter_service_max_data_len = max_data_len;
        }
        NRF_LOG_INFO("Data len is set to 0x%X(%d)", soter_service_max_data_len,
                     soter_service_max_data_len);
    }
    NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
                  p_gatt->att_mtu_desired_central,
                  p_gatt->att_mtu_desired_periph);
}

static void soter_ble_task(void *arg) {
    ret_code_t err_code;
    NRF_LOG_DEBUG("Starting lesc task...");
    while (1) {
        err_code = nrf_ble_lesc_request_handler();
        APP_ERROR_CHECK(err_code);
        vTaskDelay(100);
    }
}

void soter_ble_update_battery_level(uint8_t battery_level) {
    ret_code_t err_code;
    err_code = ble_bas_battery_level_update(&m_bas, battery_level,
                                            BLE_CONN_HANDLE_ALL);
    if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_INVALID_STATE) &&
        (err_code != NRF_ERROR_RESOURCES) && (err_code != NRF_ERROR_BUSY) &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)) {
        APP_ERROR_HANDLER(err_code);
    }
}

void soter_ble_remove_peer(uint16_t peer_id) {
    ret_code_t err_code;
    err_code = pm_peer_delete(peer_id);
    APP_ERROR_CHECK(err_code);
}

void soter_ble_init(char *device_name) {
    // Configure and initialize the BLE stack.
    m_device_name = device_name;
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();
    peer_manager_init();
    if (pdPASS != xTaskCreate(soter_ble_task, "SWBLE", SOTER_BLE_STACK_SIZE,
                              NULL, SOTER_BLE_PRIORITY,
                              &soter_ble_task_handle)) {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
    ret_code_t err_code = sd_ble_gap_tx_power_set(
        BLE_GAP_TX_POWER_ROLE_ADV, m_advertising.adv_handle, -20);
    APP_ERROR_CHECK(err_code);
    nrf_sdh_freertos_init(advertising_start, &erase_bonds);
}

bool soter_ble_connected(void) {
    return (m_conn_handle != BLE_CONN_HANDLE_INVALID) ? 1 : 0;
}
