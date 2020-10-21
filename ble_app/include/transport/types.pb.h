/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.0-dev at Thu Dec  6 15:21:23 2018. */

#ifndef PB_TYPES_PB_H_INCLUDED
#define PB_TYPES_PB_H_INCLUDED
#include <pb.h>

#include "exchange.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _FailureType {
    FailureType_Failure_UnexpectedMessage = 1,
    FailureType_Failure_ButtonExpected = 2,
    FailureType_Failure_SyntaxError = 3,
    FailureType_Failure_ActionCancelled = 4,
    FailureType_Failure_PinExpected = 5,
    FailureType_Failure_PinCancelled = 6,
    FailureType_Failure_PinInvalid = 7,
    FailureType_Failure_InvalidSignature = 8,
    FailureType_Failure_Other = 9,
    FailureType_Failure_NotEnoughFunds = 10,
    FailureType_Failure_NotInitialized = 11,
    FailureType_Failure_PinMismatch = 12,
    FailureType_Failure_FirmwareError = 99
} FailureType;
#define _FailureType_MIN FailureType_Failure_UnexpectedMessage
#define _FailureType_MAX FailureType_Failure_FirmwareError
#define _FailureType_ARRAYSIZE ((FailureType)(FailureType_Failure_FirmwareError+1))

typedef enum _OutputScriptType {
    OutputScriptType_PAYTOADDRESS = 0,
    OutputScriptType_PAYTOSCRIPTHASH = 1,
    OutputScriptType_PAYTOMULTISIG = 2,
    OutputScriptType_PAYTOOPRETURN = 3,
    OutputScriptType_PAYTOWITNESS = 4,
    OutputScriptType_PAYTOP2SHWITNESS = 5
} OutputScriptType;
#define _OutputScriptType_MIN OutputScriptType_PAYTOADDRESS
#define _OutputScriptType_MAX OutputScriptType_PAYTOP2SHWITNESS
#define _OutputScriptType_ARRAYSIZE ((OutputScriptType)(OutputScriptType_PAYTOP2SHWITNESS+1))

typedef enum _InputScriptType {
    InputScriptType_SPENDADDRESS = 0,
    InputScriptType_SPENDMULTISIG = 1,
    InputScriptType_EXTERNAL = 2,
    InputScriptType_SPENDWITNESS = 3,
    InputScriptType_SPENDP2SHWITNESS = 4
} InputScriptType;
#define _InputScriptType_MIN InputScriptType_SPENDADDRESS
#define _InputScriptType_MAX InputScriptType_SPENDP2SHWITNESS
#define _InputScriptType_ARRAYSIZE ((InputScriptType)(InputScriptType_SPENDP2SHWITNESS+1))

typedef enum _RequestType {
    RequestType_TXINPUT = 0,
    RequestType_TXOUTPUT = 1,
    RequestType_TXMETA = 2,
    RequestType_TXFINISHED = 3,
    RequestType_TXEXTRADATA = 4
} RequestType;
#define _RequestType_MIN RequestType_TXINPUT
#define _RequestType_MAX RequestType_TXEXTRADATA
#define _RequestType_ARRAYSIZE ((RequestType)(RequestType_TXEXTRADATA+1))

typedef enum _OutputAddressType {
    OutputAddressType_SPEND = 0,
    OutputAddressType_TRANSFER = 1,
    OutputAddressType_CHANGE = 2,
    OutputAddressType_EXCHANGE = 3
} OutputAddressType;
#define _OutputAddressType_MIN OutputAddressType_SPEND
#define _OutputAddressType_MAX OutputAddressType_EXCHANGE
#define _OutputAddressType_ARRAYSIZE ((OutputAddressType)(OutputAddressType_EXCHANGE+1))

typedef enum _ButtonRequestType {
    ButtonRequestType_ButtonRequest_Other = 1,
    ButtonRequestType_ButtonRequest_FeeOverThreshold = 2,
    ButtonRequestType_ButtonRequest_ConfirmOutput = 3,
    ButtonRequestType_ButtonRequest_ResetDevice = 4,
    ButtonRequestType_ButtonRequest_ConfirmWord = 5,
    ButtonRequestType_ButtonRequest_WipeDevice = 6,
    ButtonRequestType_ButtonRequest_ProtectCall = 7,
    ButtonRequestType_ButtonRequest_SignTx = 8,
    ButtonRequestType_ButtonRequest_FirmwareCheck = 9,
    ButtonRequestType_ButtonRequest_Address = 10,
    ButtonRequestType_ButtonRequest_FirmwareErase = 11,
    ButtonRequestType_ButtonRequest_ConfirmTransferToAccount = 12,
    ButtonRequestType_ButtonRequest_ConfirmTransferToNodePath = 13,
    ButtonRequestType_ButtonRequest_ChangeLabel = 14,
    ButtonRequestType_ButtonRequest_ChangeLanguage = 15,
    ButtonRequestType_ButtonRequest_EnablePassphrase = 16,
    ButtonRequestType_ButtonRequest_DisablePassphrase = 17,
    ButtonRequestType_ButtonRequest_EncryptAndSignMessage = 18,
    ButtonRequestType_ButtonRequest_EncryptMessage = 19,
    ButtonRequestType_ButtonRequest_ImportPrivateKey = 20,
    ButtonRequestType_ButtonRequest_ImportRecoverySentence = 21,
    ButtonRequestType_ButtonRequest_SignIdentity = 22,
    ButtonRequestType_ButtonRequest_Ping = 23,
    ButtonRequestType_ButtonRequest_RemovePin = 24,
    ButtonRequestType_ButtonRequest_ChangePin = 25,
    ButtonRequestType_ButtonRequest_CreatePin = 26,
    ButtonRequestType_ButtonRequest_GetEntropy = 27,
    ButtonRequestType_ButtonRequest_SignMessage = 28,
    ButtonRequestType_ButtonRequest_ApplyPolicies = 29,
    ButtonRequestType_ButtonRequest_SignExchange = 30,
    ButtonRequestType_ButtonRequest_AutoLockDelayMs = 31,
    ButtonRequestType_ButtonRequest_U2FCounter = 32
} ButtonRequestType;
#define _ButtonRequestType_MIN ButtonRequestType_ButtonRequest_Other
#define _ButtonRequestType_MAX ButtonRequestType_ButtonRequest_U2FCounter
#define _ButtonRequestType_ARRAYSIZE ((ButtonRequestType)(ButtonRequestType_ButtonRequest_U2FCounter+1))

typedef enum _PinMatrixRequestType {
    PinMatrixRequestType_PinMatrixRequestType_Current = 1,
    PinMatrixRequestType_PinMatrixRequestType_NewFirst = 2,
    PinMatrixRequestType_PinMatrixRequestType_NewSecond = 3
} PinMatrixRequestType;
#define _PinMatrixRequestType_MIN PinMatrixRequestType_PinMatrixRequestType_Current
#define _PinMatrixRequestType_MAX PinMatrixRequestType_PinMatrixRequestType_NewSecond
#define _PinMatrixRequestType_ARRAYSIZE ((PinMatrixRequestType)(PinMatrixRequestType_PinMatrixRequestType_NewSecond+1))

/* Struct definitions */
typedef PB_BYTES_ARRAY_T(128) CoinType_contract_address_t;
typedef PB_BYTES_ARRAY_T(32) CoinType_gas_limit_t;
typedef struct _CoinType {
    bool has_coin_name;
    char coin_name[17];
    bool has_coin_shortcut;
    char coin_shortcut[9];
    bool has_address_type;
    uint32_t address_type;
    bool has_maxfee_kb;
    uint64_t maxfee_kb;
    bool has_address_type_p2sh;
    uint32_t address_type_p2sh;
    bool has_address_type_p2wpkh;
    uint32_t address_type_p2wpkh;
    bool has_address_type_p2wsh;
    uint32_t address_type_p2wsh;
    bool has_signed_message_header;
    char signed_message_header[32];
    bool has_bip44_account_path;
    uint32_t bip44_account_path;
    bool has_forkid;
    uint32_t forkid;
    bool has_decimals;
    uint32_t decimals;
    bool has_contract_address;
    CoinType_contract_address_t contract_address;
    bool has_gas_limit;
    CoinType_gas_limit_t gas_limit;
    bool has_xpub_magic;
    uint32_t xpub_magic;
    bool has_xprv_magic;
    uint32_t xprv_magic;
    bool has_segwit;
    bool segwit;
    bool has_force_bip143;
    bool force_bip143;
    bool has_curve_name;
    char curve_name[21];
    bool has_cashaddr_prefix;
    char cashaddr_prefix[16];
    bool has_bech32_prefix;
    char bech32_prefix[21];
    bool has_decred;
    bool decred;
    bool has_version_group_id;
    uint32_t version_group_id;
    bool has_xpub_magic_segwit_p2sh;
    uint32_t xpub_magic_segwit_p2sh;
    bool has_xpub_magic_segwit_native;
    uint32_t xpub_magic_segwit_native;
/* @@protoc_insertion_point(struct:CoinType) */
} CoinType;

typedef struct _ExchangeType {
    bool has_signed_exchange_response;
    SignedExchangeResponse signed_exchange_response;
    bool has_withdrawal_coin_name;
    char withdrawal_coin_name[17];
    pb_size_t withdrawal_address_n_count;
    uint32_t withdrawal_address_n[8];
    pb_size_t return_address_n_count;
    uint32_t return_address_n[8];
/* @@protoc_insertion_point(struct:ExchangeType) */
} ExchangeType;

typedef PB_BYTES_ARRAY_T(32) HDNodeType_chain_code_t;
typedef PB_BYTES_ARRAY_T(32) HDNodeType_private_key_t;
typedef PB_BYTES_ARRAY_T(33) HDNodeType_public_key_t;
typedef struct _HDNodeType {
    uint32_t depth;
    uint32_t fingerprint;
    uint32_t child_num;
    HDNodeType_chain_code_t chain_code;
    bool has_private_key;
    HDNodeType_private_key_t private_key;
    bool has_public_key;
    HDNodeType_public_key_t public_key;
/* @@protoc_insertion_point(struct:HDNodeType) */
} HDNodeType;

typedef struct _IdentityType {
    bool has_proto;
    char proto[9];
    bool has_user;
    char user[64];
    bool has_host;
    char host[64];
    bool has_port;
    char port[6];
    bool has_path;
    char path[256];
    bool has_index;
    uint32_t index;
/* @@protoc_insertion_point(struct:IdentityType) */
} IdentityType;

typedef struct _PolicyType {
    bool has_policy_name;
    char policy_name[15];
    bool has_enabled;
    bool enabled;
/* @@protoc_insertion_point(struct:PolicyType) */
} PolicyType;

typedef PB_BYTES_ARRAY_T(0) RawTransactionType_payload_t;
typedef struct _RawTransactionType {
    RawTransactionType_payload_t payload;
/* @@protoc_insertion_point(struct:RawTransactionType) */
} RawTransactionType;

typedef PB_BYTES_ARRAY_T(520) TxOutputBinType_script_pubkey_t;
typedef struct _TxOutputBinType {
    uint64_t amount;
    TxOutputBinType_script_pubkey_t script_pubkey;
    bool has_decred_script_version;
    uint32_t decred_script_version;
/* @@protoc_insertion_point(struct:TxOutputBinType) */
} TxOutputBinType;

typedef PB_BYTES_ARRAY_T(32) TxRequestDetailsType_tx_hash_t;
typedef struct _TxRequestDetailsType {
    bool has_request_index;
    uint32_t request_index;
    bool has_tx_hash;
    TxRequestDetailsType_tx_hash_t tx_hash;
    bool has_extra_data_len;
    uint32_t extra_data_len;
    bool has_extra_data_offset;
    uint32_t extra_data_offset;
/* @@protoc_insertion_point(struct:TxRequestDetailsType) */
} TxRequestDetailsType;

typedef PB_BYTES_ARRAY_T(73) TxRequestSerializedType_signature_t;
typedef PB_BYTES_ARRAY_T(2048) TxRequestSerializedType_serialized_tx_t;
typedef struct _TxRequestSerializedType {
    bool has_signature_index;
    uint32_t signature_index;
    bool has_signature;
    TxRequestSerializedType_signature_t signature;
    bool has_serialized_tx;
    TxRequestSerializedType_serialized_tx_t serialized_tx;
/* @@protoc_insertion_point(struct:TxRequestSerializedType) */
} TxRequestSerializedType;

typedef struct _HDNodePathType {
    HDNodeType node;
    pb_size_t address_n_count;
    uint32_t address_n[8];
/* @@protoc_insertion_point(struct:HDNodePathType) */
} HDNodePathType;

typedef PB_BYTES_ARRAY_T(73) MultisigRedeemScriptType_signatures_t;
typedef struct _MultisigRedeemScriptType {
    pb_size_t pubkeys_count;
    HDNodePathType pubkeys[15];
    pb_size_t signatures_count;
    MultisigRedeemScriptType_signatures_t signatures[15];
    bool has_m;
    uint32_t m;
/* @@protoc_insertion_point(struct:MultisigRedeemScriptType) */
} MultisigRedeemScriptType;

typedef PB_BYTES_ARRAY_T(32) TxInputType_prev_hash_t;
typedef PB_BYTES_ARRAY_T(1650) TxInputType_script_sig_t;
typedef struct _TxInputType {
    pb_size_t address_n_count;
    uint32_t address_n[8];
    TxInputType_prev_hash_t prev_hash;
    uint32_t prev_index;
    bool has_script_sig;
    TxInputType_script_sig_t script_sig;
    bool has_sequence;
    uint32_t sequence;
    bool has_script_type;
    InputScriptType script_type;
    bool has_multisig;
    MultisigRedeemScriptType multisig;
    bool has_amount;
    uint64_t amount;
    bool has_decred_tree;
    uint32_t decred_tree;
    bool has_decred_script_version;
    uint32_t decred_script_version;
/* @@protoc_insertion_point(struct:TxInputType) */
} TxInputType;

typedef PB_BYTES_ARRAY_T(220) TxOutputType_op_return_data_t;
typedef struct _TxOutputType {
    bool has_address;
    char address[130];
    pb_size_t address_n_count;
    uint32_t address_n[8];
    uint64_t amount;
    OutputScriptType script_type;
    bool has_multisig;
    MultisigRedeemScriptType multisig;
    bool has_op_return_data;
    TxOutputType_op_return_data_t op_return_data;
    bool has_address_type;
    OutputAddressType address_type;
    bool has_exchange_type;
    ExchangeType exchange_type;
    bool has_decred_script_version;
    uint32_t decred_script_version;
/* @@protoc_insertion_point(struct:TxOutputType) */
} TxOutputType;

typedef PB_BYTES_ARRAY_T(1024) TransactionType_extra_data_t;
typedef struct _TransactionType {
    bool has_version;
    uint32_t version;
    pb_size_t inputs_count;
    TxInputType inputs[1];
    pb_size_t bin_outputs_count;
    TxOutputBinType bin_outputs[1];
    bool has_lock_time;
    uint32_t lock_time;
    pb_size_t outputs_count;
    TxOutputType outputs[1];
    bool has_inputs_cnt;
    uint32_t inputs_cnt;
    bool has_outputs_cnt;
    uint32_t outputs_cnt;
    bool has_extra_data;
    TransactionType_extra_data_t extra_data;
    bool has_extra_data_len;
    uint32_t extra_data_len;
    bool has_expiry;
    uint32_t expiry;
    bool has_overwintered;
    bool overwintered;
/* @@protoc_insertion_point(struct:TransactionType) */
} TransactionType;

/* Extensions */
extern const pb_extension_type_t wire_in; /* field type: bool wire_in; */
extern const pb_extension_type_t wire_out; /* field type: bool wire_out; */
extern const pb_extension_type_t wire_debug_in; /* field type: bool wire_debug_in; */
extern const pb_extension_type_t wire_debug_out; /* field type: bool wire_debug_out; */

/* Default values for struct fields */
extern const uint32_t CoinType_address_type_default;
extern const uint32_t CoinType_address_type_p2sh_default;
extern const uint32_t CoinType_address_type_p2wpkh_default;
extern const uint32_t CoinType_address_type_p2wsh_default;
extern const uint32_t CoinType_xpub_magic_default;
extern const uint32_t CoinType_xprv_magic_default;
extern const uint32_t TxInputType_sequence_default;
extern const InputScriptType TxInputType_script_type_default;
extern const uint32_t IdentityType_index_default;
extern const char ExchangeType_withdrawal_coin_name_default[17];

/* Initializer values for message structs */
#define HDNodeType_init_default                  {0, 0, 0, {0, {0}}, false, {0, {0}}, false, {0, {0}}}
#define HDNodePathType_init_default              {HDNodeType_init_default, 0, {0, 0, 0, 0, 0, 0, 0, 0}}
#define CoinType_init_default                    {false, "", false, "", false, 0u, false, 0, false, 5u, false, 6u, false, 10u, false, "", false, 0, false, 0, false, 0, false, {0, {0}}, false, {0, {0}}, false, 76067358u, false, 76066276u, false, 0, false, 0, false, "", false, "", false, "", false, 0, false, 0, false, 0, false, 0}
#define MultisigRedeemScriptType_init_default    {0, {HDNodePathType_init_default, HDNodePathType_init_default, HDNodePathType_init_default, HDNodePathType_init_default, HDNodePathType_init_default, HDNodePathType_init_default, HDNodePathType_init_default, HDNodePathType_init_default, HDNodePathType_init_default, HDNodePathType_init_default, HDNodePathType_init_default, HDNodePathType_init_default, HDNodePathType_init_default, HDNodePathType_init_default, HDNodePathType_init_default}, 0, {{0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}}, false, 0}
#define TxInputType_init_default                 {0, {0, 0, 0, 0, 0, 0, 0, 0}, {0, {0}}, 0, false, {0, {0}}, false, 4294967295u, false, InputScriptType_SPENDADDRESS, false, MultisigRedeemScriptType_init_default, false, 0, false, 0, false, 0}
#define TxOutputType_init_default                {false, "", 0, {0, 0, 0, 0, 0, 0, 0, 0}, 0, _OutputScriptType_MIN, false, MultisigRedeemScriptType_init_default, false, {0, {0}}, false, _OutputAddressType_MIN, false, ExchangeType_init_default, false, 0}
#define TxOutputBinType_init_default             {0, {0, {0}}, false, 0}
#define TransactionType_init_default             {false, 0, 0, {TxInputType_init_default}, 0, {TxOutputBinType_init_default}, false, 0, 0, {TxOutputType_init_default}, false, 0, false, 0, false, {0, {0}}, false, 0, false, 0, false, 0}
#define RawTransactionType_init_default          {{0, {0}}}
#define TxRequestDetailsType_init_default        {false, 0, false, {0, {0}}, false, 0, false, 0}
#define TxRequestSerializedType_init_default     {false, 0, false, {0, {0}}, false, {0, {0}}}
#define IdentityType_init_default                {false, "", false, "", false, "", false, "", false, "", false, 0u}
#define PolicyType_init_default                  {false, "", false, 0}
#define ExchangeType_init_default                {false, SignedExchangeResponse_init_default, false, "Bitcoin", 0, {0, 0, 0, 0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0, 0, 0, 0}}
#define HDNodeType_init_zero                     {0, 0, 0, {0, {0}}, false, {0, {0}}, false, {0, {0}}}
#define HDNodePathType_init_zero                 {HDNodeType_init_zero, 0, {0, 0, 0, 0, 0, 0, 0, 0}}
#define CoinType_init_zero                       {false, "", false, "", false, 0, false, 0, false, 0, false, 0, false, 0, false, "", false, 0, false, 0, false, 0, false, {0, {0}}, false, {0, {0}}, false, 0, false, 0, false, 0, false, 0, false, "", false, "", false, "", false, 0, false, 0, false, 0, false, 0}
#define MultisigRedeemScriptType_init_zero       {0, {HDNodePathType_init_zero, HDNodePathType_init_zero, HDNodePathType_init_zero, HDNodePathType_init_zero, HDNodePathType_init_zero, HDNodePathType_init_zero, HDNodePathType_init_zero, HDNodePathType_init_zero, HDNodePathType_init_zero, HDNodePathType_init_zero, HDNodePathType_init_zero, HDNodePathType_init_zero, HDNodePathType_init_zero, HDNodePathType_init_zero, HDNodePathType_init_zero}, 0, {{0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}}, false, 0}
#define TxInputType_init_zero                    {0, {0, 0, 0, 0, 0, 0, 0, 0}, {0, {0}}, 0, false, {0, {0}}, false, 0, false, _InputScriptType_MIN, false, MultisigRedeemScriptType_init_zero, false, 0, false, 0, false, 0}
#define TxOutputType_init_zero                   {false, "", 0, {0, 0, 0, 0, 0, 0, 0, 0}, 0, _OutputScriptType_MIN, false, MultisigRedeemScriptType_init_zero, false, {0, {0}}, false, _OutputAddressType_MIN, false, ExchangeType_init_zero, false, 0}
#define TxOutputBinType_init_zero                {0, {0, {0}}, false, 0}
#define TransactionType_init_zero                {false, 0, 0, {TxInputType_init_zero}, 0, {TxOutputBinType_init_zero}, false, 0, 0, {TxOutputType_init_zero}, false, 0, false, 0, false, {0, {0}}, false, 0, false, 0, false, 0}
#define RawTransactionType_init_zero             {{0, {0}}}
#define TxRequestDetailsType_init_zero           {false, 0, false, {0, {0}}, false, 0, false, 0}
#define TxRequestSerializedType_init_zero        {false, 0, false, {0, {0}}, false, {0, {0}}}
#define IdentityType_init_zero                   {false, "", false, "", false, "", false, "", false, "", false, 0}
#define PolicyType_init_zero                     {false, "", false, 0}
#define ExchangeType_init_zero                   {false, SignedExchangeResponse_init_zero, false, "", 0, {0, 0, 0, 0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0, 0, 0, 0}}

/* Field tags (for use in manual encoding/decoding) */
#define CoinType_coin_name_tag                   1
#define CoinType_coin_shortcut_tag               2
#define CoinType_address_type_tag                3
#define CoinType_maxfee_kb_tag                   4
#define CoinType_address_type_p2sh_tag           5
#define CoinType_address_type_p2wpkh_tag         6
#define CoinType_address_type_p2wsh_tag          7
#define CoinType_signed_message_header_tag       8
#define CoinType_bip44_account_path_tag          9
#define CoinType_forkid_tag                      12
#define CoinType_decimals_tag                    13
#define CoinType_contract_address_tag            14
#define CoinType_gas_limit_tag                   15
#define CoinType_xpub_magic_tag                  16
#define CoinType_xprv_magic_tag                  17
#define CoinType_segwit_tag                      18
#define CoinType_force_bip143_tag                19
#define CoinType_curve_name_tag                  20
#define CoinType_cashaddr_prefix_tag             21
#define CoinType_bech32_prefix_tag               22
#define CoinType_decred_tag                      23
#define CoinType_version_group_id_tag            24
#define CoinType_xpub_magic_segwit_p2sh_tag      25
#define CoinType_xpub_magic_segwit_native_tag    26
#define ExchangeType_signed_exchange_response_tag 1
#define ExchangeType_withdrawal_coin_name_tag    2
#define ExchangeType_withdrawal_address_n_tag    3
#define ExchangeType_return_address_n_tag        4
#define HDNodeType_depth_tag                     1
#define HDNodeType_fingerprint_tag               2
#define HDNodeType_child_num_tag                 3
#define HDNodeType_chain_code_tag                4
#define HDNodeType_private_key_tag               5
#define HDNodeType_public_key_tag                6
#define IdentityType_proto_tag                   1
#define IdentityType_user_tag                    2
#define IdentityType_host_tag                    3
#define IdentityType_port_tag                    4
#define IdentityType_path_tag                    5
#define IdentityType_index_tag                   6
#define PolicyType_policy_name_tag               1
#define PolicyType_enabled_tag                   2
#define RawTransactionType_payload_tag           1
#define TxOutputBinType_amount_tag               1
#define TxOutputBinType_script_pubkey_tag        2
#define TxOutputBinType_decred_script_version_tag 3
#define TxRequestDetailsType_request_index_tag   1
#define TxRequestDetailsType_tx_hash_tag         2
#define TxRequestDetailsType_extra_data_len_tag  3
#define TxRequestDetailsType_extra_data_offset_tag 4
#define TxRequestSerializedType_signature_index_tag 1
#define TxRequestSerializedType_signature_tag    2
#define TxRequestSerializedType_serialized_tx_tag 3
#define HDNodePathType_node_tag                  1
#define HDNodePathType_address_n_tag             2
#define MultisigRedeemScriptType_pubkeys_tag     1
#define MultisigRedeemScriptType_signatures_tag  2
#define MultisigRedeemScriptType_m_tag           3
#define TxInputType_address_n_tag                1
#define TxInputType_prev_hash_tag                2
#define TxInputType_prev_index_tag               3
#define TxInputType_script_sig_tag               4
#define TxInputType_sequence_tag                 5
#define TxInputType_script_type_tag              6
#define TxInputType_multisig_tag                 7
#define TxInputType_amount_tag                   8
#define TxInputType_decred_tree_tag              9
#define TxInputType_decred_script_version_tag    10
#define TxOutputType_address_tag                 1
#define TxOutputType_address_n_tag               2
#define TxOutputType_amount_tag                  3
#define TxOutputType_script_type_tag             4
#define TxOutputType_multisig_tag                5
#define TxOutputType_op_return_data_tag          6
#define TxOutputType_address_type_tag            7
#define TxOutputType_exchange_type_tag           8
#define TxOutputType_decred_script_version_tag   9
#define TransactionType_version_tag              1
#define TransactionType_inputs_tag               2
#define TransactionType_bin_outputs_tag          3
#define TransactionType_outputs_tag              5
#define TransactionType_lock_time_tag            4
#define TransactionType_inputs_cnt_tag           6
#define TransactionType_outputs_cnt_tag          7
#define TransactionType_extra_data_tag           8
#define TransactionType_extra_data_len_tag       9
#define TransactionType_expiry_tag               10
#define TransactionType_overwintered_tag         11
#define wire_in_tag                              50002
#define wire_out_tag                             50003
#define wire_debug_in_tag                        50004
#define wire_debug_out_tag                       50005

/* Struct field encoding specification for nanopb */
extern const pb_field_t HDNodeType_fields[7];
extern const pb_field_t HDNodePathType_fields[3];
extern const pb_field_t CoinType_fields[25];
extern const pb_field_t MultisigRedeemScriptType_fields[4];
extern const pb_field_t TxInputType_fields[11];
extern const pb_field_t TxOutputType_fields[10];
extern const pb_field_t TxOutputBinType_fields[4];
extern const pb_field_t TransactionType_fields[12];
extern const pb_field_t RawTransactionType_fields[2];
extern const pb_field_t TxRequestDetailsType_fields[5];
extern const pb_field_t TxRequestSerializedType_fields[4];
extern const pb_field_t IdentityType_fields[7];
extern const pb_field_t PolicyType_fields[3];
extern const pb_field_t ExchangeType_fields[5];

/* Maximum encoded size of messages (where known) */
#define HDNodeType_size                          121
#define HDNodePathType_size                      171
#define CoinType_size                            393
#define MultisigRedeemScriptType_size            3741
#define TxInputType_size                         5516
#define TxOutputType_size                        (4296 + SignedExchangeResponse_size)
#define TxOutputBinType_size                     540
#define TransactionType_size                     (11432 + 1*SignedExchangeResponse_size)
#define RawTransactionType_size                  2
#define TxRequestDetailsType_size                52
#define TxRequestSerializedType_size             2132
#define IdentityType_size                        416
#define PolicyType_size                          19
#define ExchangeType_size                        (121 + SignedExchangeResponse_size)

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define TYPES_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
