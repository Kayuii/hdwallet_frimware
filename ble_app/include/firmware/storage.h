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

#ifndef STORAGE_H
#define STORAGE_H

#include "board/board.h"
#include "board/memory.h"
#include "firmware/policy.h"
#include "trezor-crypto/bip32.h"

typedef struct _VolatileParams {
    uint32_t pin_failed_attempts;
    uint32_t u2f_counter;
} __attribute__((aligned(4))) VolatileParams;

typedef struct _Public {
    uint8_t wrapped_storage_key[64];
    uint8_t storage_key_fingerprint[32];
    bool has_pin;
    uint32_t revision;
    bool has_language;
    char language[16];
    bool has_label;
    char label[48];
    bool imported;
    uint32_t policies_count;
    PolicyType policies[POLICY_COUNT];
    bool has_auto_lock_delay_ms;
    uint32_t auto_lock_delay_ms;
    bool passphrase_protection;
    bool initialized;
    bool has_node;
    bool has_mnemonic;
    bool has_u2froot;
    HDNodeType u2froot;
    bool no_backup;
} __attribute__((aligned(4))) Public;

/* Cache structure */
typedef struct _Cache {
    /* Root node cache */
    uint8_t root_seed_cache_status;
    uint8_t root_seed_cache[64];
    char root_ecdsa_curve_type[10];
} __attribute__((aligned(4))) Cache;

typedef struct _Secret {
    HDNodeType node;
    char mnemonic[241];
    char pin[10];
    Cache cache;
} __attribute__((aligned(4))) Secret;

typedef struct _Storage {
    uint32_t version;
    uint8_t uuid[STORAGE_UUID_LEN];
    char uuid_str[STORAGE_UUID_STR_LEN];
    Public pub;
    bool has_sec;
    uint32_t encrypted_sec_version;
    uint8_t encrypted_sec[512];
    uint8_t reserve[512];
} __attribute__((aligned(4))) Storage;

/* Must add case fall through in storage_fromFlash after increment*/
#define STORAGE_VERSION 1

typedef struct {
    uint16_t file_id;
    uint16_t key;
} FdsFileSector;

#define VOLATILE_FILE_ID (0x6E9C)
#define VOLATILE_REC_KEY (0x6A1B)

#define STORAGE_REC_SECTORS 3
#define STORAGE_FILE_ID_0 (0xAE91)
#define STORAGE_REC_KEY_0 (0x7010)

#define STORAGE_FILE_ID_1 (0xAD82)
#define STORAGE_REC_KEY_1 (0x8021)
#define STORAGE_FILE_ID_2 (0xAC73)
#define STORAGE_REC_KEY_2 (0x9032)

static const FdsFileSector fds_sector_map[STORAGE_REC_SECTORS] = {
    {STORAGE_FILE_ID_0, STORAGE_REC_KEY_0},
    {STORAGE_FILE_ID_1, STORAGE_REC_KEY_1},
    {STORAGE_FILE_ID_2, STORAGE_REC_KEY_2}};

/* 10 minutes */
#define STORAGE_DEFAULT_SCREENSAVER_TIMEOUT (10U * 60U * 1000U)                               

/* 30 seconds */
#define STORAGE_MIN_SCREENSAVER_TIMEOUT (30U * 1000U)

/// \brief Validate storage content and copy data to shadow memory.
void storage_init(void);

/// \brief Reset configuration UUID with random numbers.
void storage_resetUuid(void);

/// \brief Clear configuration.
void storage_reset(void);

/// \brief Reset session states.
/// \param clear_pin whether to clear the pin as well.
void session_clear(bool clear_pin);

/// \brief Write content of configuration in shadow memory to storage partion
///        in flash.
void storage_commit(void);

bool storage_is_active();
ret_code_t storage_read();

void volatile_params_write();
ret_code_t volatile_params_read();

/// \brief Load configuration data from usb message to shadow memory
typedef struct _LoadDevice LoadDevice;
void storage_loadDevice(LoadDevice *msg);

/// \brief Get the Root Node of the device.
/// \param node[out]  The Root Node.
/// \param curve[in]  ECDSA curve to use.
/// \param usePassphrase[in]  Whether the seed uses a passphrase.
/// \return true iff the root node was found.
bool storage_getRootNode(const char *curve, bool usePassphrase, HDNode *node);

/// \brief Fetch the node used for U2F signing.
/// \returns true iff retrieval was successful.
bool storage_getU2FRoot(HDNode *node);

/// \brief Increment and return the next value for the U2F counter.
uint32_t storage_nextU2FCounter(void);

/// \brief Assign a new value for the U2F Counter.
void storage_setU2FCounter(uint32_t u2f_counter);

/// \brief Set device label
void storage_setLabel(const char *label);

/// \brief Get device label
const char *storage_getLabel(void);

/// \brief Set device language.
void storage_setLanguage(const char *lang);

/// \brief Get device language.
const char *storage_getLanguage(void);

/// \brief Validate pin.
/// \return true iff the privided pin is correct.
bool storage_isPinCorrect(const char *pin);

bool storage_hasPin(void);
void storage_setPin(const char *pin);
void session_cachePin(const char *pin);
bool session_isPinCached(void);
void storage_resetPinFails(void);
void storage_increasePinFails(void);
uint32_t storage_getPinFails(void);

bool storage_isInitialized(void);

bool storage_noBackup(void);
void storage_setNoBackup(void);

const char *storage_getUuidStr(void);

bool storage_getPassphraseProtected(void);
void storage_setPassphraseProtected(bool passphrase);
void session_cachePassphrase(const char *passphrase);
bool session_isPassphraseCached(void);

/// \brief Set config mnemonic in shadow memory from words.
void storage_setMnemonicFromWords(const char (*words)[12],
                                  unsigned int num_words);

/// \brief Set config mnemonic from a recovery sentence.
void storage_setMnemonic(const char *mnemonic);

/// \brief Get mnemonic from shadow memory
const char *storage_getShadowMnemonic(void);

/// \returns true iff the private key stored on device was imported.
bool storage_getImported(void);

typedef struct _PolicyType PolicyType;

/// \brief Assign policy by name.
/// \returns true iff assignment was successful.
bool storage_setPolicy(const char *policy_name, bool enabled);

/// \brief Copy out all the policies in storage
/// \param policies[out]  Where to write the policies.
void storage_getPolicies(PolicyType *policies);

/// \brief Status of policy in storage
bool storage_isPolicyEnabled(const char *policy_name);

uint32_t storage_getAutoLockDelayMs(void);
void storage_setAutoLockDelayMs(uint32_t auto_lock_delay_ms);

void storage_loadNode(HDNode *dst, const HDNodeType *src);

/// Derive the wrapping key from the user's pin.
void storage_deriveWrappingKey(const char *pin, uint8_t wrapping_key[64]);

/// Wrap the storage key.
void storage_wrapStorageKey(const uint8_t wrapping_key[64],
                            const uint8_t key[64], uint8_t wrapped_key[64]);

/// Attempt to unnwrap the storage key.
void storage_unwrapStorageKey(const uint8_t wrapping_key[64],
                              const uint8_t wrapped_key[64], uint8_t key[64]);

/// Get the fingerprint for an unwrapped storage key.
void storage_keyFingerprint(const uint8_t key[64], uint8_t fingerprint[32]);

/// Check whether a pin is correct.
/// \returns true iff the pin was correct.
bool storage_isPinCorrect_impl(const char *pin, const uint8_t wrapped_key[64],
                               const uint8_t fingerprint[32], uint8_t key[64]);

/// Migrate data in Storage to/from sec/encrypted_sec.
void storage_secMigrate(Storage *storage, const uint8_t storage_key[64],
                        bool encrypt);

void storage_resetUuid_impl(Storage *cfg);

void storage_reset_shadow(Storage *cfg, uint8_t storage_key[64]);

void storage_setPin_impl(Storage *storage, const char *pin,
                         uint8_t storage_key[64]);
/// upgrade policies:clear all old policies and write new policies
void storage_upgradePolicies(Storage *storage);
/// reset policies
void storage_resetPolicies(Storage *storage);
/// reset cache
void storage_resetCache(Cache *cache);
/// write all configurations to flash
void storage_commit_impl(Storage *cfg);

/// \brief Get user private seed.
/// \returns NULL on error, otherwise \returns the private seed.
const uint8_t *storage_getSeed(const Storage *storage, bool usePassphrase);

void storage_readPolicy(PolicyType *policy, const char *policy_name,
                        bool enabled);

bool storage_setPolicy_impl(PolicyType policies[POLICY_COUNT],
                            const char *policy_name, bool enabled);
bool storage_isPolicyEnabled_impl(const PolicyType policies[POLICY_COUNT],
                                  const char *policy_name);

#endif
