#ifndef SOTER_BLE_H__
#define SOTER_BLE_H__

#include <stdint.h>
#include <string.h>
#include "app_timer.h"
#include "app_util_platform.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_bas.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "ble_dis.h"
#include "ble_hci.h"
#include "fds.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "soter_service.h"
#include "transport_message.h"

#include "FreeRTOS.h"
#include "nrf_sdh_freertos.h"
#include "semphr.h"
#include "task.h"

#define BLE_SEGMENT_SIZE 64

typedef void (*soter_ble_pairing_pin_handler_t)(char *pin);

void soter_ble_set_pairing_pin_handler(soter_ble_pairing_pin_handler_t handler);
bool soter_ble_tx(uint8_t *message, uint16_t len);

void soter_ble_set_rx_parameters(transport_message_t *msg, SemaphoreHandle_t *semaphore);

void soter_ble_init(char *device_name);
void soter_ble_remove_peer(uint16_t peer_id);
bool soter_ble_connected(void);
void soter_ble_update_battery_level(uint8_t battery_level);
#endif