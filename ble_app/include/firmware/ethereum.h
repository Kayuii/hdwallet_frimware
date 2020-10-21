/*
 * This file is part of the TREZOR project.
 *
 * Copyright (C) 2016 Alex Beregszaszi <alex@rtfs.hu>
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

#ifndef __ETHEREUM_H__
#define __ETHEREUM_H__

#include "trezor-crypto/bip32.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct _EthereumSignTx EthereumSignTx;
typedef struct _EthereumTxAck EthereumTxAck;
typedef struct _EthereumSignMessage EthereumSignMessage;
typedef struct _EthereumVerifyMessage EthereumVerifyMessage;
typedef struct _EthereumMessageSignature EthereumMessageSignature;
typedef struct _TokenType TokenType;

void ethereum_signing_init(EthereumSignTx *msg, const HDNode *node, bool needs_confirm);
void ethereum_signing_abort(void);
void ethereum_signing_txack(EthereumTxAck *msg);
void format_ethereum_address(const uint8_t *to, char *destination_str,
                             uint32_t destination_str_len);
bool is_token_transaction(const EthereumSignTx *msg);

/**
 * \brief Get the number of decimals associated with an erc20 token
 * \param   token_shorcut String corresponding to a token_shortcut in coins table in coins.c
 * \returns uint32_t      The number of decimals to interpret the token with
 */
uint32_t ethereum_get_decimal(const char *token_shortcut);

void ethereum_message_sign(const EthereumSignMessage *msg, const HDNode *node, EthereumMessageSignature *resp);
int ethereum_message_verify(const EthereumVerifyMessage *msg);

void ethereumFormatAmount(const bignum256 *amnt, const TokenType *token, uint32_t chain_id, char *buf, int buflen);

void bn_from_bytes(const uint8_t *value, size_t value_len, bignum256 *val);

#endif
