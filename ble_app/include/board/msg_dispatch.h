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

#ifndef MSG_DISPATCH_H
#define MSG_DISPATCH_H

#include "transport/interface.h"

#include "driver/usb.h"

#include <stdbool.h>
#include <stdint.h>

#define MSG_TINY_BFR_SZ 64
#define MSG_TINY_TYPE_ERROR 0xFFFF

#define MSG_IN(ID, STRUCT_NAME, PROCESS_FUNC, MSG_PERMS)              \
    [ID].msg_perms = (MSG_PERMS), [ID].msg_id = (ID),                 \
    [ID].type = (NORMAL_MSG), [ID].dir = (IN_MSG),                    \
    [ID].fields = (STRUCT_NAME##_fields), [ID].dispatch = (PARSABLE), \
    [ID].process_func = (void (*)(void *))(PROCESS_FUNC),

#define MSG_OUT(ID, STRUCT_NAME, PROCESS_FUNC, MSG_PERMS)             \
    [ID].msg_perms = (MSG_PERMS), [ID].msg_id = (ID),                 \
    [ID].type = (NORMAL_MSG), [ID].dir = (OUT_MSG),                   \
    [ID].fields = (STRUCT_NAME##_fields), [ID].dispatch = (PARSABLE), \
    [ID].process_func = (void (*)(void *))(PROCESS_FUNC),

#define RAW_IN(ID, STRUCT_NAME, PROCESS_FUNC, MSG_PERMS)         \
    [ID].msg_perms = (MSG_PERMS), [ID].msg_id = (ID),            \
    [ID].type = (NORMAL_MSG), [ID].dir = (IN_MSG),               \
    [ID].fields = (STRUCT_NAME##_fields), [ID].dispatch = (RAW), \
    [ID].process_func = (void (*)(void *))(PROCESS_FUNC),

#define NO_PROCESS_FUNC 0

/* === Typedefs ============================================================ */

typedef void (*msg_handler_t)(void *ptr);
typedef void (*msg_failure_t)(FailureType, const char *);
typedef bool (*msg_tx_handler_t)(uint8_t *, uint16_t);

typedef enum {
    NORMAL_MSG,
} MessageMapType;

typedef enum { IN_MSG, OUT_MSG } MessageMapDirection;

typedef enum { PARSABLE, RAW } MessageMapDispatch;

typedef enum {
    AnyVariant,
    MFROnly,
    MFRProhibited,
} MessageVariantPerms;

typedef struct {
    const pb_field_t *fields;
    msg_handler_t process_func;
    MessageMapDispatch dispatch;
    MessageMapType type;
    MessageMapDirection dir;
    MessageType msg_id;
    MessageVariantPerms msg_perms;
} MessagesMap_t;

typedef struct {
    uint8_t *buffer;
    uint32_t length;
} RawMessage;

typedef enum {
    RAW_MESSAGE_NOT_STARTED,
    RAW_MESSAGE_STARTED,
    RAW_MESSAGE_COMPLETE,
    RAW_MESSAGE_ERROR
} RawMessageState;

typedef void (*raw_msg_handler_t)(RawMessage *msg, uint32_t frame_length);

bool msg_write(MessageType msg_id, const void *msg);
bool msg_u2f_write(MessageType msg_id, const void *msg);

void msg_map_init(const void *map, const size_t size);
void set_msg_failure_handler(msg_failure_t failure_func);
void call_msg_failure_handler(FailureType code, const char *text);

void msg_init(void);

MessageType wait_for_tiny_msg(uint8_t *buf);
MessageType check_for_tiny_msg(uint8_t *buf);

uint32_t parse_pb_varint(RawMessage *msg, uint8_t varint_count);
int encode_pb(const void *source_ptr, const pb_field_t *fields, uint8_t *buffer,
              uint32_t len);
#endif
