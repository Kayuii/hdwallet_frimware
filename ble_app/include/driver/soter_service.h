#ifndef SOTER_SERVICE_H
#define SOTER_SERVICE_H

#include "app_error.h"
#include "ble.h"
#include "ble_link_ctx_manager.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include <string.h>

#ifndef BLE_SOTER_BLE_OBSERVER_PRIO
#define BLE_SOTER_BLE_OBSERVER_PRIO 2
#endif

#define BLE_SOTER_DEF(_name)                                          \
  BLE_LINK_CTX_MANAGER_DEF(CONCAT_2(_name, _link_ctx_storage),        \
      (1),                                                            \
      sizeof(ble_soter_client_context_t));                            \
  static ble_soter_service_t _name =                                  \
      {                                                               \
          .p_link_ctx_storage = &CONCAT_2(_name, _link_ctx_storage)}; \
  NRF_SDH_BLE_OBSERVER(_name##_obs,                                   \
      BLE_SOTER_BLE_OBSERVER_PRIO,                                    \
      ble_soter_service_on_ble_evt,                                   \
      &_name)

#define OPCODE_LENGTH 1
#define HANDLE_LENGTH 2

/**@brief   Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
#if defined(NRF_SDH_BLE_GATT_MAX_MTU_SIZE) && (NRF_SDH_BLE_GATT_MAX_MTU_SIZE != 0)
#define BLE_SOTER_MAX_DATA_LEN (NRF_SDH_BLE_GATT_MAX_MTU_SIZE - OPCODE_LENGTH - HANDLE_LENGTH)
#else
#define BLE_SOTER_MAX_DATA_LEN (BLE_GATT_MTU_SIZE_DEFAULT - OPCODE_LENGTH - HANDLE_LENGTH)
#warning NRF_SDH_BLE_GATT_MAX_MTU_SIZE is not defined.
#endif

typedef enum {
  BLE_SOTER_EVT_RX_DATA,      /**< Data received. */
  BLE_SOTER_EVT_TX_RDY,       /**< Service is ready to accept new data to be transmitted. */
  BLE_SOTER_EVT_COMM_STARTED, /**< Notification has been enabled. */
  BLE_SOTER_EVT_COMM_STOPPED, /**< Notification has been disabled. */
} ble_soter_evt_type_t;

typedef struct ble_soter_service_s ble_soter_service_t;

typedef struct
{
  uint8_t const *p_data; /**< A pointer to the buffer with received data. */
  uint16_t length;       /**< Length of received data. */
} ble_soter_service_evt_rx_data_t;

typedef struct
{
  bool is_notification_enabled; /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
} ble_soter_client_context_t;

typedef struct
{
  ble_soter_evt_type_t type;              /**< Event type. */
  ble_soter_service_t *p_soter;           /**< A pointer to the instance. */
  uint16_t conn_handle;                   /**< Connection handle. */
  ble_soter_client_context_t *p_link_ctx; /**< A pointer to the link context. */
  union {
    ble_soter_service_evt_rx_data_t rx_data; /**< @ref BLE_SOTER_EVT_RX_DATA event data. */
  } params;
} ble_soter_service_evt_t;

typedef void (*ble_soter_service_data_handler_t)(ble_soter_service_evt_t *p_evt);

typedef struct
{
  ble_soter_service_data_handler_t data_handler;
} ble_soter_service_init_t;

struct ble_soter_service_s {
  uint8_t uuid_type;                                 /**< UUID type for Nordic UART Service Base UUID. */
  uint16_t service_handle;                           /**< Handle of Nordic UART Service (as provided by the SoftDevice). */
  ble_gatts_char_handles_t tx_handles;               /**< Handles related to the TX characteristic (as provided by the SoftDevice). */
  ble_gatts_char_handles_t rx_handles;               /**< Handles related to the RX characteristic (as provided by the SoftDevice). */
  blcm_link_ctx_storage_t *const p_link_ctx_storage; /**< Pointer to link context storage with handles of all current connections and its context. */
  ble_soter_service_data_handler_t data_handler;     /**< Event handler to be called for handling received data. */
};

uint32_t ble_soter_service_init(ble_soter_service_t *p_soter_service, const ble_soter_service_init_t *p_soter_service_init);

void ble_soter_service_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context);

uint32_t ble_soter_service_data_send(ble_soter_service_t *p_soter,
    uint8_t *p_data,
    uint16_t *p_length,
    uint16_t conn_handle);

#endif //SOTER_SERVICE_H