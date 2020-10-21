/*
 * This file is part of the TREZOR project.
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
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

#include "u2f_knownapps.h"
#include "board/board.h"
#include "board/confirm_sm.h"
#include "board/layout.h"
#include "board/memory.h"
#include "board/msg_dispatch.h"
#include "board/u2f.h"
#include "driver/rng.h"
#include "firmware/app_confirm.h"
#include "firmware/app_layout.h"
#include "firmware/coins.h"
#include "firmware/crypto.h"
#include "firmware/ethereum.h"
#include "firmware/ethereum_tokens.h"
#include "firmware/exchange.h"
#include "firmware/fsm.h"
#include "firmware/passphrase_sm.h"
#include "firmware/pin_sm.h"
#include "firmware/policy.h"
#include "firmware/recovery.h"
#include "firmware/recovery_cipher.h"
#include "firmware/reset.h"
#include "firmware/signing.h"
#include "firmware/storage.h"
#include "firmware/transaction.h"
#include "firmware/util.h"
#include "trezor-crypto/address.h"
#include "trezor-crypto/aes/aes.h"
#include "trezor-crypto/base58.h"
#include "trezor-crypto/bip39.h"
#include "trezor-crypto/curves.h"
#include "trezor-crypto/ecdsa.h"
#include "trezor-crypto/hmac.h"
#include "trezor-crypto/memzero.h"
#include "trezor-crypto/rand.h"
#include "trezor-crypto/ripemd160.h"
#include "trezor-crypto/secp256k1.h"

#include "messages.pb.h"

#include <stdio.h>

#define _(X) (X)

static uint8_t msg_resp[MAX_FRAME_SIZE] __attribute__((aligned(4)));

#define CHECK_INITIALIZED                                   \
    if (!storage_isInitialized()) {                         \
        fsm_sendFailure(FailureType_Failure_NotInitialized, \
                        "Device not initialized");          \
        return;                                             \
    }

#define CHECK_NOT_INITIALIZED                                              \
    if (storage_isInitialized()) {                                         \
        fsm_sendFailure(FailureType_Failure_UnexpectedMessage,             \
                        "Device is already initialized. Use Wipe first."); \
        return;                                                            \
    }

#define CHECK_PIN                \
    if (!pin_protect_cached()) { \
        layout_home();           \
        return;                  \
    }

#define CHECK_PIN_TXSIGN         \
    if (!pin_protect_txsign()) { \
        layout_home();           \
        return;                  \
    }

#define CHECK_PARAM(cond, errormsg)                             \
    if (!(cond)) {                                              \
        fsm_sendFailure(FailureType_Failure_Other, (errormsg)); \
        layout_home();                                          \
        return;                                                 \
    }

static const MessagesMap_t MessagesMap[] = {
#include "messagemap.def"
};

#undef MSG_IN
#define MSG_IN(ID, STRUCT_NAME, PROCESS_FUNC, MSG_PERMS) \
    _Static_assert(sizeof(STRUCT_NAME) <= MAX_DECODE_SIZE, "Message too big");

#undef MSG_OUT
#define MSG_OUT(ID, STRUCT_NAME, PROCESS_FUNC, MSG_PERMS)

#undef RAW_IN
#define RAW_IN(ID, STRUCT_NAME, PROCESS_FUNC, MSG_PERMS) \
    _Static_assert(sizeof(STRUCT_NAME) <= MAX_DECODE_SIZE, "Message too big");

#undef DEBUG_IN
#define DEBUG_IN(ID, STRUCT_NAME, PROCESS_FUNC, MSG_PERMS) \
    _Static_assert(sizeof(STRUCT_NAME) <= MAX_DECODE_SIZE, "Message too big");

#undef DEBUG_OUT
#define DEBUG_OUT(ID, STRUCT_NAME, PROCESS_FUNC, MSG_PERMS)

#include "messagemap.def"

extern bool m_reset_msg_stack;

static const CoinType *fsm_getCoin(bool has_name, const char *name) {
    const CoinType *coin;
    if (has_name) {
        coin = coinByName(name);
    } else {
        coin = coinByName("Bitcoin");
    }
    if (!coin) {
        fsm_sendFailure(FailureType_Failure_Other, "Invalid coin name");
        layout_home();
        return 0;
    }

    return coin;
}

static HDNode *fsm_getDerivedNode(const char *curve, uint32_t *address_n,
                                  size_t address_n_count,
                                  uint32_t *fingerprint) {
    static HDNode CONFIDENTIAL node;
    if (fingerprint) {
        *fingerprint = 0;
    }

    if (!storage_getRootNode(curve, true, &node)) {
        fsm_sendFailure(
            FailureType_Failure_NotInitialized,
            "Device not initialized or passphrase request cancelled");
        layout_home();
        return 0;
    }

    if (!address_n || address_n_count == 0) {
        return &node;
    }

    if (hdnode_private_ckd_cached(&node, address_n, address_n_count,
                                  fingerprint) == 0) {
        fsm_sendFailure(FailureType_Failure_Other,
                        "Failed to derive private key");
        layout_home();
        return 0;
    }

    return &node;
}

void fsm_init(void) {
    msg_map_init(MessagesMap, sizeof(MessagesMap) / sizeof(MessagesMap_t));
    set_msg_failure_handler(&fsm_sendFailure);

    msg_init();
}

void fsm_sendSuccess(const char *text) {
    if (m_reset_msg_stack) {
        fsm_msgInitialize((Initialize *)0);
        m_reset_msg_stack = false;
        return;
    }

    RESP_INIT(Success);

    if (text) {
        resp->has_message = true;
        strlcpy(resp->message, text, sizeof(resp->message));
    }

    msg_write(MessageType_MessageType_Success, resp);
}

void fsm_sendFailure(FailureType code, const char *text) {
    if (m_reset_msg_stack) {
        fsm_msgInitialize((Initialize *)0);
        m_reset_msg_stack = false;
        return;
    }

    RESP_INIT(Failure);
    resp->has_code = true;
    resp->code = code;

    if (text) {
        resp->has_message = true;
        strlcpy(resp->message, text, sizeof(resp->message));
    }
    msg_write(MessageType_MessageType_Failure, resp);
}

void fsm_msgClearSession(ClearSession *msg) {
    (void)msg;
    session_clear(/*clear_pin=*/true);
    fsm_sendSuccess("Session cleared");
}

#include "fsm_msg_coin.h"
#include "fsm_msg_common.h"
#include "fsm_msg_crypto.h"
#include "fsm_msg_ethereum.h"
