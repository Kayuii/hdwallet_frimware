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

#ifndef __DRAW_H__
#define __DRAW_H__

/* === Includes ============================================================ */

#include <stddef.h>
#include <stdbool.h>

#include "canvas.h"
#include "font.h"

/* === Typedefs ============================================================ */
typedef struct Image_ {
    uint16_t w;
    uint16_t h;
    uint32_t length;
    const uint8_t *data;
} Image;

typedef struct AnimationFrame_ {
    uint16_t x;
    uint16_t y;
    uint16_t duration;
    const Image *image;
} AnimationFrame;

typedef struct
{
    uint8_t color;
    uint16_t     x;
    uint16_t     y;
} DrawableParams;

typedef struct
{
    DrawableParams   base;
    uint16_t         height;
    uint16_t         width;
} BoxDrawableParams;

/* === Functions =========================================================== */

bool draw_char_with_shift(canvas_t *canvas, DrawableParams *p,
                          uint16_t *x_shift, uint16_t *y_shift,
                          const CharacterImage *img);

void draw_string(canvas_t *canvas, const Font *font, const char *c,
                 DrawableParams *p, uint16_t width, uint16_t line_height);

void draw_char(canvas_t *canvas, const Font *font, char c, DrawableParams *p);

void draw_char_simple(canvas_t *canvas, const Font *font,
                      char c, uint8_t color, uint16_t x,
                      uint16_t y);

void draw_box(canvas_t *canvas, BoxDrawableParams  *params);

void draw_box_simple(canvas_t *canvas, uint8_t color,
                     uint16_t x, uint16_t y,
                     uint16_t width, uint16_t height);

void draw_box_contour_simple(canvas_t *canvas, uint8_t color, uint16_t x,
                             uint16_t y, uint16_t width, uint16_t height,
                             uint8_t thickness);

void draw_box_contour(canvas_t *canvas, BoxDrawableParams *params,  uint8_t thickness);

bool draw_bitmap_mono_rle(canvas_t *canvas, const AnimationFrame *frame);

bool draw_bitmap_mono_bit(canvas_t *canvas, const AnimationFrame *frame);

#endif

