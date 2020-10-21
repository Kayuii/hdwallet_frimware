#ifndef SB_HID_DRIVER_H
#define USB_HID_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "transport_message.h"
#include "nrf_drv_usbd.h"
#include "app_usbd_hid_generic.h"
#include "FreeRTOS.h"
#include "nrf_sdh_freertos.h"
#include "semphr.h"
#include "task.h"

#define USE_U2F	true
 
 /**
* @brief HID generic class endpoint number.
* */
#define ENDPOINT_ADDRESS_IN         NRF_DRV_USBD_EPIN1
#define ENDPOINT_ADDRESS_OUT        NRF_DRV_USBD_EPOUT1
#define ENDPOINT_ADDRESS_U2F_IN     NRF_DRV_USBD_EPIN3
#define ENDPOINT_ADDRESS_U2F_OUT    NRF_DRV_USBD_EPOUT3

#define USB_SEGMENT_SIZE 64
#define MAX_NUM_USB_SEGMENTS 1

void usb_set_u2f_rx_callback(transport_rx_callback_t callback);

void usb_set_rx_parameters(transport_message_t *msg, SemaphoreHandle_t *semaphore);

void usb_set_u2f_transport(void);
void usb_set_hid_transport(void);
bool usb_is_u2f_transport(void);


/**
 * @brief Send U2F HID Data.
 *
 *
 * @param[in] cid       HID Channel identifier.
 * @param[in] cmd       Frame command.
 * @param[in] p_data    Frame Data packet.
 * @param[in] size      Data length
 *
 * @return Standard error code.
 */
uint8_t u2f_hid_if_send(uint32_t cid, uint8_t cmd, 
                        uint8_t * p_data, size_t size);



/**
 * @brief Receive U2F HID Data.
 *
 *
 * @param[out] p_cid       HID Channel identifier.
 * @param[out] p_cmd       Frame command.
 * @param[out] p_data      Frame Data packet.
 * @param[out] p_size      Data length
 * @param[in]  timeout     message timeout in ms 
 *
 * @return Standard error code.
 */
uint8_t u2f_hid_if_recv(uint32_t * p_cid, uint8_t * p_cmd, 
                    uint8_t * p_data, size_t * p_size,
                    uint32_t timeout);

/**
* @brief Initialize USB HID interface.
*
* @return Standard error code.
*/
void usb_init(void);
void usb_poll(void);

/**
 * @brief U2F HID interface process.
 *
 *
 */
char usbTiny(char set);

bool usb_tx(uint8_t *message, uint16_t len);
bool usb_u2f_tx_helper(uint8_t *message, uint16_t len, uint8_t endpoint);
bool get_usb_init_stat();
/** @} */

#ifdef __cplusplus
}
#endif

#endif // USB_HID_DRIVER_H
