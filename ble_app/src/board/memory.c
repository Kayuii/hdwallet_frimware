/*
 * This file is part of the KEEPKEY project.
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

#include "trezor-crypto/sha2.h"

#include "board/board.h"
#include "board/memory.h"
#include "firmware/storage.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

int memory_bootloader_hash(uint8_t *hash, bool cached) {
    static uint8_t cached_hash[SHA256_DIGEST_LENGTH];

    if (cached_hash[0] == '\0' || !cached) {
        sha256_Raw((const uint8_t *)FLASH_BTLDR_START, FLASH_BTLDR_LEN,
                   cached_hash);
        sha256_Raw(cached_hash, SHA256_DIGEST_LENGTH, cached_hash);
    }

    memcpy(hash, cached_hash, SHA256_DIGEST_LENGTH);

    return SHA256_DIGEST_LENGTH;
}

/*
 * memory_firmware_hash() - SHA256 hash of firmware (meta and application)
 *
 * INPUT
 *     - hash: buffer to be filled with hash
 * OUTPUT
 *     none
 */
int memory_firmware_hash(uint8_t *hash) {
    SHA256_CTX ctx;
    sha256_Init(&ctx);
    sha256_Update(&ctx, (const uint8_t *)FLASH_APP_START, FLASH_APP_LEN);
    sha256_Final(&ctx, hash);
    return SHA256_DIGEST_LENGTH;
}

const char *memory_firmware_hash_str(char digest[SHA256_DIGEST_STRING_LENGTH]) {
    SHA256_CTX ctx;
    sha256_Init(&ctx);
    sha256_Update(&ctx, (const uint8_t *)FLASH_APP_START, FLASH_APP_LEN);
    sha256_End(&ctx, digest);
    return &digest[0];
}
