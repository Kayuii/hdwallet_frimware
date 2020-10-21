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

#ifndef __BOARD_H__
#define __BOARD_H__

/* === Includes ============================================================ */

#include "driver/button.h"
#include "board/canvas.h"
#include "board/layout.h"
#include "trezor-crypto/curves.h"
#include "trezor-crypto/bip32.h"
#include "trezor-crypto/curves.h"
#include "driver/usb.h"
#include "driver/rng.h"
#include "crc32.h"

/*
 storage layout:

 offset | type/length |  description
--------+-------------+-------------------------------
 0x0000 |  4 bytes    |  magic = 'stor'
 0x0004 |  6 bytes    |  uuid
 0x0010 |  13 bytes   |  uuid_str
 0x0029 |  ?          |  Storage structure
 */

#define MAJOR_VERSION 0
#define MINOR_VERSION 0
#define PATCH_VERSION 1

#define STORAGE_SECTOR_LEN  0x00004000

#define CACHE_EXISTS        0xCA

/* Specify the length of the uuid binary string */
#define STORAGE_UUID_LEN    12

/* Length of the uuid binary converted to readable ASCII.  */
#define STORAGE_UUID_STR_LEN ((STORAGE_UUID_LEN * 2) + 1)

#define SMALL_STR_BUF       32
#define MEDIUM_STR_BUF      64
#define LARGE_STR_BUF       128

#define VERSION_NUM(x) #x
#define VERSION_STR(x) VERSION_NUM(x)

/* === Defines ============================================================= */

#define ONE_SEC 1100    /* Count for 1 second  */
#define HALF_SEC 500    /* Count for 0.5 second */
#define TIME_TASK_DELAY (220)
/* === Typedefs ============================================================ */

#endif
