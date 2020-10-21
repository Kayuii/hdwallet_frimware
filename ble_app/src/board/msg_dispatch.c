/*
 * This file is part of the KeepKey project.
 *
 * Copyright (C) 2015 KeepKey LLC
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

/* === Includes ============================================================ */

#include "board/msg_dispatch.h"
#include "board/layout.h"
#include "board/u2f.h"
#include "driver/soter_ble.h"
#include "driver/usb.h"

#include <nanopb.h>

#include <assert.h>
#include <string.h>

/* === Private Variables =================================================== */

static const MessagesMap_t *m_MessagesMap = NULL;
static size_t m_map_size = 0;
static msg_failure_t m_msg_failure;

#ifndef CONFIDENTIAL
#define CONFIDENTIAL
#endif

#define MSG_TASK_STACK_SIZE 1024
#define MSG_TASK_PRIORITY 2

link_type_t m_link_type;

static transport_message_t rx_message;
static TaskHandle_t msg_task_handle;
static SemaphoreHandle_t msg_semaphore;

/* Tiny messages */
static bool m_msg_tiny_flag = false;
static CONFIDENTIAL uint8_t m_msg_tiny[MSG_TINY_BFR_SZ];
static uint16_t m_msg_tiny_id = MSG_TINY_TYPE_ERROR; /* Default to error type */

/* === Variables =========================================================== */

/* Allow mapped messages to reset message stack.  This variable by itself
 * doesn't do much but messages down the line can use it to determine for to
 * gracefully exit from a message should the message stack been reset
 */
bool m_reset_msg_stack = false;

/* === Private Functions =================================================== */

/*
 * message_map_entry() - Finds a requested message map entry
 *
 * INPUT
 *     - msg_id: message id
 *     - dir: direction of message
 * OUTPUT
 *     entry if found
 */
static const MessagesMap_t *message_map_entry(MessageType msg_id,
                                              MessageMapDirection dir) {
    const MessagesMap_t *m = m_MessagesMap;

    if (m_map_size > msg_id && m[msg_id].msg_id == msg_id &&
        m[msg_id].dir == dir) {
        switch (m[msg_id].msg_perms) {
            case MFROnly:
                return NULL;
            case MFRProhibited:
                return &m[msg_id];
            case AnyVariant:
                return &m[msg_id];
        }
#if defined(DEBUG_ON)
        __builtin_unreachable();
#else
        return NULL;
#endif
    }

    return NULL;
}

/*
 * message_fields() - Get protocol buffer for requested message map entry
 *
 * INPUT
 *     - type: type of message (normal or debug)
 *     - msg_id: message id
 *     - dir: direction of message
 * OUTPUT
 *      protocol buffer
 */
static const pb_field_t *message_fields(MessageMapType type, MessageType msg_id,
                                        MessageMapDirection dir) {
    assert(m_MessagesMap != NULL);

    const MessagesMap_t *m = m_MessagesMap;

    if (m_map_size > msg_id && m[msg_id].msg_id == msg_id &&
        m[msg_id].type == type && m[msg_id].dir == dir) {
        return m[msg_id].fields;
    }

    return NULL;
}

/*
 * write_pb() - Add frame header info to message buffer and perform
 * transmission
 *
 * INPUT
 *     - fields: protocol buffer
 *     - msg: pointer to message buffer
 *     - id: message id
 *     - tx_handler: handler to use to write data
 * OUTPUT
 *     none
 */
static TrezorFrameBuffer framebuf;
static void write_pb(const pb_field_t *fields, const void *msg, MessageType id,
                     msg_tx_handler_t tx_handler) {
    assert(fields != NULL);

    memset(&framebuf, 0, sizeof(framebuf));
    framebuf.frame.usb_header.hid_type = '?';
    framebuf.frame.header.pre1 = '#';
    framebuf.frame.header.pre2 = '#';
    framebuf.frame.header.id = __builtin_bswap16(id);

    pb_ostream_t os =
        pb_ostream_from_buffer(framebuf.buffer, sizeof(framebuf.buffer));

    if (pb_encode(&os, fields, msg)) {
        framebuf.frame.header.len = __builtin_bswap32(os.bytes_written);
        (*tx_handler)((uint8_t *)&framebuf,
                      sizeof(framebuf.frame) + os.bytes_written);
    }
}

/*
 * pb_parse() - Process USB message by protocol buffer
 *
 * INPUT
 *     - entry: pointer to message entry
 *     - msg: pointer to received message buffer
 *     - msg_size: size of message
 *     - buf: pointer to destination buffer
 * OUTPUT
 *     true/false whether protocol buffers were parsed successfully
 */
static bool pb_parse(const MessagesMap_t *entry, uint8_t *msg,
                     uint32_t msg_size, uint8_t *buf) {
    pb_istream_t stream = pb_istream_from_buffer(msg, msg_size);
    return (pb_decode(&stream, entry->fields, buf));
}

/*
 * dispatch() - Process received message and jump to corresponding process
 * function
 *
 * INPUT
 *     - entry: pointer to message entry
 *     - msg: pointer to received message buffer
 *     - msg_size: size of message
 * OUTPUT
 *     none
 *
 */
static void dispatch(const MessagesMap_t *entry, uint8_t *msg,
                     uint32_t msg_size) {
    static CONFIDENTIAL uint8_t decode_buffer[MAX_DECODE_SIZE]
        __attribute__((aligned(4)));
    memset(decode_buffer, 0, sizeof(decode_buffer));

    if (pb_parse(entry, msg, msg_size, decode_buffer)) {
        if (entry->process_func) {
            entry->process_func(decode_buffer);
        } else {
            (*m_msg_failure)(FailureType_Failure_UnexpectedMessage,
                             "Unexpected message");
        }
    } else {
        (*m_msg_failure)(FailureType_Failure_UnexpectedMessage,
                         "Could not parse protocol buffer message");
    }
}

/*
 * tiny_dispatch() - Process received tiny messages
 *
 * INPUT
 *     - entry: pointer to message entry
 *     - msg: pointer to received message buffer
 *     - msg_size: size of message
 * OUTPUT
 *     none
 *
 */
static void tiny_dispatch(const MessagesMap_t *entry, uint8_t *msg,
                          uint32_t msg_size) {
    bool status = pb_parse(entry, msg, msg_size, m_msg_tiny);

    if (status) {
        m_msg_tiny_id = entry->msg_id;
    } else {
        call_msg_failure_handler(
            FailureType_Failure_UnexpectedMessage,
            "Could not parse tiny protocol buffer message");
    }
}

/*
 * raw_dispatch() - Process messages that will not be parsed by protocol buffers
 * and should be manually parsed at message function
 *
 * INPUT
 *     - entry: pointer to message entry
 *     - msg: pointer to received message buffer
 *     - msg_size: size of message
 *     - frame_length: total expected size
 * OUTPUT
 *     none
 */
static void raw_dispatch(const MessagesMap_t *entry, uint8_t *msg,
                         uint32_t msg_size, uint32_t frame_length) {
    static RawMessage raw_msg;
    raw_msg.buffer = msg;
    raw_msg.length = msg_size;

    if (entry->process_func) {
        ((raw_msg_handler_t)entry->process_func)(&raw_msg, frame_length);
    }
}

#if !defined(__has_builtin)
#define __has_builtin(X) 0
#endif
#if __has_builtin(__builtin_add_overflow)
#define check_uadd_overflow(A, B, R)                                   \
    ({                                                                 \
        typeof(A) __a = (A);                                           \
        typeof(B) __b = (B);                                           \
        typeof(R) __r = (R);                                           \
        (void)(&__a == &__b && "types must match");                    \
        (void)(&__a == __r && "types must match");                     \
        _Static_assert(0 < (typeof(A)) - 1, "types must be unsigned"); \
        __builtin_add_overflow((A), (B), (R));                         \
    })
#else
#define check_uadd_overflow(A, B, R)                                   \
    ({                                                                 \
        typeof(A) __a = (A);                                           \
        typeof(B) __b = (B);                                           \
        typeof(R) __r = (R);                                           \
        (void)(&__a == &__b);                                          \
        (void)(&__a == __r);                                           \
        (void)(&__a == &__b && "types must match");                    \
        (void)(&__a == __r && "types must match");                     \
        _Static_assert(0 < (typeof(A)) - 1, "types must be unsigned"); \
        *__r = __a + __b;                                              \
        *__r < __a;                                                    \
    })
#endif

void rx_helper(void) {
    static TrezorFrameHeaderFirst last_frame_header = {.id = 0xffff, .len = 0};
    static uint8_t content_buf[MAX_FRAME_SIZE];
    static size_t content_pos = 0, content_size = 0;
    static bool mid_frame = false;

    const MessagesMap_t *entry;
    TrezorFrame *frame = (TrezorFrame *)(rx_message.message);
    TrezorFrameFragment *frame_fragment =
        (TrezorFrameFragment *)(rx_message.message);

    bool last_segment;
    uint8_t *contents;

    m_link_type = rx_message.link_type;

    if (rx_message.len < sizeof(TrezorFrameHeaderFirst) ||
        frame->usb_header.hid_type != '?') {
        goto done_handling;
    }

    /* Check to see if this is the first frame of a series, * or a
       continuation/fragment.  */
    if (frame->header.pre1 == '#' && frame->header.pre2 == '#' && !mid_frame) {
        /* Byte swap in place. */
        last_frame_header.id = __builtin_bswap16(frame->header.id);
        last_frame_header.len = __builtin_bswap32(frame->header.len);

        contents = frame->contents;

        /* Init content pos and size */
        content_pos = rx_message.len - 9;
        content_size = content_pos;

    } else if (mid_frame) {
        contents = frame_fragment->contents;
        if (check_uadd_overflow(content_pos, (size_t)(rx_message.len - 1),
                                &content_pos))
            goto reset;
        content_size = rx_message.len - 1;
    } else {
        contents = frame_fragment->contents;
        content_size = rx_message.len - 1;
    }

    last_segment = content_pos >= last_frame_header.len;
    mid_frame = !last_segment;

    /* Determine callback handler and message map type */
    entry = message_map_entry(last_frame_header.id, IN_MSG);

    if (entry && entry->dispatch == RAW) {
        /* Call dispatch for every segment since we are not buffering and
         * parsing, and assume the raw dispatched callbacks will handle their
         * own state and buffering internally
         */
        raw_dispatch(entry, contents, content_size, last_frame_header.len);
    } else if (entry) {
        size_t offset, len;
        if (content_size == content_pos) {
            offset = 0;
            len = content_size;
        } else {
            offset = content_pos - (rx_message.len - 1);
            len = rx_message.len - 1;
        }

        size_t end;
        if (check_uadd_overflow(offset, len, &end) || sizeof(content_buf) < end)
            goto reset;

        /* Copy content to frame buffer */
        memcpy(content_buf + offset, contents, len);
    }

    /*
     * Only parse and message map if all segments have been buffered
     * and this message type is parsable
     */
    if (last_segment && !entry) {
        (*m_msg_failure)(FailureType_Failure_UnexpectedMessage,
                         "Unknown message");
    } else if (last_segment && entry->dispatch != RAW) {
        if (m_msg_tiny_flag) {
            tiny_dispatch(entry, content_buf, last_frame_header.len);
            goto done_handling;
        } else {
            dispatch(entry, content_buf, last_frame_header.len);
            goto done_handling;
        }
    }
    // the packetized u2f-injection device<->host protocol requires and ACK sent
    // to be sent after every packet
    if (usb_is_u2f_transport()) {
        // only send the ACK packet if the recvd message came in over the U2F
        // transport
        // TODO pass in debug link and set flag here to handle framed debuglink
        // acks
        uint8_t empty_report[] = {0x00, 0x00, 0x00, contents[1], 0x00,
                                  0x00, 0x00, 0x90, 0x00};
        send_u2f_msg(empty_report, sizeof(empty_report));
    }
    goto done_handling;

reset:
    last_frame_header.id = 0xffff;
    last_frame_header.len = 0;
    memset(content_buf, 0, sizeof(content_buf));
    content_pos = 0;
    content_size = 0;
    mid_frame = false;

done_handling:
    return;
}

/*
 * tiny_msg_poll_and_buffer() - Poll usb port to check for tiny message from
 * host
 *
 * INPUT
 *     - block: flag to continually poll usb until tiny message is received
 *     - buf: pointer to destination buffer
 * OUTPUT
 *     message type
 *
 */
static MessageType tiny_msg_poll_and_buffer(bool block, uint8_t *buf) {
    m_msg_tiny_id = MSG_TINY_TYPE_ERROR;
    m_msg_tiny_flag = true;

    while (m_msg_tiny_id == MSG_TINY_TYPE_ERROR) {
        if (pdTRUE == xSemaphoreTake(msg_semaphore, 100)) {
            rx_helper();
        }

        if (!block) {
            break;
        }
    }

    m_msg_tiny_flag = false;

    if (m_msg_tiny_id != MSG_TINY_TYPE_ERROR) {
        memcpy(buf, m_msg_tiny, sizeof(m_msg_tiny));
    }

    return (m_msg_tiny_id);
}

/* === Functions =========================================================== */

/*
 * msg_map_init() - Setup message map with corresping message type
 *
 * INPUT
 *     - map: pointer message map array
 *     - size: size of message map
 * OUTPUT
 *
 */
void msg_map_init(const void *map, const size_t size) {
    assert(map != NULL);
    m_MessagesMap = map;
    m_map_size = size;
}

/*
 * set_msg_failure_handler() - Setup usb message failure handler
 *
 * INPUT
 *     - failure_func: message failure handler
 * OUTPUT
 *     none
 */
void set_msg_failure_handler(msg_failure_t failure_func) {
    m_msg_failure = failure_func;
}

/*
 * call_msg_failure_handler() - Call message failure handler
 *
 * INPUT
 *     - code: failure code
 *     - text: pinter to function arguments
 * OUTPUT
 *     none
 */
void call_msg_failure_handler(FailureType code, const char *text) {
    if (m_msg_failure) {
        (*m_msg_failure)(code, text);
    }
}

static void msg_task(void *arg) {
    UNUSED_PARAMETER(arg);
    NRF_LOG_DEBUG("Starting msg task...");
    while (1) {
        if (pdTRUE == xSemaphoreTake(msg_semaphore, portMAX_DELAY)) {
            rx_helper();
        }
    }
}

/*
 * msg_init() - Setup usb receive callback handler
 *
 * INPUT
 *     none
 * OUTPUT
 *     none
 */
void msg_init(void) {
    m_link_type = LINK_TYPE_USB;
    msg_semaphore = xSemaphoreCreateBinary();
    if (pdPASS != xTaskCreate(msg_task, "MSG", MSG_TASK_STACK_SIZE, NULL,
                              MSG_TASK_PRIORITY, &msg_task_handle)) {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
    usb_set_rx_parameters(&rx_message, &msg_semaphore);
    soter_ble_set_rx_parameters(&rx_message, &msg_semaphore);
}

/*
 * msg_write() - Transmit message over usb port
 *
 * INPUT
 *     - msg_id: protocol buffer message id
 *     - msg: pointer to message buffer
 * OUTPUT
 *     true/false status of write
 */
bool msg_write(MessageType msg_id, const void *msg) {
    const pb_field_t *fields = message_fields(NORMAL_MSG, msg_id, OUT_MSG);

    if (!fields)  // unknown message
    {
        NRF_LOG_DEBUG("Unknown message");
        return (false);
    }

    switch (m_link_type) {
        case LINK_TYPE_BLE:
            write_pb(fields, msg, msg_id, &soter_ble_tx);
            break;
        case LINK_TYPE_USB:
            write_pb(fields, msg, msg_id, &usb_tx);
            break;
        default:
            NRF_LOG_DEBUG("Unknown Link Type %d", m_link_type);
    }
    return (true);
}

/*
 * wait_for_tiny_msg() - Wait for usb tiny message type from host
 *
 * INPUT
 *     - buf: pointer to destination buffer
 * OUTPUT
 *     message tiny type
 *
 */
MessageType wait_for_tiny_msg(uint8_t *buf) {
    return (tiny_msg_poll_and_buffer(true, buf));
}

/*
 * check_for_tiny_msg() - Check for usb tiny message type from host
 *
 * INPUT
 *     - buf: pointer to destination buffer
 * OUTPUT
 *     message tiny type
 *
 */
MessageType check_for_tiny_msg(uint8_t *buf) {
    return (tiny_msg_poll_and_buffer(false, buf));
}

/*
 * parse_pb_varint() - Parses varints off of raw messages
 *
 * INPUT
 *     - msg: pointer to raw message
 *     - varint_count: how many varints to remove
 * OUTPUT
 *     bytes that were skipped
 */
uint32_t parse_pb_varint(RawMessage *msg, uint8_t varint_count) {
    uint32_t skip;
    uint8_t i;
    uint64_t pb_varint;
    pb_istream_t stream;

    /*
     * Parse varints
     */
    stream = pb_istream_from_buffer(msg->buffer, msg->length);
    skip = stream.bytes_left;
    for (i = 0; i < varint_count; ++i) {
        pb_decode_varint(&stream, &pb_varint);
    }
    skip = skip - stream.bytes_left;

    /*
     * Increment skip over message
     */
    msg->length -= skip;
    msg->buffer = (uint8_t *)(msg->buffer + skip);

    return skip;
}

/*
 * encode_pb() - convert to raw pb data
 *
 * INPUT
 *     - source_ptr : pointer to struct
 *     - fields: pointer pb fields
 *     - *buffer: pointer to destination buffer
 *     - len: size of buffer
 * OUTPUT
 *     bytes written to buffer
 */
int encode_pb(const void *source_ptr, const pb_field_t *fields, uint8_t *buffer,
              uint32_t len) {
    pb_ostream_t os = pb_ostream_from_buffer(buffer, len);

    if (pb_encode(&os, fields, source_ptr)) {
        return (os.bytes_written);
    } else {
        return (0);
    }
}
