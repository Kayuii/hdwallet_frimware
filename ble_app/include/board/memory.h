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

#ifndef MEMORY_H
#define MEMORY_H

#include "trezor-crypto/sha2.h"
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>

/*

 flash memory layout:
 --------------------
   name    |          range          |  size   |     function          | permissions
-----------+-------------------------+---------+-----------------------+-------------
 Sector  0 | 0x00000000 - 0x00025FFF |  152 KiB| MBR + SoftDevice      | Read
 Sector  1 | 0x00026000 - 0x000E3FFF |  760 KiB| Application           | Read
 Sector  2 | 0x000E4000 - 0x000EFFFF |  48 KiB | FDS                   | Read  Write
 Sector  3 | 0x000F0000 - 0x000FDFFF |  56 KiB | Bootloader            | Read
 Sector  4 | 0x000FE000 - 0x000FEFFF |  4  KiB | MBR Settings          | Read
 Sector  5 | 0x000FF000 - 0x000FFFFF |  4  KiB | Bootloader Settings   | Read
*/

#define FLASH_PTR(x)		(const uint8_t*) (x)

/* Flash Info */
#define FLASH_ORIGIN             (0x00000000)
#define FLASH_TOTAL_SIZE         (1024 * 1024)
#define FLASH_END                (FLASH_ORIGIN + FLASH_TOTAL_SIZE)

/* MBR + SoftDevice Partition */
#define FLASH_MBR_START          (FLASH_ORIGIN)     //0x0000_0000 - 0x0002_5FFF
#define FLASH_MBR_LEN            (0x26000)

/* Application Partition */
#define FLASH_APP_START         (FLASH_MBR_START + FLASH_MBR_LEN)  // 0x0002_6000
#define FLASH_APP_LEN           (0xC5000)

/* FDS Partition */
#define FLASH_FDS_START         (FLASH_APP_START + FLASH_APP_LEN)  // 0x000E_4000
#define FLASH_FDS_LEN           (0x0C000)

/* Boot Loader Partition */
#define FLASH_BTLDR_START        (FLASH_FDS_START + FLASH_FDS_LEN)  //0x000F_0000
#define FLASH_BTLDR_LEN          (0x07000)

/* MBR Settings Partition */
#define FLASH_MBR_SETTINGS_START        (FLASH_BTLDR_START + FLASH_BTLDR_LEN)  //0x000F_E000
#define FLASH_MBR_SETTINGS_LEN          (0x01000)

/* Boot Loader Settings Partition */
#define FLASH_BTLDR_SETTINGS_START        (FLASH_MBR_SETTINGS_START + FLASH_MBR_SETTINGS_LEN)  //0x000F_F000
#define FLASH_BTLDR_SETTINGS_LEN          (0x01000)

/// Double sha256 hash of the bootloader.
///
/// \param hash    Buffer to be filled with hash.
///                Must be at least SHA256_DIGEST_LENGTH bytes long.
/// \param cached  Whether a cached value is acceptable.
int memory_bootloader_hash(uint8_t *hash, bool cached);

int memory_firmware_hash(uint8_t *hash);
const char *memory_firmware_hash_str(char digest[SHA256_DIGEST_STRING_LENGTH]);

#endif
