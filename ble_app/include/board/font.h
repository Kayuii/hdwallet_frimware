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

#ifndef FONT_H
#define FONT_H

/* === Includes ============================================================ */

#include <stdint.h>

/* === Typedefs ============================================================ */

/* Data pertaining to the image of a character */
typedef struct {
    uint8_t *data;
    uint16_t width;
    uint16_t height;
} CharacterImage;

/* Character information. */
typedef struct {
    long int code;
    const CharacterImage *image;
} Character;

/* A complete font package. */
typedef struct {
    int length;
    int size;
    const Character *characters;
} SoftFontCharacters;

typedef enum { ENGLISH, CHINESE, JAPANESE, KOREAN } FontLanguage;

typedef enum {
    TITLE_TEXT,
    BODY_TEXT,
    PIN_TEXT,
    ADDR_TEXT,
	BLE_PIN_TEXT
} TextType;

#define SW_FONT 0xFF

/* A complete font package. */
typedef struct {
    FontLanguage language;
    TextType text_type;
    uint16_t font_type;
    uint16_t width;
    uint16_t height;
    CharacterImage *image;
} Font;

/* === Functions =========================================================== */
const Font *get_font(TextType text_type);
const CharacterImage *font_get_char(const Font *font, char c);

const CharacterImage *get_charactor_image(const Font *font,
                                          const char *charactor, int *next_pos,
                                          int *ch);

uint32_t font_height(const Font *font);
uint32_t font_width(const Font *font);

uint32_t calc_str_width(const Font *font, const char *str);
uint32_t calc_str_line(const Font *font, const char *str, uint16_t line_width);

#endif
