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

#include "firmware/storage.h"

#include "board/board.h"
#include "board/memory.h"
#include "board/u2f.h"
#include "driver/flash_fds.h"
#include "driver/rng.h"
#include "firmware/fsm.h"
#include "firmware/passphrase_sm.h"
#include "firmware/policy.h"
#include "firmware/util.h"
#include "transport/interface.h"
#include "trezor-crypto/aes/aes.h"
#include "trezor-crypto/bip32.h"
#include "trezor-crypto/bip39.h"
#include "trezor-crypto/curves.h"
#include "trezor-crypto/memzero.h"
#include "trezor-crypto/pbkdf2.h"
#include "trezor-crypto/rand.h"

#include <stdint.h>
#include <string.h>

#ifndef MAX
#define MAX(a, b)           \
    ({                      \
        typeof(a) _a = (a); \
        typeof(b) _b = (b); \
        _a > _b ? _a : _b;  \
    })
#endif

static bool m_sessionSeedCached, m_sessionSeedUsesPassphrase;
static uint8_t CONFIDENTIAL m_sessionSeed[64];

static bool m_sessionPinCached;
static uint8_t CONFIDENTIAL m_sessionStorageKey[64];

static bool m_sessionPassphraseCached;
static char CONFIDENTIAL m_sessionPassphrase[51];

/* Shadow memory for configuration data in storage partition */
_Static_assert(sizeof(Storage) <= (FDS_VIRTUAL_PAGE_SIZE - 5) * 4,
               "ConfigFlash struct is too large for storage partition");

static Secret CONFIDENTIAL m_secret;
static Storage CONFIDENTIAL m_shadow_storage;
static VolatileParams CONFIDENTIAL m_volatile_params;

static void get_u2froot_callback(uint32_t iter, uint32_t total) {
    (void)iter;
    (void)total;
    // layoutProgress(_("Updating"), 1000 * iter / total);
    animating_progress_handler();
}

static void storage_compute_u2froot(const char *mnemonic, HDNodeType *u2froot) {
    static CONFIDENTIAL HDNode node;
    mnemonic_to_seed(mnemonic, "", m_sessionSeed,
                     get_u2froot_callback);  // BIP-0039
    hdnode_from_seed(m_sessionSeed, 64, NIST256P1_NAME, &node);
    hdnode_private_ckd(&node, U2F_KEY_PATH);
    u2froot->depth = node.depth;
    u2froot->child_num = U2F_KEY_PATH;
    u2froot->chain_code.size = sizeof(node.chain_code);
    memcpy(u2froot->chain_code.bytes, node.chain_code, sizeof(node.chain_code));
    u2froot->has_private_key = true;
    u2froot->private_key.size = sizeof(node.private_key);
    memcpy(u2froot->private_key.bytes, node.private_key,
           sizeof(node.private_key));
    memzero(&node, sizeof(node));
}

bool storage_getU2FRoot(HDNode *node) {
    return m_shadow_storage.pub.has_u2froot &&
           hdnode_from_xprv(m_shadow_storage.pub.u2froot.depth,
                            m_shadow_storage.pub.u2froot.child_num,
                            m_shadow_storage.pub.u2froot.chain_code.bytes,
                            m_shadow_storage.pub.u2froot.private_key.bytes,
                            NIST256P1_NAME, node);
}

uint32_t storage_nextU2FCounter(void) {
    m_volatile_params.u2f_counter++;
    volatile_params_write();
    return m_volatile_params.u2f_counter;
}

void storage_setU2FCounter(uint32_t u2f_counter) {
    m_volatile_params.u2f_counter = u2f_counter;
    volatile_params_write();
}

void storage_upgradePolicies(Storage *storage) {
    for (int i = storage->pub.policies_count; i < (int)(POLICY_COUNT); ++i) {
        memzero(&storage->pub.policies[i], sizeof(storage->pub.policies[i]));
        memcpy(&storage->pub.policies[i], &policies[i],
               sizeof(storage->pub.policies[i]));
    }
    storage->pub.policies_count = POLICY_COUNT;
}

void storage_resetPolicies(Storage *storage) {
    storage->pub.policies_count = 0;
    storage_upgradePolicies(storage);
}

void storage_resetCache(Cache *cache) { memzero(cache, sizeof(*cache)); }

void storage_readPolicy(PolicyType *policy, const char *policy_name,
                        bool enabled) {
    policy->has_policy_name = true;
    memzero(policy->policy_name, sizeof(policy->policy_name));
    strncpy(policy->policy_name, policy_name, sizeof(policy->policy_name));
    policy->has_enabled = true;
    policy->enabled = enabled;
}

void storage_deriveWrappingKey(const char *pin, uint8_t wrapping_key[64]) {
    sha512_Raw((const uint8_t *)pin, strlen(pin), wrapping_key);
}

void storage_wrapStorageKey(const uint8_t wrapping_key[64],
                            const uint8_t key[64], uint8_t wrapped_key[64]) {
    uint8_t iv[64];
    memcpy(iv, wrapping_key, sizeof(iv));
    aes_encrypt_ctx ctx;
    aes_encrypt_key256(wrapping_key, &ctx);
    aes_cbc_encrypt(key, wrapped_key, 64, iv + 32, &ctx);
    memzero(&ctx, sizeof(ctx));
    memzero(iv, sizeof(iv));
}

void storage_unwrapStorageKey(const uint8_t wrapping_key[64],
                              const uint8_t wrapped_key[64], uint8_t key[64]) {
    uint8_t iv[64];
    memcpy(iv, wrapping_key, sizeof(iv));
    aes_decrypt_ctx ctx;
    aes_decrypt_key256(wrapping_key, &ctx);
    aes_cbc_decrypt(wrapped_key, key, 64, iv + 32, &ctx);
    memzero(&ctx, sizeof(ctx));
    memzero(iv, sizeof(iv));
}

void storage_keyFingerprint(const uint8_t key[64], uint8_t fingerprint[32]) {
    sha256_Raw(key, 64, fingerprint);
}

bool storage_isPinCorrect_impl(const char *pin, const uint8_t wrapped_key[64],
                               const uint8_t fingerprint[32], uint8_t key[64]) {
    uint8_t wrapping_key[64];
    storage_deriveWrappingKey(pin, wrapping_key);
    storage_unwrapStorageKey(wrapping_key, wrapped_key, key);

    uint8_t fp[32];
    storage_keyFingerprint(key, fp);

    bool ret = memcmp(fp, fingerprint, 32) == 0;
    if (!ret) memzero(key, 64);
    memzero(wrapping_key, 64);
    memzero(fp, 32);
    return ret;
}

void storage_secMigrate(Storage *storage, const uint8_t storage_key[64],
                        bool encrypt) {
    static CONFIDENTIAL char scratch[512];
    _Static_assert(sizeof(scratch) == sizeof(storage->encrypted_sec),
                   "Be extermely careful when changing the size of scratch.");
    memzero(scratch, sizeof(scratch));

    if (encrypt) {
        memzero(storage->encrypted_sec, sizeof(storage->encrypted_sec));

        // Serialize to scratch.
        memcpy(scratch, &m_secret, sizeof(Secret));

        // Encrypt with the storage key.
        uint8_t iv[64];
        memcpy(iv, storage_key, sizeof(iv));
        aes_encrypt_ctx ctx;
        aes_encrypt_key256(storage_key, &ctx);
        aes_cbc_encrypt((const uint8_t *)scratch, storage->encrypted_sec,
                        sizeof(scratch), iv + 32, &ctx);
        memzero(&ctx, sizeof(ctx));
        storage->encrypted_sec_version = STORAGE_VERSION;
    } else {
        memzero(&m_secret, sizeof(m_secret));

        // Decrypt with the storage key.
        uint8_t iv[64];
        memcpy(iv, storage_key, sizeof(iv));
        aes_decrypt_ctx ctx;
        aes_decrypt_key256(storage_key, &ctx);
        if (EXIT_FAILURE ==
            aes_cbc_decrypt((const uint8_t *)storage->encrypted_sec,
                            (uint8_t *)&scratch[0], sizeof(scratch), iv + 32,
                            &ctx)) {
            memzero(iv, sizeof(iv));
            memzero(scratch, sizeof(scratch));
            return;
        }

        // De-serialize from scratch.
        memcpy(&m_secret, scratch, sizeof(Secret));
        NRF_LOG_DEBUG("Mnemonic: %s", &(m_secret.mnemonic[0]));
        // Derive the u2froot, if we haven't already.
        if (storage->pub.has_mnemonic && !storage->pub.has_u2froot) {
            storage_compute_u2froot(m_secret.mnemonic, &storage->pub.u2froot);
            storage->pub.has_u2froot = true;
        }

        storage->has_sec = true;
    }

    memzero(scratch, sizeof(scratch));
}

_Static_assert(offsetof(Cache, root_seed_cache) == 1, "rsc");
_Static_assert(offsetof(Cache, root_ecdsa_curve_type) == 65, "rect");
_Static_assert(sizeof(((Cache *)0)->root_ecdsa_curve_type) == 10, "rect");

/// \brief Set root session seed in storage.
///
/// \param cfg[in]    The active storage sector.
/// \param seed[in]   Root seed to write into storage.
/// \param curve[in]  ECDSA curve name being used.
static void storage_setRootSeedCache(Storage *storage, const uint8_t *seed,
                                     const char *curve) {
    // Don't cache when passphrase protection is enabled.
    if (storage->pub.passphrase_protection && strlen(m_sessionPassphrase))
        return;

    memzero(&m_secret.cache, sizeof(m_secret.cache));

    memcpy(&m_secret.cache.root_seed_cache, seed,
           sizeof(m_secret.cache.root_seed_cache));

    strlcpy(m_secret.cache.root_ecdsa_curve_type, curve,
            sizeof(m_secret.cache.root_ecdsa_curve_type));

    m_secret.cache.root_seed_cache_status = CACHE_EXISTS;
    storage->has_sec = true;
    storage_commit();
}

/// \brief Get root session seed cache from storage.
///
/// \param cfg[in]   The active storage sector.
/// \param curve[in] ECDSA curve name being used.
/// \param seed[out] The root seed value.
/// \returns true on success.
static bool storage_getRootSeedCache(Storage *storage, const char *curve,
                                     bool usePassphrase, uint8_t *seed) {
    if (!storage->has_sec) return false;

    if (m_secret.cache.root_seed_cache_status != CACHE_EXISTS) return false;

    if (usePassphrase && storage->pub.passphrase_protection &&
        strlen(m_sessionPassphrase)) {
        return false;
    }

    if (strcmp(m_secret.cache.root_ecdsa_curve_type, curve) != 0) {
        return false;
    }

    memzero(seed, sizeof(m_sessionSeed));
    memcpy(seed, &m_secret.cache.root_seed_cache,
           sizeof(m_secret.cache.root_seed_cache));
    _Static_assert(
        sizeof(m_sessionSeed) == sizeof(m_secret.cache.root_seed_cache),
        "size mismatch");
    return true;
}

ret_code_t storage_read() {
    Storage temp_storage;
    memzero(&temp_storage, sizeof(Storage));
    memzero(&m_shadow_storage, sizeof(Storage));
    ret_code_t rc = FDS_ERR_INTERNAL;
    uint32_t min_revision = 0xFFFFFFFF;
    uint32_t data_len = sizeof(Storage);

    for (int i = STORAGE_REC_SECTORS - 1; i >= 0; i--) {
        uint16_t fid = fds_sector_map[i].file_id;
        uint16_t key = fds_sector_map[i].key;
        rc = record_read(fid, key, &temp_storage, &data_len);
        if (rc == FDS_SUCCESS) {
            min_revision = (temp_storage.pub.revision < min_revision)
                               ? temp_storage.pub.revision
                               : min_revision;
            if (m_shadow_storage.pub.revision < temp_storage.pub.revision) {
                memcpy(&m_shadow_storage, &temp_storage, sizeof(Storage));
            }
        } else if (rc == FDS_ERR_CRC_CHECK_FAILED) {
            return FDS_ERR_CRC_CHECK_FAILED;
        }
    }

    if (m_shadow_storage.pub.revision == 0) {
        return FDS_ERR_INTERNAL;
    }

    if (min_revision < m_shadow_storage.pub.revision) {
        storage_commit();
    }

    rc = volatile_params_read();

    return rc;
}

ret_code_t volatile_params_read() {
    ret_code_t rc = FDS_ERR_INTERNAL;
    memzero(&m_volatile_params, sizeof(VolatileParams));
    uint32_t data_len = sizeof(VolatileParams);
    rc = record_read(VOLATILE_FILE_ID, VOLATILE_REC_KEY, &m_volatile_params,
                     &data_len);
    return rc;
}

bool storage_is_active() {
    bool ret = false;
    for (int i = 0; i < STORAGE_REC_SECTORS; i++) {
        uint16_t fid = fds_sector_map[i].file_id;
        uint16_t key = fds_sector_map[i].key;
        if (record_exists(fid, key)) {
            ret = true;
            break;
        }
    }
    return ret;
}

void storage_init(void) {
    // Reset shadow configuration in RAM
    storage_fds_init();
    storage_reset_shadow(&m_shadow_storage, m_sessionStorageKey);
    if (!storage_is_active()) {
        storage_resetUuid();
        storage_commit();
        storage_resetPinFails();
    }
    ret_code_t rc = storage_read();
    if (rc == FDS_ERR_CRC_CHECK_FAILED) {
        layout_standard_notification("The Wallet Can't Work!",
                                     "Please contact support to restore data!",
                                     NOTIFICATION_INFO);
        while (true) {
            vTaskDelay(TIME_TASK_DELAY);
        }
    } else if (rc != FDS_SUCCESS) {
        storage_reset();
        storage_resetUuid();
        storage_commit();
        storage_resetPinFails();
        NRF_LOG_ERROR("Can not initialize flash storage!\n");
        layout_warning_static("Flash Initialize failed. Reboot Device!");
        return;
    }

    data2hex(m_shadow_storage.uuid, sizeof(m_shadow_storage.uuid),
             m_shadow_storage.uuid_str);

    if (!storage_hasPin()) session_cachePin("");
}

void storage_resetUuid(void) { storage_resetUuid_impl(&m_shadow_storage); }

void storage_resetUuid_impl(Storage *storage) {
    random_buffer(storage->uuid, sizeof(storage->uuid));
    const uint16_t high_bytes = (uint16_t)NRF_FICR->DEVICEADDR[1] | 0xC000;
    const uint32_t low_bytes = NRF_FICR->DEVICEADDR[0];
    storage->uuid[0] = low_bytes & 0x000000ff;
    storage->uuid[1] = (low_bytes >> 8) & 0x000000ff;
    storage->uuid[2] = (low_bytes >> 16) & 0x000000ff;
    storage->uuid[3] = (low_bytes >> 24) & 0x000000ff;
    storage->uuid[4] = high_bytes & 0x00ff;
    storage->uuid[5] = (high_bytes >> 8) & 0x00ff;
    data2hex(storage->uuid, sizeof(storage->uuid), storage->uuid_str);
}

void storage_reset(void) {
    storage_reset_shadow(&m_shadow_storage, m_sessionStorageKey);
    for (uint8_t i = 0; i < STORAGE_REC_SECTORS; i++) {
        record_delete(fds_sector_map[i].file_id, fds_sector_map[i].key);
    }
    record_delete(VOLATILE_FILE_ID, VOLATILE_REC_KEY);
    record_gc();
}

void storage_reset_shadow(Storage *storage, uint8_t storage_key[64]) {
    memzero(storage, sizeof(Storage));
    memzero(&m_volatile_params, sizeof(VolatileParams));

    storage_resetPolicies(storage);
    storage_setPin_impl(storage, "", storage_key);

    storage->version = STORAGE_VERSION;

    m_sessionPinCached = false;
    m_sessionSeedCached = false;
    m_sessionPassphraseCached = false;

    memzero(&m_sessionSeed, sizeof(m_sessionSeed));
    memzero(&m_sessionPassphrase, sizeof(m_sessionPassphrase));
    memzero(m_sessionStorageKey, sizeof(m_sessionStorageKey));

    storage->has_sec = false;
    memzero(&m_secret, sizeof(Secret));
}

void session_clear(bool clear_pin) {
    m_sessionSeedCached = false;
    memzero(&m_sessionSeed, sizeof(m_sessionSeed));

    m_sessionPassphraseCached = false;
    memzero(&m_sessionPassphrase, sizeof(m_sessionPassphrase));

    if (storage_hasPin()) {
        if (clear_pin) {
            memzero(m_sessionStorageKey, sizeof(m_sessionStorageKey));
            m_sessionPinCached = false;
            m_shadow_storage.has_sec = false;
            memzero(&m_secret, sizeof(m_secret));
        }
    } else {
        session_cachePin("");
    }
}

void storage_commit(void) { storage_commit_impl(&m_shadow_storage); }

void storage_commit_impl(Storage *storage) {
    ret_code_t rc = FDS_ERR_INTERNAL;

    if (m_sessionPinCached) {
        storage_secMigrate(storage, m_sessionStorageKey, true);
    }

    storage->pub.revision++;

    for (int i = 0; i < STORAGE_REC_SECTORS; i++) {
        uint16_t fid = fds_sector_map[i].file_id;
        uint16_t key = fds_sector_map[i].key;
        rc = record_write(fid, key, storage, sizeof(Storage));
    }

    if (rc != FDS_SUCCESS) {
        layout_warning_static("Write Storage Error Detected. Reboot Device!");
    }
}

void volatile_params_write() {
    ret_code_t rc = FDS_ERR_INTERNAL;
    rc = record_write(VOLATILE_FILE_ID, VOLATILE_REC_KEY, &m_volatile_params,
                      sizeof(VolatileParams));
    if (rc != FDS_SUCCESS) {
        layout_warning_static("Writer params Error Detected. Reboot Device!");
    }
}

// Great candidate for C++ templates... sigh.
void storage_dumpNode(HDNodeType *dst, const HDNode *src) {
    (void)dst;
    (void)src;
}

void storage_loadNode(HDNode *dst, const HDNodeType *src) {
    dst->depth = src->depth;
    dst->child_num = src->child_num;

    memcpy(dst->chain_code, src->chain_code.bytes,
           sizeof(src->chain_code.bytes));
    _Static_assert(sizeof(dst->chain_code) == sizeof(src->chain_code.bytes),
                   "chain_code type mismatch");

    if (src->has_private_key) {
        memcpy(dst->private_key, src->private_key.bytes,
               sizeof(src->private_key.bytes));
        _Static_assert(
            sizeof(dst->private_key) == sizeof(src->private_key.bytes),
            "private_key type mismatch");
    } else {
        memzero(dst->private_key, sizeof(dst->private_key));
    }

    if (src->has_public_key) {
        memcpy(dst->public_key, src->public_key.bytes, sizeof(src->public_key));
        _Static_assert(sizeof(dst->public_key) == sizeof(src->public_key.bytes),
                       "public_key type mismatch");
    } else {
        memzero(dst->public_key, sizeof(dst->public_key));
    }
}

void storage_loadDevice(LoadDevice *msg) {
    storage_reset_shadow(&m_shadow_storage, m_sessionStorageKey);

    m_shadow_storage.pub.imported = true;

    storage_setPin(msg->has_pin ? msg->pin : "");

    m_shadow_storage.pub.no_backup = false;
    m_shadow_storage.pub.passphrase_protection =
        msg->has_passphrase_protection && msg->passphrase_protection;

    if (msg->has_node) {
        m_shadow_storage.pub.has_node = true;
        m_shadow_storage.pub.has_mnemonic = false;
        m_shadow_storage.has_sec = true;
        memcpy(&m_secret.node, &msg->node, sizeof(msg->node));
        m_sessionSeedCached = false;
        memzero(&m_sessionSeed, sizeof(m_sessionSeed));
    } else if (msg->has_mnemonic) {
        m_shadow_storage.pub.has_mnemonic = true;
        m_shadow_storage.pub.has_node = false;
        m_shadow_storage.has_sec = true;
        strlcpy(m_secret.mnemonic, msg->mnemonic, sizeof(m_secret.mnemonic));

        storage_compute_u2froot(m_secret.mnemonic,
                                &m_shadow_storage.pub.u2froot);
        m_shadow_storage.pub.has_u2froot = true;
        m_sessionSeedCached = false;
        memzero(&m_sessionSeed, sizeof(m_sessionSeed));
    }

    if (msg->has_language) {
        storage_setLanguage(msg->language);
    }

    if (msg->has_label) {
        storage_setLabel(msg->label);
    }

    if (msg->has_u2f_counter) {
        storage_setU2FCounter(msg->u2f_counter);
    }
}

void storage_setLabel(const char *label) {
    if (!label) {
        return;
    }

    m_shadow_storage.pub.has_label = true;
    memzero(m_shadow_storage.pub.label, sizeof(m_shadow_storage.pub.label));
    strlcpy(m_shadow_storage.pub.label, label,
            sizeof(m_shadow_storage.pub.label));
}

const char *storage_getLabel(void) {
    if (!m_shadow_storage.pub.has_label) {
        return NULL;
    }

    return m_shadow_storage.pub.label;
}

void storage_setLanguage(const char *lang) {
    if (!lang) {
        return;
    }

    // sanity check
    if (strcmp(lang, "english") == 0) {
        m_shadow_storage.pub.has_language = true;
        memzero(m_shadow_storage.pub.language,
                sizeof(m_shadow_storage.pub.language));
        strlcpy(m_shadow_storage.pub.language, lang,
                sizeof(m_shadow_storage.pub.language));
    } else if (strcmp(lang, "chinese") == 0) {
        m_shadow_storage.pub.has_language = true;
        memzero(m_shadow_storage.pub.language,
                sizeof(m_shadow_storage.pub.language));
        strlcpy(m_shadow_storage.pub.language, lang,
                sizeof(m_shadow_storage.pub.language));
    } else if (strcmp(lang, "japanese") == 0) {
        m_shadow_storage.pub.has_language = true;
        memzero(m_shadow_storage.pub.language,
                sizeof(m_shadow_storage.pub.language));
        strlcpy(m_shadow_storage.pub.language, lang,
                sizeof(m_shadow_storage.pub.language));
    } else if (strcmp(lang, "korean") == 0) {
        m_shadow_storage.pub.has_language = true;
        memzero(m_shadow_storage.pub.language,
                sizeof(m_shadow_storage.pub.language));
        strlcpy(m_shadow_storage.pub.language, lang,
                sizeof(m_shadow_storage.pub.language));
    }
}

const char *storage_getLanguage(void) {
    if (!m_shadow_storage.pub.has_language) {
        return NULL;
    }

    return m_shadow_storage.pub.language;
}

bool storage_isPinCorrect(const char *pin) {
    uint8_t storage_key[64];
    bool ret = storage_isPinCorrect_impl(
        pin, m_shadow_storage.pub.wrapped_storage_key,
        m_shadow_storage.pub.storage_key_fingerprint, storage_key);
    memzero(storage_key, 64);
    return ret;
}

bool storage_hasPin(void) { return m_shadow_storage.pub.has_pin; }

void storage_setPin(const char *pin) {
    storage_setPin_impl(&m_shadow_storage, pin, m_sessionStorageKey);

    m_sessionPinCached = true;
}

void storage_setPin_impl(Storage *storage, const char *pin,
                         uint8_t storage_key[64]) {
    // Derive the wrapping key for the new pin
    uint8_t wrapping_key[64];
    storage_deriveWrappingKey(pin, wrapping_key);

    // Derive a new storage_key.
    random_buffer(storage_key, 64);

    // Wrap the new storage_key.
    storage_wrapStorageKey(wrapping_key, storage_key,
                           storage->pub.wrapped_storage_key);

    // Fingerprint the storage_key.
    storage_keyFingerprint(storage_key, storage->pub.storage_key_fingerprint);

    // Clean up secrets to get them off the stack.
    memzero(wrapping_key, sizeof(wrapping_key));

    storage->pub.has_pin = !!strlen(pin);

    storage_secMigrate(storage, storage_key, /*encrypt=*/true);
}

void session_cachePin(const char *pin) {
    m_sessionPinCached = storage_isPinCorrect_impl(
        pin, m_shadow_storage.pub.wrapped_storage_key,
        m_shadow_storage.pub.storage_key_fingerprint, m_sessionStorageKey);

    if (!m_sessionPinCached) {
        memzero(m_sessionStorageKey, sizeof(m_sessionStorageKey));
        return;
    }

    storage_secMigrate(&m_shadow_storage, m_sessionStorageKey,
                       /*encrypt=*/false);
}

bool session_isPinCached(void) { return m_sessionPinCached; }

void storage_resetPinFails(void) {
    m_volatile_params.pin_failed_attempts = 0;
    volatile_params_write();
}

void storage_increasePinFails(void) {
    m_volatile_params.pin_failed_attempts++;
    volatile_params_write();
}

uint32_t storage_getPinFails(void) {
    return m_volatile_params.pin_failed_attempts;
}

/// \brief Calls animation callback.
/// \param iter Current iteration.
/// \param total Total iterations.
static void get_root_node_callback(uint32_t iter, uint32_t total) {
    (void)iter;
    (void)total;
    animating_progress_handler();
}

const uint8_t *storage_getSeed(const Storage *storage, bool usePassphrase) {
    // root node is properly cached
    if (usePassphrase == m_sessionSeedUsesPassphrase && m_sessionSeedCached) {
        return m_sessionSeed;
    }

    // if storage has mnemonic, convert it to node and use it
    if (storage->pub.has_mnemonic) {
        if (!storage->has_sec) {
            return NULL;
        }

        if (usePassphrase && !passphrase_protect()) {
            return NULL;
        }

        layout_loading();
        mnemonic_to_seed(m_secret.mnemonic,
                         usePassphrase ? m_sessionPassphrase : "",
                         m_sessionSeed,
                         get_root_node_callback);  // BIP-0039
        m_sessionSeedCached = true;
        m_sessionSeedUsesPassphrase = usePassphrase;
        return m_sessionSeed;
    }

    return NULL;
}

bool storage_getRootNode(const char *curve, bool usePassphrase, HDNode *node) {
    // if storage has node, decrypt and use it
    if (m_shadow_storage.pub.has_node && strcmp(curve, SECP256K1_NAME) == 0) {
        if (!m_shadow_storage.has_sec) {
            return false;
        }

        if (!passphrase_protect()) {
            /* passphrased failed. Bailing */
            return false;
        }

        if (hdnode_from_xprv(m_secret.node.depth, m_secret.node.child_num,
                             m_secret.node.chain_code.bytes,
                             m_secret.node.private_key.bytes, curve,
                             node) == 0) {
            return false;
        }

        if (m_shadow_storage.pub.passphrase_protection &&
            m_sessionPassphraseCached && strlen(m_sessionPassphrase) > 0) {
            // decrypt hd node
            static uint8_t CONFIDENTIAL secret[64];
            PBKDF2_HMAC_SHA512_CTX pctx;
            pbkdf2_hmac_sha512_Init(&pctx, (const uint8_t *)m_sessionPassphrase,
                                    strlen(m_sessionPassphrase),
                                    (const uint8_t *)"SOTERWHD", 8, 1);
            for (int i = 0; i < 8; i++) {
                pbkdf2_hmac_sha512_Update(&pctx, BIP39_PBKDF2_ROUNDS / 8);
                get_root_node_callback((i + 1) * BIP39_PBKDF2_ROUNDS / 8,
                                       BIP39_PBKDF2_ROUNDS);
            }
            pbkdf2_hmac_sha512_Final(&pctx, secret);
            aes_decrypt_ctx ctx;
            aes_decrypt_key256(secret, &ctx);
            aes_cbc_decrypt(node->chain_code, node->chain_code, 32, secret + 32,
                            &ctx);
            aes_cbc_decrypt(node->private_key, node->private_key, 32,
                            secret + 32, &ctx);
            memzero(&ctx, sizeof(ctx));
            memzero(secret, sizeof(secret));
        }

        return true;
    }

    /* get node from mnemonic */
    if (m_shadow_storage.pub.has_mnemonic) {
        if (!m_shadow_storage.has_sec) {
            return false;
        }

        if (!passphrase_protect()) {
            /* passphrased failed. Bailing */
            return false;
        }

        if (!m_sessionSeedCached) {
            m_sessionSeedCached = storage_getRootSeedCache(
                &m_shadow_storage, curve, usePassphrase, m_sessionSeed);

            if (!m_sessionSeedCached) {
                /* calculate session seed and update the global
                 * sessionSeed/sessionSeedCached variables */
                storage_getSeed(&m_shadow_storage, usePassphrase);

                if (!m_sessionSeedCached) {
                    return false;
                }

                storage_setRootSeedCache(&m_shadow_storage, m_sessionSeed,
                                         curve);
            }
        }

        if (hdnode_from_seed(m_sessionSeed, 64, curve, node) == 1) {
            return true;
        }
    }

    return false;
}

bool storage_isInitialized(void) {
    return m_shadow_storage.pub.has_node || m_shadow_storage.pub.has_mnemonic;
}

const char *storage_getUuidStr(void) { return m_shadow_storage.uuid_str; }

bool storage_getPassphraseProtected(void) {
    return m_shadow_storage.pub.passphrase_protection;
}

void storage_setPassphraseProtected(bool passphrase) {
    m_shadow_storage.pub.passphrase_protection = passphrase;
}

void session_cachePassphrase(const char *passphrase) {
    strlcpy(m_sessionPassphrase, passphrase, sizeof(m_sessionPassphrase));
    m_sessionPassphraseCached = true;
}

bool session_isPassphraseCached(void) { return m_sessionPassphraseCached; }

void storage_setMnemonicFromWords(const char (*words)[12],
                                  unsigned int word_count) {
    strlcpy(m_secret.mnemonic, words[0], sizeof(m_secret.mnemonic));

    for (uint32_t i = 1; i < word_count; i++) {
        strlcat(m_secret.mnemonic, " ", sizeof(m_secret.mnemonic));
        strlcat(m_secret.mnemonic, words[i], sizeof(m_secret.mnemonic));
    }

    m_shadow_storage.pub.has_mnemonic = true;
    m_shadow_storage.has_sec = true;
}

void storage_setMnemonic(const char *m) {
    memzero(m_secret.mnemonic, sizeof(m_secret.mnemonic));
    strlcpy(m_secret.mnemonic, m, sizeof(m_secret.mnemonic));

    m_shadow_storage.pub.has_mnemonic = true;
    m_shadow_storage.has_sec = true;
}

bool storage_hasMnemonic(void) { return m_shadow_storage.pub.has_mnemonic; }

const char *storage_getShadowMnemonic(void) {
    if (!m_shadow_storage.has_sec) return NULL;
    return m_secret.mnemonic;
}

bool storage_getImported(void) { return m_shadow_storage.pub.imported; }

bool storage_hasNode(void) { return m_shadow_storage.pub.has_node; }

bool storage_setPolicy(const char *policy_name, bool enabled) {
    return storage_setPolicy_impl(m_shadow_storage.pub.policies, policy_name,
                                  enabled);
}

bool storage_setPolicy_impl(PolicyType ps[POLICY_COUNT],
                            const char *policy_name, bool enabled) {
    for (unsigned i = 0; i < POLICY_COUNT; ++i) {
        if (strcmp(policy_name, ps[i].policy_name) == 0) {
            ps[i].has_enabled = true;
            ps[i].enabled = enabled;
            return true;
        }
    }

    return false;
}

void storage_getPolicies(PolicyType *policy_data) {
    for (size_t i = 0; i < POLICY_COUNT; ++i) {
        memcpy(&policy_data[i], &m_shadow_storage.pub.policies[i],
               sizeof(policy_data[i]));
    }
}

bool storage_isPolicyEnabled(const char *policy_name) {
    return storage_isPolicyEnabled_impl(m_shadow_storage.pub.policies,
                                        policy_name);
}

bool storage_isPolicyEnabled_impl(const PolicyType ps[POLICY_COUNT],
                                  const char *policy_name) {
    for (unsigned i = 0; i < POLICY_COUNT; ++i) {
        if (strcmp(policy_name, ps[i].policy_name) == 0) {
            return ps[i].enabled;
        }
    }
    return false;
}

bool storage_noBackup(void) { return m_shadow_storage.pub.no_backup; }

void storage_setNoBackup(void) { m_shadow_storage.pub.no_backup = true; }

uint32_t storage_getAutoLockDelayMs() {
    return m_shadow_storage.pub.has_auto_lock_delay_ms
               ? MAX(m_shadow_storage.pub.auto_lock_delay_ms,
                     STORAGE_MIN_SCREENSAVER_TIMEOUT)
               : STORAGE_DEFAULT_SCREENSAVER_TIMEOUT;
}

void storage_setAutoLockDelayMs(uint32_t auto_lock_delay_ms) {
    m_shadow_storage.pub.has_auto_lock_delay_ms = true;
    m_shadow_storage.pub.auto_lock_delay_ms =
        MAX(auto_lock_delay_ms, STORAGE_MIN_SCREENSAVER_TIMEOUT);
}
