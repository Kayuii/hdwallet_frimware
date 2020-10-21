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
#include "board/font.h"
#include <stddef.h>
#include "driver/gt24l24a2y_app.h"
#include "firmware/storage.h"
#include "firmware/utf8.h"

#define HEIGHT_BYTES(h) (((h) + 7) / 8)
#define FONT_IMAGE_DATA_LEN(p_font) \
    (((p_font)->width) * HEIGHT_BYTES((p_font)->height))

//decode font width for data from hardware font chip
#define FONT_IMG_WITH(data) (uint16_t)(((0x00ff & data[0]) << 8) + data [1])

/************pin_font**********/
static uint8_t image_data_pin_font_0x31[8] = {0x60, 0x00, 0xe0, 0x00,
                                              0xff, 0xf0, 0xff, 0xf0};
static const CharacterImage pin_font_0x31 = {image_data_pin_font_0x31, 4, 12};

static uint8_t image_data_pin_font_0x32[16] = {
    0xc1, 0xf0, 0xc3, 0xf0, 0xc6, 0x30, 0xc6, 0x30,
    0xc6, 0x30, 0xc6, 0x30, 0x7c, 0x30, 0x38, 0x30};
static const CharacterImage pin_font_0x32 = {image_data_pin_font_0x32, 8, 12};

static uint8_t image_data_pin_font_0x33[16] = {
    0xc0, 0x30, 0xc6, 0x30, 0xc6, 0x30, 0xc6, 0x30,
    0xc6, 0x30, 0xc6, 0x30, 0x7f, 0xe0, 0x39, 0xc0};
static const CharacterImage pin_font_0x33 = {image_data_pin_font_0x33, 8, 12};

static uint8_t image_data_pin_font_0x34[16] = {
    0x03, 0x80, 0x0f, 0x80, 0x3d, 0x80, 0xf1, 0x80,
    0xc1, 0x80, 0xff, 0xf0, 0xff, 0xf0, 0x01, 0x80};
static const CharacterImage pin_font_0x34 = {image_data_pin_font_0x34, 8, 12};

static uint8_t image_data_pin_font_0x35[16] = {
    0xfc, 0x20, 0xfe, 0x30, 0xc6, 0x30, 0xc6, 0x30,
    0xc6, 0x30, 0xc6, 0x30, 0xc3, 0xe0, 0xc1, 0xc0};
static const CharacterImage pin_font_0x35 = {image_data_pin_font_0x35, 8, 12};

static uint8_t image_data_pin_font_0x36[16] = {
    0x3f, 0xc0, 0x7f, 0xe0, 0xc6, 0x30, 0xc6, 0x30,
    0xc6, 0x30, 0xc6, 0x30, 0x43, 0xe0, 0x01, 0xc0};
static const CharacterImage pin_font_0x36 = {image_data_pin_font_0x36, 8, 12};

static uint8_t image_data_pin_font_0x37[16] = {
    0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x30, 0xc0, 0xf0,
    0xc3, 0xc0, 0xcf, 0x00, 0xfc, 0x00, 0xf0, 0x00};
static const CharacterImage pin_font_0x37 = {image_data_pin_font_0x37, 8, 12};

static uint8_t image_data_pin_font_0x38[16] = {
    0x39, 0xc0, 0x7f, 0xe0, 0xc6, 0x30, 0xc6, 0x30,
    0xc6, 0x30, 0xc6, 0x30, 0x7f, 0xe0, 0x39, 0xc0};
static const CharacterImage pin_font_0x38 = {image_data_pin_font_0x38, 8, 12};

static uint8_t image_data_pin_font_0x39[16] = {
    0x38, 0x00, 0x7c, 0x20, 0xc6, 0x30, 0xc6, 0x30,
    0xc6, 0x30, 0xc6, 0x30, 0x7f, 0xe0, 0x3f, 0xc0};
static const CharacterImage pin_font_0x39 = {image_data_pin_font_0x39, 8, 12};

static const Character pin_font_array[] = {

    /* Character: '1' */
    {0x31, &pin_font_0x31},

    /* Character: '2' */
    {0x32, &pin_font_0x32},

    /* Character: '3' */
    {0x33, &pin_font_0x33},

    /* Character: '4' */
    {0x34, &pin_font_0x34},

    /* Character: '5' */
    {0x35, &pin_font_0x35},

    /* Character: '6' */
    {0x36, &pin_font_0x36},

    /* Character: '7' */
    {0x37, &pin_font_0x37},

    /* Character: '8' */
    {0x38, &pin_font_0x38},

    /* Character: '9' */
    {0x39, &pin_font_0x39}
};

static SoftFontCharacters sw_pin_font = {
    sizeof(pin_font_array) / sizeof(pin_font_array[0]), 14, pin_font_array};

/************pin_font**********/

/************body_font**********/
static uint8_t image_data_body_font_0x20[8] = {0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x20 = {image_data_body_font_0x20, 4, 10};

static uint8_t image_data_body_font_0x21[4] = {0x7d, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x21 = {image_data_body_font_0x21, 2, 10};

static uint8_t image_data_body_font_0x22[8] = {0x60, 0x00, 0x00, 0x00,
                                               0x60, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x22 = {image_data_body_font_0x22, 4, 10};

static uint8_t image_data_body_font_0x23[14] = {0x24, 0x00, 0x7e, 0x00, 0x24,
                                                0x00, 0x24, 0x00, 0x7e, 0x00,
                                                0x24, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x23 = {image_data_body_font_0x23, 7, 10};

static uint8_t image_data_body_font_0x24[12] = {
    0x31, 0x00, 0x49, 0x00, 0xff, 0x80, 0x49, 0x00, 0x46, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x24 = {image_data_body_font_0x24, 6, 10};

static uint8_t image_data_body_font_0x25[16] = {
    0x21, 0x00, 0x52, 0x00, 0x24, 0x00, 0x08, 0x00,
    0x12, 0x00, 0x25, 0x00, 0x42, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x25 = {image_data_body_font_0x25, 8, 10};

static uint8_t image_data_body_font_0x26[14] = {0x36, 0x00, 0x49, 0x00, 0x49,
                                                0x00, 0x35, 0x00, 0x02, 0x00,
                                                0x05, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x26 = {image_data_body_font_0x26, 7, 10};

static uint8_t image_data_body_font_0x27[4] = {0x60, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x27 = {image_data_body_font_0x27, 2, 10};

static uint8_t image_data_body_font_0x28[6] = {0x7f, 0x00, 0x80,
                                               0x80, 0x00, 0x00};
static const CharacterImage body_font_0x28 = {image_data_body_font_0x28, 3, 10};

static uint8_t image_data_body_font_0x29[6] = {0x80, 0x80, 0x7f,
                                               0x00, 0x00, 0x00};
static const CharacterImage body_font_0x29 = {image_data_body_font_0x29, 3, 10};

static uint8_t image_data_body_font_0x2a[12] = {
    0x50, 0x00, 0x20, 0x00, 0xf8, 0x00, 0x20, 0x00, 0x50, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x2a = {image_data_body_font_0x2a, 6, 10};

static uint8_t image_data_body_font_0x2b[12] = {
    0x08, 0x00, 0x08, 0x00, 0x3e, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x2b = {image_data_body_font_0x2b, 6, 10};

static uint8_t image_data_body_font_0x2c[6] = {0x03, 0x40, 0x03,
                                               0x80, 0x00, 0x00};
static const CharacterImage body_font_0x2c = {image_data_body_font_0x2c, 3, 10};

static uint8_t image_data_body_font_0x2d[12] = {
    0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x2d = {image_data_body_font_0x2d, 6, 10};

static uint8_t image_data_body_font_0x2e[6] = {0x03, 0x00, 0x03,
                                               0x00, 0x00, 0x00};
static const CharacterImage body_font_0x2e = {image_data_body_font_0x2e, 3, 10};

static uint8_t image_data_body_font_0x2f[16] = {
    0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x08, 0x00,
    0x10, 0x00, 0x20, 0x00, 0x40, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x2f = {image_data_body_font_0x2f, 8, 10};

static uint8_t image_data_body_font_0x30[12] = {
    0x3e, 0x00, 0x45, 0x00, 0x49, 0x00, 0x51, 0x00, 0x3e, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x30 = {image_data_body_font_0x30, 6, 10};

static uint8_t image_data_body_font_0x31[6] = {0x40, 0x00, 0x7f,
                                               0x00, 0x00, 0x00};
static const CharacterImage body_font_0x31 = {image_data_body_font_0x31, 3, 10};

static uint8_t image_data_body_font_0x32[12] = {
    0x47, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x31, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x32 = {image_data_body_font_0x32, 6, 10};

static uint8_t image_data_body_font_0x33[12] = {
    0x41, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x36, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x33 = {image_data_body_font_0x33, 6, 10};

static uint8_t image_data_body_font_0x34[12] = {
    0x0c, 0x00, 0x14, 0x00, 0x24, 0x00, 0x7f, 0x00, 0x04, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x34 = {image_data_body_font_0x34, 6, 10};

static uint8_t image_data_body_font_0x35[12] = {
    0x79, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x46, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x35 = {image_data_body_font_0x35, 6, 10};

static uint8_t image_data_body_font_0x36[12] = {
    0x3e, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x06, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x36 = {image_data_body_font_0x36, 6, 10};

static uint8_t image_data_body_font_0x37[12] = {
    0x40, 0x00, 0x41, 0x00, 0x46, 0x00, 0x58, 0x00, 0x60, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x37 = {image_data_body_font_0x37, 6, 10};

static uint8_t image_data_body_font_0x38[12] = {
    0x36, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x36, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x38 = {image_data_body_font_0x38, 6, 10};

static uint8_t image_data_body_font_0x39[12] = {
    0x30, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x3e, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x39 = {image_data_body_font_0x39, 6, 10};

static uint8_t image_data_body_font_0x3a[6] = {0x1b, 0x00, 0x1b,
                                               0x00, 0x00, 0x00};
static const CharacterImage body_font_0x3a = {image_data_body_font_0x3a, 3, 10};

static uint8_t image_data_body_font_0x3b[6] = {0x1b, 0x40, 0x1b,
                                               0x80, 0x00, 0x00};
static const CharacterImage body_font_0x3b = {image_data_body_font_0x3b, 3, 10};

static uint8_t image_data_body_font_0x3c[10] = {0x08, 0x00, 0x14, 0x00, 0x22,
                                                0x00, 0x41, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x3c = {image_data_body_font_0x3c, 5, 10};

static uint8_t image_data_body_font_0x3d[12] = {
    0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x3d = {image_data_body_font_0x3d, 6, 10};

static uint8_t image_data_body_font_0x3e[10] = {0x41, 0x00, 0x22, 0x00, 0x14,
                                                0x00, 0x08, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x3e = {image_data_body_font_0x3e, 5, 10};

static uint8_t image_data_body_font_0x3f[12] = {
    0x20, 0x00, 0x40, 0x00, 0x45, 0x00, 0x48, 0x00, 0x30, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x3f = {image_data_body_font_0x3f, 6, 10};

static uint8_t image_data_body_font_0x40[16] = {
    0x3e, 0x00, 0x41, 0x00, 0x84, 0x80, 0xaa, 0x80,
    0xaa, 0x80, 0x9e, 0x80, 0x42, 0x00, 0x3c, 0x00};
static const CharacterImage body_font_0x40 = {image_data_body_font_0x40, 8, 10};

static uint8_t image_data_body_font_0x41[12] = {
    0x3f, 0x00, 0x48, 0x00, 0x48, 0x00, 0x48, 0x00, 0x3f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x41 = {image_data_body_font_0x41, 6, 10};

static uint8_t image_data_body_font_0x42[12] = {
    0x7f, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x36, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x42 = {image_data_body_font_0x42, 6, 10};

static uint8_t image_data_body_font_0x43[12] = {
    0x3e, 0x00, 0x41, 0x00, 0x41, 0x00, 0x41, 0x00, 0x22, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x43 = {image_data_body_font_0x43, 6, 10};

static uint8_t image_data_body_font_0x44[12] = {
    0x7f, 0x00, 0x41, 0x00, 0x41, 0x00, 0x41, 0x00, 0x3e, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x44 = {image_data_body_font_0x44, 6, 10};

static uint8_t image_data_body_font_0x45[12] = {
    0x7f, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x41, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x45 = {image_data_body_font_0x45, 6, 10};

static uint8_t image_data_body_font_0x46[12] = {
    0x7f, 0x00, 0x48, 0x00, 0x48, 0x00, 0x48, 0x00, 0x40, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x46 = {image_data_body_font_0x46, 6, 10};

static uint8_t image_data_body_font_0x47[12] = {
    0x3e, 0x00, 0x41, 0x00, 0x49, 0x00, 0x49, 0x00, 0x2f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x47 = {image_data_body_font_0x47, 6, 10};

static uint8_t image_data_body_font_0x48[12] = {
    0x7f, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x7f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x48 = {image_data_body_font_0x48, 6, 10};

static uint8_t image_data_body_font_0x49[8] = {0x41, 0x00, 0x7f, 0x00,
                                               0x41, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x49 = {image_data_body_font_0x49, 4, 10};

static uint8_t image_data_body_font_0x4a[12] = {
    0x02, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7e, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x4a = {image_data_body_font_0x4a, 6, 10};

static uint8_t image_data_body_font_0x4b[12] = {
    0x7f, 0x00, 0x08, 0x00, 0x14, 0x00, 0x22, 0x00, 0x41, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x4b = {image_data_body_font_0x4b, 6, 10};

static uint8_t image_data_body_font_0x4c[12] = {
    0x7f, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x4c = {image_data_body_font_0x4c, 6, 10};

static uint8_t image_data_body_font_0x4d[16] = {
    0x7f, 0x00, 0x20, 0x00, 0x10, 0x00, 0x08, 0x00,
    0x10, 0x00, 0x20, 0x00, 0x7f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x4d = {image_data_body_font_0x4d, 8, 10};

static uint8_t image_data_body_font_0x4e[12] = {
    0x7f, 0x00, 0x10, 0x00, 0x08, 0x00, 0x04, 0x00, 0x7f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x4e = {image_data_body_font_0x4e, 6, 10};

static uint8_t image_data_body_font_0x4f[12] = {
    0x3e, 0x00, 0x41, 0x00, 0x41, 0x00, 0x41, 0x00, 0x3e, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x4f = {image_data_body_font_0x4f, 6, 10};

static uint8_t image_data_body_font_0x50[12] = {
    0x7f, 0x00, 0x44, 0x00, 0x44, 0x00, 0x44, 0x00, 0x38, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x50 = {image_data_body_font_0x50, 6, 10};

static uint8_t image_data_body_font_0x51[12] = {
    0x3e, 0x00, 0x41, 0x00, 0x41, 0x00, 0x41, 0x00, 0x3e, 0x80, 0x00, 0x00};
static const CharacterImage body_font_0x51 = {image_data_body_font_0x51, 6, 10};

static uint8_t image_data_body_font_0x52[12] = {
    0x7f, 0x00, 0x44, 0x00, 0x44, 0x00, 0x46, 0x00, 0x39, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x52 = {image_data_body_font_0x52, 6, 10};

static uint8_t image_data_body_font_0x53[12] = {
    0x32, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x26, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x53 = {image_data_body_font_0x53, 6, 10};

static uint8_t image_data_body_font_0x54[12] = {
    0x40, 0x00, 0x40, 0x00, 0x7f, 0x00, 0x40, 0x00, 0x40, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x54 = {image_data_body_font_0x54, 6, 10};

static uint8_t image_data_body_font_0x55[12] = {
    0x7e, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7e, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x55 = {image_data_body_font_0x55, 6, 10};

static uint8_t image_data_body_font_0x56[12] = {
    0x78, 0x00, 0x06, 0x00, 0x01, 0x00, 0x06, 0x00, 0x78, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x56 = {image_data_body_font_0x56, 6, 10};

static uint8_t image_data_body_font_0x57[16] = {
    0x7e, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7e, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x7e, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x57 = {image_data_body_font_0x57, 8, 10};

static uint8_t image_data_body_font_0x58[12] = {
    0x63, 0x00, 0x14, 0x00, 0x08, 0x00, 0x14, 0x00, 0x63, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x58 = {image_data_body_font_0x58, 6, 10};

static uint8_t image_data_body_font_0x59[12] = {
    0x70, 0x00, 0x08, 0x00, 0x07, 0x00, 0x08, 0x00, 0x70, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x59 = {image_data_body_font_0x59, 6, 10};

static uint8_t image_data_body_font_0x5a[12] = {
    0x43, 0x00, 0x45, 0x00, 0x49, 0x00, 0x51, 0x00, 0x61, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x5a = {image_data_body_font_0x5a, 6, 10};

static uint8_t image_data_body_font_0x5b[8] = {0x7f, 0x00, 0x41, 0x00,
                                               0x41, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x5b = {image_data_body_font_0x5b, 4, 10};

static uint8_t image_data_body_font_0x5c[16] = {
    0x40, 0x00, 0x20, 0x00, 0x10, 0x00, 0x08, 0x00,
    0x04, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x5c = {image_data_body_font_0x5c, 8, 10};

static uint8_t image_data_body_font_0x5d[8] = {0x41, 0x00, 0x41, 0x00,
                                               0x7f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x5d = {image_data_body_font_0x5d, 4, 10};

static uint8_t image_data_body_font_0x5e[8] = {0x20, 0x00, 0x40, 0x00,
                                               0x20, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x5e = {image_data_body_font_0x5e, 4, 10};

static uint8_t image_data_body_font_0x5f[12] = {
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x5f = {image_data_body_font_0x5f, 6, 10};

static uint8_t image_data_body_font_0x60[6] = {0x40, 0x00, 0x20,
                                               0x00, 0x00, 0x00};
static const CharacterImage body_font_0x60 = {image_data_body_font_0x60, 3, 10};

static uint8_t image_data_body_font_0x61[12] = {
    0x02, 0x00, 0x15, 0x00, 0x15, 0x00, 0x15, 0x00, 0x0f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x61 = {image_data_body_font_0x61, 6, 10};

static uint8_t image_data_body_font_0x62[12] = {
    0x7f, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x0e, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x62 = {image_data_body_font_0x62, 6, 10};

static uint8_t image_data_body_font_0x63[12] = {
    0x0e, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x63 = {image_data_body_font_0x63, 6, 10};

static uint8_t image_data_body_font_0x64[12] = {
    0x0e, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x7f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x64 = {image_data_body_font_0x64, 6, 10};

static uint8_t image_data_body_font_0x65[12] = {
    0x0e, 0x00, 0x15, 0x00, 0x15, 0x00, 0x15, 0x00, 0x0d, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x65 = {image_data_body_font_0x65, 6, 10};

static uint8_t image_data_body_font_0x66[10] = {0x10, 0x00, 0x3f, 0x00, 0x50,
                                                0x00, 0x50, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x66 = {image_data_body_font_0x66, 5, 10};

static uint8_t image_data_body_font_0x67[12] = {
    0x0e, 0x00, 0x11, 0x40, 0x11, 0x40, 0x11, 0x40, 0x1f, 0x80, 0x00, 0x00};
static const CharacterImage body_font_0x67 = {image_data_body_font_0x67, 6, 10};

static uint8_t image_data_body_font_0x68[12] = {
    0x7f, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x0f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x68 = {image_data_body_font_0x68, 6, 10};

static uint8_t image_data_body_font_0x69[4] = {0x5f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x69 = {image_data_body_font_0x69, 2, 10};

static uint8_t image_data_body_font_0x6a[6] = {0x00, 0x40, 0x5f,
                                               0x80, 0x00, 0x00};
static const CharacterImage body_font_0x6a = {image_data_body_font_0x6a, 3, 10};

static uint8_t image_data_body_font_0x6b[10] = {0x7f, 0x00, 0x04, 0x00, 0x0a,
                                                0x00, 0x11, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x6b = {image_data_body_font_0x6b, 5, 10};

static uint8_t image_data_body_font_0x6c[4] = {0x7f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x6c = {image_data_body_font_0x6c, 2, 10};

static uint8_t image_data_body_font_0x6d[16] = {
    0x1f, 0x00, 0x10, 0x00, 0x10, 0x00, 0x1f, 0x00,
    0x10, 0x00, 0x10, 0x00, 0x0f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x6d = {image_data_body_font_0x6d, 8, 10};

static uint8_t image_data_body_font_0x6e[12] = {
    0x1f, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x0f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x6e = {image_data_body_font_0x6e, 6, 10};

static uint8_t image_data_body_font_0x6f[12] = {
    0x0e, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x0e, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x6f = {image_data_body_font_0x6f, 6, 10};

static uint8_t image_data_body_font_0x70[12] = {
    0x1f, 0xc0, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x0e, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x70 = {image_data_body_font_0x70, 6, 10};

static uint8_t image_data_body_font_0x71[12] = {
    0x0e, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x1f, 0xc0, 0x00, 0x00};
static const CharacterImage body_font_0x71 = {image_data_body_font_0x71, 6, 10};

static uint8_t image_data_body_font_0x72[10] = {0x1f, 0x00, 0x08, 0x00, 0x10,
                                                0x00, 0x10, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x72 = {image_data_body_font_0x72, 5, 10};

static uint8_t image_data_body_font_0x73[12] = {
    0x09, 0x00, 0x15, 0x00, 0x15, 0x00, 0x15, 0x00, 0x12, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x73 = {image_data_body_font_0x73, 6, 10};

static uint8_t image_data_body_font_0x74[10] = {0x10, 0x00, 0x7e, 0x00, 0x11,
                                                0x00, 0x11, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x74 = {image_data_body_font_0x74, 5, 10};

static uint8_t image_data_body_font_0x75[12] = {
    0x1e, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x1f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x75 = {image_data_body_font_0x75, 6, 10};

static uint8_t image_data_body_font_0x76[12] = {
    0x18, 0x00, 0x06, 0x00, 0x01, 0x00, 0x06, 0x00, 0x18, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x76 = {image_data_body_font_0x76, 6, 10};

static uint8_t image_data_body_font_0x77[16] = {
    0x1e, 0x00, 0x01, 0x00, 0x01, 0x00, 0x1e, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x1e, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x77 = {image_data_body_font_0x77, 8, 10};

static uint8_t image_data_body_font_0x78[12] = {
    0x11, 0x00, 0x0a, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x11, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x78 = {image_data_body_font_0x78, 6, 10};

static uint8_t image_data_body_font_0x79[12] = {
    0x1e, 0x00, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40, 0x1f, 0x80, 0x00, 0x00};
static const CharacterImage body_font_0x79 = {image_data_body_font_0x79, 6, 10};

static uint8_t image_data_body_font_0x7a[12] = {
    0x11, 0x00, 0x13, 0x00, 0x15, 0x00, 0x19, 0x00, 0x11, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x7a = {image_data_body_font_0x7a, 6, 10};

static uint8_t image_data_body_font_0x7b[10] = {0x08, 0x00, 0x36, 0x00, 0x41,
                                                0x00, 0x41, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x7b = {image_data_body_font_0x7b, 5, 10};

static uint8_t image_data_body_font_0x7c[4] = {0x7f, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x7c = {image_data_body_font_0x7c, 2, 10};

static uint8_t image_data_body_font_0x7d[10] = {0x41, 0x00, 0x41, 0x00, 0x36,
                                                0x00, 0x08, 0x00, 0x00, 0x00};
static const CharacterImage body_font_0x7d = {image_data_body_font_0x7d, 5, 10};

static uint8_t image_data_body_font_0x7e[14] = {0x00, 0x00, 0x04, 0x00, 0x02,
                                                0x00, 0x01, 0x00, 0x06, 0x00,
                                                0x18, 0x00, 0x60, 0x00};
static const CharacterImage body_font_0x7e = {image_data_body_font_0x7e, 7, 10};

static const Character body_font_array[] = {

    /* Character: ' ' */
    {0x20, &body_font_0x20},

    /* Character: '!' */
    {0x21, &body_font_0x21},

    /* Character: '"' */
    {0x22, &body_font_0x22},

    /* Character: '#' */
    {0x23, &body_font_0x23},

    /* Character: '$' */
    {0x24, &body_font_0x24},

    /* Character: '%' */
    {0x25, &body_font_0x25},

    /* Character: '&' */
    {0x26, &body_font_0x26},

    /* Character: ''' */
    {0x27, &body_font_0x27},

    /* Character: '(' */
    {0x28, &body_font_0x28},

    /* Character: ')' */
    {0x29, &body_font_0x29},

    /* Character: '*' */
    {0x2a, &body_font_0x2a},

    /* Character: '+' */
    {0x2b, &body_font_0x2b},

    /* Character: ',' */
    {0x2c, &body_font_0x2c},

    /* Character: '-' */
    {0x2d, &body_font_0x2d},

    /* Character: '.' */
    {0x2e, &body_font_0x2e},

    /* Character: '/' */
    {0x2f, &body_font_0x2f},

    /* Character: '0' */
    {0x30, &body_font_0x30},

    /* Character: '1' */
    {0x31, &body_font_0x31},

    /* Character: '2' */
    {0x32, &body_font_0x32},

    /* Character: '3' */
    {0x33, &body_font_0x33},

    /* Character: '4' */
    {0x34, &body_font_0x34},

    /* Character: '5' */
    {0x35, &body_font_0x35},

    /* Character: '6' */
    {0x36, &body_font_0x36},

    /* Character: '7' */
    {0x37, &body_font_0x37},

    /* Character: '8' */
    {0x38, &body_font_0x38},

    /* Character: '9' */
    {0x39, &body_font_0x39},

    /* Character: ':' */
    {0x3a, &body_font_0x3a},

    /* Character: ';' */
    {0x3b, &body_font_0x3b},

    /* Character: '<' */
    {0x3c, &body_font_0x3c},

    /* Character: '=' */
    {0x3d, &body_font_0x3d},

    /* Character: '>' */
    {0x3e, &body_font_0x3e},

    /* Character: '?' */
    {0x3f, &body_font_0x3f},

    /* Character: '\x0040' */
    {0x40, &body_font_0x40},

    /* Character: 'A' */
    {0x41, &body_font_0x41},

    /* Character: 'B' */
    {0x42, &body_font_0x42},

    /* Character: 'C' */
    {0x43, &body_font_0x43},

    /* Character: 'D' */
    {0x44, &body_font_0x44},

    /* Character: 'E' */
    {0x45, &body_font_0x45},

    /* Character: 'F' */
    {0x46, &body_font_0x46},

    /* Character: 'G' */
    {0x47, &body_font_0x47},

    /* Character: 'H' */
    {0x48, &body_font_0x48},

    /* Character: 'I' */
    {0x49, &body_font_0x49},

    /* Character: 'J' */
    {0x4a, &body_font_0x4a},

    /* Character: 'K' */
    {0x4b, &body_font_0x4b},

    /* Character: 'L' */
    {0x4c, &body_font_0x4c},

    /* Character: 'M' */
    {0x4d, &body_font_0x4d},

    /* Character: 'N' */
    {0x4e, &body_font_0x4e},

    /* Character: 'O' */
    {0x4f, &body_font_0x4f},

    /* Character: 'P' */
    {0x50, &body_font_0x50},

    /* Character: 'Q' */
    {0x51, &body_font_0x51},

    /* Character: 'R' */
    {0x52, &body_font_0x52},

    /* Character: 'S' */
    {0x53, &body_font_0x53},

    /* Character: 'T' */
    {0x54, &body_font_0x54},

    /* Character: 'U' */
    {0x55, &body_font_0x55},

    /* Character: 'V' */
    {0x56, &body_font_0x56},

    /* Character: 'W' */
    {0x57, &body_font_0x57},

    /* Character: 'X' */
    {0x58, &body_font_0x58},

    /* Character: 'Y' */
    {0x59, &body_font_0x59},

    /* Character: 'Z' */
    {0x5a, &body_font_0x5a},

    /* Character: '[' */
    {0x5b, &body_font_0x5b},

    /* Character: '\' */
    {0x5c, &body_font_0x5c},

    /* Character: ']' */
    {0x5d, &body_font_0x5d},

    /* Character: '^' */
    {0x5e, &body_font_0x5e},

    /* Character: '_' */
    {0x5f, &body_font_0x5f},

    /* Character: '`' */
    {0x60, &body_font_0x60},

    /* Character: 'a' */
    {0x61, &body_font_0x61},

    /* Character: 'b' */
    {0x62, &body_font_0x62},

    /* Character: 'c' */
    {0x63, &body_font_0x63},

    /* Character: 'd' */
    {0x64, &body_font_0x64},

    /* Character: 'e' */
    {0x65, &body_font_0x65},

    /* Character: 'f' */
    {0x66, &body_font_0x66},

    /* Character: 'g' */
    {0x67, &body_font_0x67},

    /* Character: 'h' */
    {0x68, &body_font_0x68},

    /* Character: 'i' */
    {0x69, &body_font_0x69},

    /* Character: 'j' */
    {0x6a, &body_font_0x6a},

    /* Character: 'k' */
    {0x6b, &body_font_0x6b},

    /* Character: 'l' */
    {0x6c, &body_font_0x6c},

    /* Character: 'm' */
    {0x6d, &body_font_0x6d},

    /* Character: 'n' */
    {0x6e, &body_font_0x6e},

    /* Character: 'o' */
    {0x6f, &body_font_0x6f},

    /* Character: 'p' */
    {0x70, &body_font_0x70},

    /* Character: 'q' */
    {0x71, &body_font_0x71},

    /* Character: 'r' */
    {0x72, &body_font_0x72},

    /* Character: 's' */
    {0x73, &body_font_0x73},

    /* Character: 't' */
    {0x74, &body_font_0x74},

    /* Character: 'u' */
    {0x75, &body_font_0x75},

    /* Character: 'v' */
    {0x76, &body_font_0x76},

    /* Character: 'w' */
    {0x77, &body_font_0x77},

    /* Character: 'x' */
    {0x78, &body_font_0x78},

    /* Character: 'y' */
    {0x79, &body_font_0x79},

    /* Character: 'z' */
    {0x7a, &body_font_0x7a},

    /* Character: '{' */
    {0x7b, &body_font_0x7b},

    /* Character: '|' */
    {0x7c, &body_font_0x7c},

    /* Character: '}' */
    {0x7d, &body_font_0x7d},

    /* Character: '~' (we use this one for checkmark) */
    {0x7e, &body_font_0x7e}
};

static SoftFontCharacters sw_body_font = {
    sizeof(body_font_array) / sizeof(body_font_array[0]), 10, body_font_array};

/************body_font**********/

/************title_font**********/
static uint8_t image_data_title_font_0x20[10] = {0x00, 0x00, 0x00, 0x00, 0x00,
                                                 0x00, 0x00, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x20 = {image_data_title_font_0x20, 5,
                                               10};

static uint8_t image_data_title_font_0x21[6] = {0x7d, 0x00, 0x7d,
                                                0x00, 0x00, 0x00};
static const CharacterImage title_font_0x21 = {image_data_title_font_0x21, 3,
                                               10};

static uint8_t image_data_title_font_0x22[10] = {0x60, 0x00, 0x60, 0x00, 0x60,
                                                 0x00, 0x60, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x22 = {image_data_title_font_0x22, 5,
                                               10};

static uint8_t image_data_title_font_0x23[16] = {
    0x24, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x24, 0x00,
    0x7e, 0x00, 0x7e, 0x00, 0x24, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x23 = {image_data_title_font_0x23, 8,
                                               10};

static uint8_t image_data_title_font_0x24[14] = {0x31, 0x00, 0x79, 0x00, 0xff,
                                                 0x80, 0xff, 0x80, 0x4f, 0x00,
                                                 0x46, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x24 = {image_data_title_font_0x24, 7,
                                               10};

static uint8_t image_data_title_font_0x25[18] = {
    0x21, 0x00, 0x73, 0x00, 0x76, 0x00, 0x2c, 0x00, 0x1a,
    0x00, 0x37, 0x00, 0x67, 0x00, 0x42, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x25 = {image_data_title_font_0x25, 9,
                                               10};

static uint8_t image_data_title_font_0x26[16] = {
    0x36, 0x00, 0x7f, 0x00, 0x49, 0x00, 0x7d, 0x00,
    0x37, 0x00, 0x07, 0x00, 0x05, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x26 = {image_data_title_font_0x26, 8,
                                               10};

static uint8_t image_data_title_font_0x27[6] = {0x60, 0x00, 0x60,
                                                0x00, 0x00, 0x00};
static const CharacterImage title_font_0x27 = {image_data_title_font_0x27, 3,
                                               10};

static uint8_t image_data_title_font_0x28[8] = {0x7f, 0x00, 0xff, 0x80,
                                                0x80, 0x80, 0x00, 0x00};
static const CharacterImage title_font_0x28 = {image_data_title_font_0x28, 4,
                                               10};

static uint8_t image_data_title_font_0x29[8] = {0x80, 0x80, 0xff, 0x80,
                                                0x7f, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x29 = {image_data_title_font_0x29, 4,
                                               10};

static uint8_t image_data_title_font_0x2a[14] = {0x50, 0x00, 0x70, 0x00, 0xf8,
                                                 0x00, 0xf8, 0x00, 0x70, 0x00,
                                                 0x50, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x2a = {image_data_title_font_0x2a, 7,
                                               10};

static uint8_t image_data_title_font_0x2b[14] = {0x08, 0x00, 0x08, 0x00, 0x3e,
                                                 0x00, 0x3e, 0x00, 0x08, 0x00,
                                                 0x08, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x2b = {image_data_title_font_0x2b, 7,
                                               10};

static uint8_t image_data_title_font_0x2c[8] = {0x03, 0x40, 0x03, 0xc0,
                                                0x03, 0x80, 0x00, 0x00};
static const CharacterImage title_font_0x2c = {image_data_title_font_0x2c, 4,
                                               10};

static uint8_t image_data_title_font_0x2d[14] = {0x08, 0x00, 0x08, 0x00, 0x08,
                                                 0x00, 0x08, 0x00, 0x08, 0x00,
                                                 0x08, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x2d = {image_data_title_font_0x2d, 7,
                                               10};

static uint8_t image_data_title_font_0x2e[8] = {0x03, 0x00, 0x03, 0x00,
                                                0x03, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x2e = {image_data_title_font_0x2e, 4,
                                               10};

static uint8_t image_data_title_font_0x2f[18] = {
    0x01, 0x00, 0x03, 0x00, 0x06, 0x00, 0x0c, 0x00, 0x18,
    0x00, 0x30, 0x00, 0x60, 0x00, 0x40, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x2f = {image_data_title_font_0x2f, 9,
                                               10};

static uint8_t image_data_title_font_0x30[14] = {0x3e, 0x00, 0x7f, 0x00, 0x4d,
                                                 0x00, 0x59, 0x00, 0x7f, 0x00,
                                                 0x3e, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x30 = {image_data_title_font_0x30, 7,
                                               10};

static uint8_t image_data_title_font_0x31[8] = {0x40, 0x00, 0x7f, 0x00,
                                                0x7f, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x31 = {image_data_title_font_0x31, 4,
                                               10};

static uint8_t image_data_title_font_0x32[14] = {0x47, 0x00, 0x4f, 0x00, 0x49,
                                                 0x00, 0x49, 0x00, 0x79, 0x00,
                                                 0x31, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x32 = {image_data_title_font_0x32, 7,
                                               10};

static uint8_t image_data_title_font_0x33[14] = {0x41, 0x00, 0x49, 0x00, 0x49,
                                                 0x00, 0x49, 0x00, 0x7f, 0x00,
                                                 0x36, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x33 = {image_data_title_font_0x33, 7,
                                               10};

static uint8_t image_data_title_font_0x34[14] = {0x0c, 0x00, 0x1c, 0x00, 0x34,
                                                 0x00, 0x7f, 0x00, 0x7f, 0x00,
                                                 0x04, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x34 = {image_data_title_font_0x34, 7,
                                               10};

static uint8_t image_data_title_font_0x35[14] = {0x79, 0x00, 0x79, 0x00, 0x49,
                                                 0x00, 0x49, 0x00, 0x4f, 0x00,
                                                 0x46, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x35 = {image_data_title_font_0x35, 7,
                                               10};

static uint8_t image_data_title_font_0x36[14] = {0x3e, 0x00, 0x7f, 0x00, 0x49,
                                                 0x00, 0x49, 0x00, 0x4f, 0x00,
                                                 0x06, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x36 = {image_data_title_font_0x36, 7,
                                               10};

static uint8_t image_data_title_font_0x37[14] = {0x40, 0x00, 0x41, 0x00, 0x47,
                                                 0x00, 0x5e, 0x00, 0x78, 0x00,
                                                 0x60, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x37 = {image_data_title_font_0x37, 7,
                                               10};

static uint8_t image_data_title_font_0x38[14] = {0x36, 0x00, 0x7f, 0x00, 0x49,
                                                 0x00, 0x49, 0x00, 0x7f, 0x00,
                                                 0x36, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x38 = {image_data_title_font_0x38, 7,
                                               10};

static uint8_t image_data_title_font_0x39[14] = {0x30, 0x00, 0x79, 0x00, 0x49,
                                                 0x00, 0x49, 0x00, 0x7f, 0x00,
                                                 0x3e, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x39 = {image_data_title_font_0x39, 7,
                                               10};

static uint8_t image_data_title_font_0x3a[8] = {0x1b, 0x00, 0x1b, 0x00,
                                                0x1b, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x3a = {image_data_title_font_0x3a, 4,
                                               10};

static uint8_t image_data_title_font_0x3b[8] = {0x1b, 0x40, 0x1b, 0xc0,
                                                0x1b, 0x80, 0x00, 0x00};
static const CharacterImage title_font_0x3b = {image_data_title_font_0x3b, 4,
                                               10};

static uint8_t image_data_title_font_0x3c[12] = {
    0x08, 0x00, 0x1c, 0x00, 0x36, 0x00, 0x63, 0x00, 0x41, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x3c = {image_data_title_font_0x3c, 6,
                                               10};

static uint8_t image_data_title_font_0x3d[14] = {0x14, 0x00, 0x14, 0x00, 0x14,
                                                 0x00, 0x14, 0x00, 0x14, 0x00,
                                                 0x14, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x3d = {image_data_title_font_0x3d, 7,
                                               10};

static uint8_t image_data_title_font_0x3e[12] = {
    0x41, 0x00, 0x63, 0x00, 0x36, 0x00, 0x1c, 0x00, 0x08, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x3e = {image_data_title_font_0x3e, 6,
                                               10};

static uint8_t image_data_title_font_0x3f[14] = {0x20, 0x00, 0x60, 0x00, 0x45,
                                                 0x00, 0x4d, 0x00, 0x78, 0x00,
                                                 0x30, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x3f = {image_data_title_font_0x3f, 7,
                                               10};

static uint8_t image_data_title_font_0x40[18] = {
    0x3e, 0x00, 0x7f, 0x00, 0xc5, 0x80, 0xae, 0x80, 0xaa,
    0x80, 0xbe, 0x80, 0xde, 0x80, 0x7e, 0x00, 0x3c, 0x00};
static const CharacterImage title_font_0x40 = {image_data_title_font_0x40, 9,
                                               10};

static uint8_t image_data_title_font_0x41[14] = {0x3f, 0x00, 0x7f, 0x00, 0x48,
                                                 0x00, 0x48, 0x00, 0x7f, 0x00,
                                                 0x3f, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x41 = {image_data_title_font_0x41, 7,
                                               10};

static uint8_t image_data_title_font_0x42[14] = {0x7f, 0x00, 0x7f, 0x00, 0x49,
                                                 0x00, 0x49, 0x00, 0x7f, 0x00,
                                                 0x36, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x42 = {image_data_title_font_0x42, 7,
                                               10};

static uint8_t image_data_title_font_0x43[14] = {0x3e, 0x00, 0x7f, 0x00, 0x41,
                                                 0x00, 0x41, 0x00, 0x63, 0x00,
                                                 0x22, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x43 = {image_data_title_font_0x43, 7,
                                               10};

static uint8_t image_data_title_font_0x44[14] = {0x7f, 0x00, 0x7f, 0x00, 0x41,
                                                 0x00, 0x41, 0x00, 0x7f, 0x00,
                                                 0x3e, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x44 = {image_data_title_font_0x44, 7,
                                               10};

static uint8_t image_data_title_font_0x45[14] = {0x7f, 0x00, 0x7f, 0x00, 0x49,
                                                 0x00, 0x49, 0x00, 0x49, 0x00,
                                                 0x41, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x45 = {image_data_title_font_0x45, 7,
                                               10};

static uint8_t image_data_title_font_0x46[14] = {0x7f, 0x00, 0x7f, 0x00, 0x48,
                                                 0x00, 0x48, 0x00, 0x48, 0x00,
                                                 0x40, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x46 = {image_data_title_font_0x46, 7,
                                               10};

static uint8_t image_data_title_font_0x47[14] = {0x3e, 0x00, 0x7f, 0x00, 0x41,
                                                 0x00, 0x49, 0x00, 0x6f, 0x00,
                                                 0x2f, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x47 = {image_data_title_font_0x47, 7,
                                               10};

static uint8_t image_data_title_font_0x48[14] = {0x7f, 0x00, 0x7f, 0x00, 0x08,
                                                 0x00, 0x08, 0x00, 0x7f, 0x00,
                                                 0x7f, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x48 = {image_data_title_font_0x48, 7,
                                               10};

static uint8_t image_data_title_font_0x49[10] = {0x41, 0x00, 0x7f, 0x00, 0x7f,
                                                 0x00, 0x41, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x49 = {image_data_title_font_0x49, 5,
                                               10};

static uint8_t image_data_title_font_0x4a[14] = {0x02, 0x00, 0x03, 0x00, 0x01,
                                                 0x00, 0x01, 0x00, 0x7f, 0x00,
                                                 0x7e, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x4a = {image_data_title_font_0x4a, 7,
                                               10};

static uint8_t image_data_title_font_0x4b[14] = {0x7f, 0x00, 0x7f, 0x00, 0x1c,
                                                 0x00, 0x36, 0x00, 0x63, 0x00,
                                                 0x41, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x4b = {image_data_title_font_0x4b, 7,
                                               10};

static uint8_t image_data_title_font_0x4c[14] = {0x7f, 0x00, 0x7f, 0x00, 0x01,
                                                 0x00, 0x01, 0x00, 0x01, 0x00,
                                                 0x01, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x4c = {image_data_title_font_0x4c, 7,
                                               10};

static uint8_t image_data_title_font_0x4d[18] = {
    0x7f, 0x00, 0x7f, 0x00, 0x30, 0x00, 0x18, 0x00, 0x18,
    0x00, 0x30, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x4d = {image_data_title_font_0x4d, 9,
                                               10};

static uint8_t image_data_title_font_0x4e[14] = {0x7f, 0x00, 0x7f, 0x00, 0x18,
                                                 0x00, 0x0c, 0x00, 0x7f, 0x00,
                                                 0x7f, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x4e = {image_data_title_font_0x4e, 7,
                                               10};

static uint8_t image_data_title_font_0x4f[14] = {0x3e, 0x00, 0x7f, 0x00, 0x41,
                                                 0x00, 0x41, 0x00, 0x7f, 0x00,
                                                 0x3e, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x4f = {image_data_title_font_0x4f, 7,
                                               10};

static uint8_t image_data_title_font_0x50[14] = {0x7f, 0x00, 0x7f, 0x00, 0x44,
                                                 0x00, 0x44, 0x00, 0x7c, 0x00,
                                                 0x38, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x50 = {image_data_title_font_0x50, 7,
                                               10};

static uint8_t image_data_title_font_0x51[14] = {0x3e, 0x00, 0x7f, 0x00, 0x41,
                                                 0x00, 0x41, 0x00, 0x7f, 0x80,
                                                 0x3e, 0x80, 0x00, 0x00};
static const CharacterImage title_font_0x51 = {image_data_title_font_0x51, 7,
                                               10};

static uint8_t image_data_title_font_0x52[14] = {0x7f, 0x00, 0x7f, 0x00, 0x44,
                                                 0x00, 0x46, 0x00, 0x7f, 0x00,
                                                 0x39, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x52 = {image_data_title_font_0x52, 7,
                                               10};

static uint8_t image_data_title_font_0x53[14] = {0x32, 0x00, 0x7b, 0x00, 0x49,
                                                 0x00, 0x49, 0x00, 0x6f, 0x00,
                                                 0x26, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x53 = {image_data_title_font_0x53, 7,
                                               10};

static uint8_t image_data_title_font_0x54[14] = {0x40, 0x00, 0x40, 0x00, 0x7f,
                                                 0x00, 0x7f, 0x00, 0x40, 0x00,
                                                 0x40, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x54 = {image_data_title_font_0x54, 7,
                                               10};

static uint8_t image_data_title_font_0x55[14] = {0x7e, 0x00, 0x7f, 0x00, 0x01,
                                                 0x00, 0x01, 0x00, 0x7f, 0x00,
                                                 0x7e, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x55 = {image_data_title_font_0x55, 7,
                                               10};

static uint8_t image_data_title_font_0x56[14] = {0x78, 0x00, 0x7e, 0x00, 0x07,
                                                 0x00, 0x07, 0x00, 0x7e, 0x00,
                                                 0x78, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x56 = {image_data_title_font_0x56, 7,
                                               10};

static uint8_t image_data_title_font_0x57[18] = {
    0x7e, 0x00, 0x7f, 0x00, 0x01, 0x00, 0x7f, 0x00, 0x7f,
    0x00, 0x01, 0x00, 0x7f, 0x00, 0x7e, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x57 = {image_data_title_font_0x57, 9,
                                               10};

static uint8_t image_data_title_font_0x58[14] = {0x63, 0x00, 0x77, 0x00, 0x1c,
                                                 0x00, 0x1c, 0x00, 0x77, 0x00,
                                                 0x63, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x58 = {image_data_title_font_0x58, 7,
                                               10};

static uint8_t image_data_title_font_0x59[14] = {0x70, 0x00, 0x78, 0x00, 0x0f,
                                                 0x00, 0x0f, 0x00, 0x78, 0x00,
                                                 0x70, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x59 = {image_data_title_font_0x59, 7,
                                               10};

static uint8_t image_data_title_font_0x5a[14] = {0x43, 0x00, 0x47, 0x00, 0x4d,
                                                 0x00, 0x59, 0x00, 0x71, 0x00,
                                                 0x61, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x5a = {image_data_title_font_0x5a, 7,
                                               10};

static uint8_t image_data_title_font_0x5b[10] = {0x7f, 0x00, 0x7f, 0x00, 0x41,
                                                 0x00, 0x41, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x5b = {image_data_title_font_0x5b, 5,
                                               10};

static uint8_t image_data_title_font_0x5c[18] = {
    0x40, 0x00, 0x60, 0x00, 0x30, 0x00, 0x18, 0x00, 0x0c,
    0x00, 0x06, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x5c = {image_data_title_font_0x5c, 9,
                                               10};

static uint8_t image_data_title_font_0x5d[10] = {0x41, 0x00, 0x41, 0x00, 0x7f,
                                                 0x00, 0x7f, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x5d = {image_data_title_font_0x5d, 5,
                                               10};

static uint8_t image_data_title_font_0x5e[10] = {0x20, 0x00, 0x60, 0x00, 0x60,
                                                 0x00, 0x20, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x5e = {image_data_title_font_0x5e, 5,
                                               10};

static uint8_t image_data_title_font_0x5f[14] = {0x01, 0x00, 0x01, 0x00, 0x01,
                                                 0x00, 0x01, 0x00, 0x01, 0x00,
                                                 0x01, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x5f = {image_data_title_font_0x5f, 7,
                                               10};

static uint8_t image_data_title_font_0x60[8] = {0x40, 0x00, 0x60, 0x00,
                                                0x20, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x60 = {image_data_title_font_0x60, 4,
                                               10};

static uint8_t image_data_title_font_0x61[14] = {0x02, 0x00, 0x17, 0x00, 0x15,
                                                 0x00, 0x15, 0x00, 0x1f, 0x00,
                                                 0x0f, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x61 = {image_data_title_font_0x61, 7,
                                               10};

static uint8_t image_data_title_font_0x62[14] = {0x7f, 0x00, 0x7f, 0x00, 0x11,
                                                 0x00, 0x11, 0x00, 0x1f, 0x00,
                                                 0x0e, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x62 = {image_data_title_font_0x62, 7,
                                               10};

static uint8_t image_data_title_font_0x63[14] = {0x0e, 0x00, 0x1f, 0x00, 0x11,
                                                 0x00, 0x11, 0x00, 0x11, 0x00,
                                                 0x11, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x63 = {image_data_title_font_0x63, 7,
                                               10};

static uint8_t image_data_title_font_0x64[14] = {0x0e, 0x00, 0x1f, 0x00, 0x11,
                                                 0x00, 0x11, 0x00, 0x7f, 0x00,
                                                 0x7f, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x64 = {image_data_title_font_0x64, 7,
                                               10};

static uint8_t image_data_title_font_0x65[14] = {0x0e, 0x00, 0x1f, 0x00, 0x15,
                                                 0x00, 0x15, 0x00, 0x1d, 0x00,
                                                 0x0d, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x65 = {image_data_title_font_0x65, 7,
                                               10};

static uint8_t image_data_title_font_0x66[12] = {
    0x10, 0x00, 0x3f, 0x00, 0x7f, 0x00, 0x50, 0x00, 0x50, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x66 = {image_data_title_font_0x66, 6,
                                               10};

static uint8_t image_data_title_font_0x67[14] = {0x0e, 0x00, 0x1f, 0x40, 0x11,
                                                 0x40, 0x11, 0x40, 0x1f, 0xc0,
                                                 0x1f, 0x80, 0x00, 0x00};
static const CharacterImage title_font_0x67 = {image_data_title_font_0x67, 7,
                                               10};

static uint8_t image_data_title_font_0x68[14] = {0x7f, 0x00, 0x7f, 0x00, 0x10,
                                                 0x00, 0x10, 0x00, 0x1f, 0x00,
                                                 0x0f, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x68 = {image_data_title_font_0x68, 7,
                                               10};

static uint8_t image_data_title_font_0x69[6] = {0x5f, 0x00, 0x5f,
                                                0x00, 0x00, 0x00};
static const CharacterImage title_font_0x69 = {image_data_title_font_0x69, 3,
                                               10};

static uint8_t image_data_title_font_0x6a[8] = {0x00, 0x40, 0x5f, 0xc0,
                                                0x5f, 0x80, 0x00, 0x00};
static const CharacterImage title_font_0x6a = {image_data_title_font_0x6a, 4,
                                               10};

static uint8_t image_data_title_font_0x6b[12] = {
    0x7f, 0x00, 0x7f, 0x00, 0x0e, 0x00, 0x1b, 0x00, 0x11, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x6b = {image_data_title_font_0x6b, 6,
                                               10};

static uint8_t image_data_title_font_0x6c[6] = {0x7f, 0x00, 0x7f,
                                                0x00, 0x00, 0x00};
static const CharacterImage title_font_0x6c = {image_data_title_font_0x6c, 3,
                                               10};

static uint8_t image_data_title_font_0x6d[18] = {
    0x1f, 0x00, 0x1f, 0x00, 0x10, 0x00, 0x1f, 0x00, 0x1f,
    0x00, 0x10, 0x00, 0x1f, 0x00, 0x0f, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x6d = {image_data_title_font_0x6d, 9,
                                               10};

static uint8_t image_data_title_font_0x6e[14] = {0x1f, 0x00, 0x1f, 0x00, 0x10,
                                                 0x00, 0x10, 0x00, 0x1f, 0x00,
                                                 0x0f, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x6e = {image_data_title_font_0x6e, 7,
                                               10};

static uint8_t image_data_title_font_0x6f[14] = {0x0e, 0x00, 0x1f, 0x00, 0x11,
                                                 0x00, 0x11, 0x00, 0x1f, 0x00,
                                                 0x0e, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x6f = {image_data_title_font_0x6f, 7,
                                               10};

static uint8_t image_data_title_font_0x70[14] = {0x1f, 0xc0, 0x1f, 0xc0, 0x11,
                                                 0x00, 0x11, 0x00, 0x1f, 0x00,
                                                 0x0e, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x70 = {image_data_title_font_0x70, 7,
                                               10};

static uint8_t image_data_title_font_0x71[14] = {0x0e, 0x00, 0x1f, 0x00, 0x11,
                                                 0x00, 0x11, 0x00, 0x1f, 0xc0,
                                                 0x1f, 0xc0, 0x00, 0x00};
static const CharacterImage title_font_0x71 = {image_data_title_font_0x71, 7,
                                               10};

static uint8_t image_data_title_font_0x72[12] = {
    0x1f, 0x00, 0x1f, 0x00, 0x18, 0x00, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x72 = {image_data_title_font_0x72, 6,
                                               10};

static uint8_t image_data_title_font_0x73[14] = {0x09, 0x00, 0x1d, 0x00, 0x15,
                                                 0x00, 0x15, 0x00, 0x17, 0x00,
                                                 0x12, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x73 = {image_data_title_font_0x73, 7,
                                               10};

static uint8_t image_data_title_font_0x74[12] = {
    0x10, 0x00, 0x7e, 0x00, 0x7f, 0x00, 0x11, 0x00, 0x11, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x74 = {image_data_title_font_0x74, 6,
                                               10};

static uint8_t image_data_title_font_0x75[14] = {0x1e, 0x00, 0x1f, 0x00, 0x01,
                                                 0x00, 0x01, 0x00, 0x1f, 0x00,
                                                 0x1f, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x75 = {image_data_title_font_0x75, 7,
                                               10};

static uint8_t image_data_title_font_0x76[14] = {0x18, 0x00, 0x1e, 0x00, 0x07,
                                                 0x00, 0x07, 0x00, 0x1e, 0x00,
                                                 0x18, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x76 = {image_data_title_font_0x76, 7,
                                               10};

static uint8_t image_data_title_font_0x77[18] = {
    0x1e, 0x00, 0x1f, 0x00, 0x01, 0x00, 0x1f, 0x00, 0x1f,
    0x00, 0x01, 0x00, 0x1f, 0x00, 0x1e, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x77 = {image_data_title_font_0x77, 9,
                                               10};

static uint8_t image_data_title_font_0x78[14] = {0x11, 0x00, 0x1b, 0x00, 0x0e,
                                                 0x00, 0x0e, 0x00, 0x1b, 0x00,
                                                 0x11, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x78 = {image_data_title_font_0x78, 7,
                                               10};

static uint8_t image_data_title_font_0x79[14] = {0x1e, 0x00, 0x1f, 0x40, 0x01,
                                                 0x40, 0x01, 0x40, 0x1f, 0xc0,
                                                 0x1f, 0x80, 0x00, 0x00};
static const CharacterImage title_font_0x79 = {image_data_title_font_0x79, 7,
                                               10};

static uint8_t image_data_title_font_0x7a[14] = {0x11, 0x00, 0x13, 0x00, 0x17,
                                                 0x00, 0x1d, 0x00, 0x19, 0x00,
                                                 0x11, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x7a = {image_data_title_font_0x7a, 7,
                                               10};

static uint8_t image_data_title_font_0x7b[12] = {
    0x08, 0x00, 0x3e, 0x00, 0x77, 0x00, 0x41, 0x00, 0x41, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x7b = {image_data_title_font_0x7b, 6,
                                               10};

static uint8_t image_data_title_font_0x7c[6] = {0x7f, 0x00, 0x7f,
                                                0x00, 0x00, 0x00};
static const CharacterImage title_font_0x7c = {image_data_title_font_0x7c, 3,
                                               10};

static uint8_t image_data_title_font_0x7d[12] = {
    0x41, 0x00, 0x41, 0x00, 0x77, 0x00, 0x3e, 0x00, 0x08, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x7d = {image_data_title_font_0x7d, 6,
                                               10};

static uint8_t image_data_title_font_0x7e[14] = {0x20, 0x00, 0x60, 0x00, 0x60,
                                                 0x00, 0x60, 0x00, 0x60, 0x00,
                                                 0x40, 0x00, 0x00, 0x00};
static const CharacterImage title_font_0x7e = {image_data_title_font_0x7e, 7,
                                               10};

static const Character title_font_array[] = {

    /* Character: ' ' */
    {0x20, &title_font_0x20},

    /* Character: '!' */
    {0x21, &title_font_0x21},

    /* Character: '"' */
    {0x22, &title_font_0x22},

    /* Character: '#' */
    {0x23, &title_font_0x23},

    /* Character: '$' */
    {0x24, &title_font_0x24},

    /* Character: '%' */
    {0x25, &title_font_0x25},

    /* Character: '&' */
    {0x26, &title_font_0x26},

    /* Character: ''' */
    {0x27, &title_font_0x27},

    /* Character: '(' */
    {0x28, &title_font_0x28},

    /* Character: ')' */
    {0x29, &title_font_0x29},

    /* Character: '*' */
    {0x2a, &title_font_0x2a},

    /* Character: '+' */
    {0x2b, &title_font_0x2b},

    /* Character: ',' */
    {0x2c, &title_font_0x2c},

    /* Character: '-' */
    {0x2d, &title_font_0x2d},

    /* Character: '.' */
    {0x2e, &title_font_0x2e},

    /* Character: '/' */
    {0x2f, &title_font_0x2f},

    /* Character: '0' */
    {0x30, &title_font_0x30},

    /* Character: '1' */
    {0x31, &title_font_0x31},

    /* Character: '2' */
    {0x32, &title_font_0x32},

    /* Character: '3' */
    {0x33, &title_font_0x33},

    /* Character: '4' */
    {0x34, &title_font_0x34},

    /* Character: '5' */
    {0x35, &title_font_0x35},

    /* Character: '6' */
    {0x36, &title_font_0x36},

    /* Character: '7' */
    {0x37, &title_font_0x37},

    /* Character: '8' */
    {0x38, &title_font_0x38},

    /* Character: '9' */
    {0x39, &title_font_0x39},

    /* Character: ':' */
    {0x3a, &title_font_0x3a},

    /* Character: ';' */
    {0x3b, &title_font_0x3b},

    /* Character: '<' */
    {0x3c, &title_font_0x3c},

    /* Character: '=' */
    {0x3d, &title_font_0x3d},

    /* Character: '>' */
    {0x3e, &title_font_0x3e},

    /* Character: '?' */
    {0x3f, &title_font_0x3f},

    /* Character: '\x0040' */
    {0x40, &title_font_0x40},

    /* Character: 'A' */
    {0x41, &title_font_0x41},

    /* Character: 'B' */
    {0x42, &title_font_0x42},

    /* Character: 'C' */
    {0x43, &title_font_0x43},

    /* Character: 'D' */
    {0x44, &title_font_0x44},

    /* Character: 'E' */
    {0x45, &title_font_0x45},

    /* Character: 'F' */
    {0x46, &title_font_0x46},

    /* Character: 'G' */
    {0x47, &title_font_0x47},

    /* Character: 'H' */
    {0x48, &title_font_0x48},

    /* Character: 'I' */
    {0x49, &title_font_0x49},

    /* Character: 'J' */
    {0x4a, &title_font_0x4a},

    /* Character: 'K' */
    {0x4b, &title_font_0x4b},

    /* Character: 'L' */
    {0x4c, &title_font_0x4c},

    /* Character: 'M' */
    {0x4d, &title_font_0x4d},

    /* Character: 'N' */
    {0x4e, &title_font_0x4e},

    /* Character: 'O' */
    {0x4f, &title_font_0x4f},

    /* Character: 'P' */
    {0x50, &title_font_0x50},

    /* Character: 'Q' */
    {0x51, &title_font_0x51},

    /* Character: 'R' */
    {0x52, &title_font_0x52},

    /* Character: 'S' */
    {0x53, &title_font_0x53},

    /* Character: 'T' */
    {0x54, &title_font_0x54},

    /* Character: 'U' */
    {0x55, &title_font_0x55},

    /* Character: 'V' */
    {0x56, &title_font_0x56},

    /* Character: 'W' */
    {0x57, &title_font_0x57},

    /* Character: 'X' */
    {0x58, &title_font_0x58},

    /* Character: 'Y' */
    {0x59, &title_font_0x59},

    /* Character: 'Z' */
    {0x5a, &title_font_0x5a},

    /* Character: '[' */
    {0x5b, &title_font_0x5b},

    /* Character: '\' */
    {0x5c, &title_font_0x5c},

    /* Character: ']' */
    {0x5d, &title_font_0x5d},

    /* Character: '^' */
    {0x5e, &title_font_0x5e},

    /* Character: '_' */
    {0x5f, &title_font_0x5f},

    /* Character: '`' */
    {0x60, &title_font_0x60},

    /* Character: 'a' */
    {0x61, &title_font_0x61},

    /* Character: 'b' */
    {0x62, &title_font_0x62},

    /* Character: 'c' */
    {0x63, &title_font_0x63},

    /* Character: 'd' */
    {0x64, &title_font_0x64},

    /* Character: 'e' */
    {0x65, &title_font_0x65},

    /* Character: 'f' */
    {0x66, &title_font_0x66},

    /* Character: 'g' */
    {0x67, &title_font_0x67},

    /* Character: 'h' */
    {0x68, &title_font_0x68},

    /* Character: 'i' */
    {0x69, &title_font_0x69},

    /* Character: 'j' */
    {0x6a, &title_font_0x6a},

    /* Character: 'k' */
    {0x6b, &title_font_0x6b},

    /* Character: 'l' */
    {0x6c, &title_font_0x6c},

    /* Character: 'm' */
    {0x6d, &title_font_0x6d},

    /* Character: 'n' */
    {0x6e, &title_font_0x6e},

    /* Character: 'o' */
    {0x6f, &title_font_0x6f},

    /* Character: 'p' */
    {0x70, &title_font_0x70},

    /* Character: 'q' */
    {0x71, &title_font_0x71},

    /* Character: 'r' */
    {0x72, &title_font_0x72},

    /* Character: 's' */
    {0x73, &title_font_0x73},

    /* Character: 't' */
    {0x74, &title_font_0x74},

    /* Character: 'u' */
    {0x75, &title_font_0x75},

    /* Character: 'v' */
    {0x76, &title_font_0x76},

    /* Character: 'w' */
    {0x77, &title_font_0x77},

    /* Character: 'x' */
    {0x78, &title_font_0x78},

    /* Character: 'y' */
    {0x79, &title_font_0x79},

    /* Character: 'z' */
    {0x7a, &title_font_0x7a},

    /* Character: '{' */
    {0x7b, &title_font_0x7b},

    /* Character: '|' */
    {0x7c, &title_font_0x7c},

    /* Character: '}' */
    {0x7d, &title_font_0x7d},

    /* Character: '~' */
    {0x7e, &title_font_0x7e}
};

static SoftFontCharacters sw_title_font = {
    sizeof(title_font_array) / sizeof(title_font_array[0]), 10,
    title_font_array};
/************title_font**********/


/* --- Pin Font ------------------------------------------------------------ */
static uint8_t image_data_pin[32] = {0};
static CharacterImage pin_font_image = {image_data_pin, 16, 16};
static Font pin_font = {ENGLISH, PIN_TEXT, ASCII_8X16, 16, 16, &pin_font_image};


/* --- BLE Pin Font ------------------------------------------------------------ */
static uint8_t image_data_ble_pin[64] = {0};
static CharacterImage ble_pin_font_image = {image_data_ble_pin, 16, 32};
static Font ble_pin_font = {ENGLISH, BLE_PIN_TEXT, ASCII_16X32,
                        16,      32,       &ble_pin_font_image};

/* --- Title Font ---------------------------------------------------------- */
static uint8_t image_data_title[64] = {0};
static CharacterImage title_font_image = {image_data_title, 16, 16};
static Font title_font = {ENGLISH, TITLE_TEXT, ASCII_8X16,
                          16,      16,         &title_font_image};

/* --- Body Font ---------------------------------------------------------*/
static uint8_t image_data_body[32] = {0};
static CharacterImage body_font_image = {image_data_body, 16, 16};
static Font body_font = {ENGLISH, BODY_TEXT, ASCII_8X16,
                         16,      16,        &body_font_image};

static uint8_t image_font_sadface_9x10[9 * 10] = {
    0x7f, 0x80, 0xa8, 0x40, 0x91, 0x40, 0xaa, 0x40, 0x82,
    0x40, 0xaa, 0x40, 0x91, 0x40, 0xa8, 0x40, 0x7f, 0x80};
static const CharacterImage sadface_9x10 = {image_font_sadface_9x10, 9, 10};

#define MAX_FONT_IMAGE_BYTES 72
static char image_tmp_data[MAX_FONT_IMAGE_BYTES] = {0};

static void set_font_language(Font *font) {
    const char *language = (const char *)storage_getLanguage();
    if (NULL == language) {
        font->language = ENGLISH;  // default language
    } else if (strcmp(language, "english") == 0) {
        font->language = ENGLISH;
    } else if (strcmp(language, "chinese") == 0) {
        font->language = CHINESE;
    } else if (strcmp(language, "japanese") == 0) {
        font->language = JAPANESE;
    } else if (strcmp(language, "korean") == 0) {
        font->language = KOREAN;
    } else {
        font->language = ENGLISH;
    }
}

static void set_font_type(Font *font) {
    switch (font->text_type) {
        case TITLE_TEXT:
            switch (font->language) {
                case ENGLISH:
                    font->font_type = ASCII_16_A;
                    font->width = 8;
                    font->height = 16;
                    break;
                case CHINESE:
                    font->font_type = SEL_GB;
                    font->width = 16;
                    font->height = 16;
                    break;
                case JAPANESE:
                    font->font_type = SEL_JIS;
                    font->width = 16;
                    font->height = 16;
                    break;
                case KOREAN:
                    font->font_type = SEL_KSC;
                    font->width = 16;
                    font->height = 16;
                    break;
                default:
                    break;
            }
            break;
        case BODY_TEXT:
            switch (font->language) {
                case ENGLISH:
                    font->font_type = SW_FONT;
                    font->width = 6;
                    font->height = 10;
                    break;
                case CHINESE:
                    font->font_type = SEL_GB;
                    font->width = 16;
                    font->height = 16;
                    break;
                case JAPANESE:
                    font->font_type = SEL_JIS;
                    font->width = 16;
                    font->height = 16;
                    break;
                case KOREAN:
                    font->font_type = SEL_KSC;
                    font->width = 16;
                    font->height = 16;
                    break;
                default:
                    break;
            }
            break;
        case PIN_TEXT:
            font->font_type = ASCII_16_A;
            font->width = 8;
            font->height = 16;
            break;
        case BLE_PIN_TEXT:
            font->font_type = ASCII_16X32;
            font->width = 16;
            font->height = 32;
            break;
        case ADDR_TEXT:
            font->font_type = SW_FONT;
            font->width = 6;
            font->height = 10;
            break;
        default:
            break;
    }
}

const Font *get_font(TextType text_type) {
    Font *p_font = NULL;
    switch (text_type) {
        case TITLE_TEXT:
            p_font = &title_font;
            p_font->text_type = TITLE_TEXT;
            break;
        case BODY_TEXT:
            p_font = &body_font;
            p_font->text_type = BODY_TEXT;
            break;
        case PIN_TEXT:
            p_font = &pin_font;
            p_font->text_type = PIN_TEXT;
            break;
        case ADDR_TEXT:
            p_font = &body_font;
            p_font->text_type = ADDR_TEXT;
            break;
        case BLE_PIN_TEXT:
            p_font = &ble_pin_font;
            p_font->text_type = BLE_PIN_TEXT;
			break;
        default:
            break;
    }

    if (p_font != NULL) {
        set_font_language(p_font);
        set_font_type(p_font);
    }
    return p_font;
}

static uint8_t bits_shift(uint8_t c) {
    c = (c << 4) | (c >> 4);
    c = ((c << 2) & 0xcc) | ((c >> 2) & 0x33);
    c = ((c << 1) & 0xaa) | ((c >> 1) & 0x55);
    return c;
}

static void convert_font_data(const Font *font) {
    uint8_t data_heigth = (font->image->height + 7) / 8;
    uint8_t data_width = font->image->width;
    uint8_t data_align = 0;
    char *image_data = (char *)font->image->data;
    switch (font->font_type) {
        case ASCII_12_B_A:
        case ASCII_12_B_T:
            data_align = 12 - font->image->width;
            image_data = (char *)(font->image->data + 2);
            break;
        case ASCII_16_A:
        case ASCII_16_T:
            data_align = 16 - font->image->width;
            image_data = (char *)(font->image->data + 2);
            break;
        default:
            break;
    }

    memset(image_tmp_data, 0, sizeof(image_tmp_data));
    uint8_t width_step = data_width + data_align;
    for (uint8_t col = 0; col < data_width; col++) {
        for (uint8_t row = 0; row < data_heigth; row++) {
            uint8_t c = (uint8_t)image_data[row * width_step + col];
            image_tmp_data[col * data_heigth + row] = bits_shift(c);
        }
    }

    memcpy(font->image->data, image_tmp_data, data_heigth * data_width);
}


/*
 * font_get_char() - Get a character from the provided font
 *
 *     Returns the image for the given character. If the font does not have the
 *     character, it returns a sadface emoji.
 *
 * INPUT
 *     - font: pointer to font structure
 *     - c: ascii charactor
 * OUTPUT
 *     pointer to ascii charactor image
 *
 */
const CharacterImage *font_get_char(const Font *font, char c) {
    const SoftFontCharacters *sw_font = NULL;
    switch (font->text_type) {
        case PIN_TEXT:
            sw_font = &sw_pin_font;
            break;
        case TITLE_TEXT:
            sw_font = &sw_title_font;
            break;
        case BODY_TEXT:
        case ADDR_TEXT:
            sw_font = &sw_body_font;
            break;
        default:
            break;
    }
    if (NULL == sw_font)
    {
        return &sadface_9x10;
    }

    for (int i = 0; i < sw_font->length; i++) {
        if (sw_font->characters[i].code == c) {
            return sw_font->characters[i].image;
        }
    }

    return &sadface_9x10;
}

const CharacterImage *get_charactor_image(const Font *font,
                                          const char *charactor, int *index,
                                          int *ch) {
    if (font->font_type == SW_FONT)
    {
        char c = *charactor;
        if (index != NULL)
        {
            *index = 1;
        }
        if (ch != NULL)
        {
            *ch = *(charactor);
        }

        return font_get_char(font, c);
    } else {
        int idx = 0;
        int c = u8_nextchar(charactor, &idx);
        if (index != NULL) {
            *index = idx;
        }
        if (ch != NULL) {
            *ch = c;
        }

        memset(font->image->data, 0, FONT_IMAGE_DATA_LEN(font));
        uint16_t img_len = zk_getbmp(c, font->font_type, font->image->data);
        if (img_len > 0) {
            switch (font->font_type) {
                case ASCII_12_B_A:
                case ASCII_12_B_T:
                case ASCII_16_A:
                case ASCII_16_T: {
                    font->image->width = FONT_IMG_WITH(font->image->data);
                    break;
                }
                default: {
                    font->image->width = font->width;
                    break;
                }
            }
            font->image->height = font->height;
            convert_font_data(font);
            return font->image;
        }
    }
 

    return &sadface_9x10;
}

/*
 * font_height() - Get font height
 *
 * INPUT
 *     - font: pointer to font structure
 * OUTPUT
 *     font height
 */
uint32_t font_height(const Font *font) { return font->height; }

/*
 * font_width() - Get font width
 *
 * INPUT
 *     - font: pointer to font structure
 * OUTPUT
 *     font width
 */
uint32_t font_width(const Font *font) {
    /*Return worst case width using the | char as the reference.  */
    if (font->font_type == SW_FONT)
    {
        return font_get_char(font, '|')->width;
    }
    return font->width;
}

/*
 * calc_str_width() - Get string width respect to font type and string size
 *
 * INPUT
 *     - font: pointer to font structure
 *     - str: pointer string
 * OUTPUT
 *     calculated string width

 need to change to fit uni-code string
 */

uint32_t calc_str_width(const Font *font, const char *str) {
    uint32_t width = 0;
    switch (font->font_type) {
        case ASCII_12_B_A:
        case ASCII_12_B_T:
        case ASCII_16_A:
        case ASCII_16_T: 
        case SW_FONT: {
            const char *ptr = str;
            while (*ptr != 0) {
                int ch = 0;
                int idx = 0;
                const CharacterImage *img = get_charactor_image(font, ptr, &idx, &ch);
                width += img->width;
                ptr = ptr + idx;
            }
        } break;
        default: {
            uint32_t len = u8_strlen((char *)str);
            width = font_width(font) * len;
        } break;
    }

    return width;
}

/*
 * calc_str_line() - Calculates how many lines a string will occupy given a line
 * width
 *
 * INPUT
 *     - font: pointer to font structure
 *     - str: pointer string
 *     - line_width: maximum line width before string is wrapped
 * OUTPUT
 *     line count
 */
uint32_t calc_str_line(const Font *font, const char *str, uint16_t line_width) {
    uint8_t line_count = 1;
    uint16_t x_offset = 0;
    const char *ptr = str;
    while (*ptr != 0) {
        int ch = 0;
        int next_offset = 0;
        const CharacterImage *img =
            get_charactor_image(font, ptr, &next_offset, &ch);
        uint16_t character_width = img->width;
        uint16_t word_width = character_width;
        char *next_character = (char *)ptr + next_offset;

        /* Allow line breaks */
        if (*ptr == '\n') {
            line_count++;
            x_offset = 0;
            ptr += next_offset;
            continue;
        }

        /* Calculate next word width */
        if (*ptr == ' ') {
            while (*next_character && *next_character != ' ' &&
                   *next_character != '\n') {
                word_width += font_width(font);
                next_character++;
            }
        }

        /* New line? */
        if (x_offset + word_width > line_width) {
            line_count++;
            x_offset = 0;
        }

        /* Remove leading spaces */
        if (x_offset == 0 && *ptr == ' ') {
            ptr += next_offset;
            continue;
        }

        x_offset += character_width;
        ptr += next_offset;
    }

    return line_count;
}
