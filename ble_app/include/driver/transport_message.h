#ifndef TRANSPORT_MESSAGE_H
#define TRANSPORT_MESSAGE_H

#include "inttypes.h"

#define MAX_TRANSPORT_MESSAGE_SIZE 64

typedef enum { LINK_TYPE_BLE, LINK_TYPE_USB } link_type_t;


typedef struct {
    uint32_t len;
    uint8_t message[MAX_TRANSPORT_MESSAGE_SIZE];
    link_type_t link_type;
} transport_message_t;

typedef void (*transport_rx_callback_t)(transport_message_t* msg);

#endif  // TRANSPORT_MESSAGE_H