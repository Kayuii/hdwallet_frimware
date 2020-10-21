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

#include "board/draw.h"
#include "board/canvas.h"
#include "board/font.h"
#include "driver/display.h"
#include "firmware/fsm.h"
#include "firmware/utf8.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

#define BITS_MASK_SHIFT(bits) (0x80 >> (bits))
#define HEIGHT_COL_BYTES(row) (((row) + 7) / 8)
#define BYTES_SHIFT(x) ((x) / 8)
#define BITS_SHIFT(x) ((x) % 8)
#define BITS_PER_BYTE (8)

/*
 * draw_char_with_shift() - Draw image on display with left/top margins
 *
 * INPUT
 *     - canvas: canvas
 *     - p: pointer to Margins and text color
 *     - x_shift: left margin
 *     - y_shift: top margin
 *     - img: pointer to image drawn on the screen
 * OUTPUT
 *      true/false whether image was drawn
 */
bool draw_char_with_shift(canvas_t *canvas, DrawableParams *p, uint16_t *x_shift,
                          uint16_t *y_shift, const CharacterImage *img) 
{
    bool ret_stat = false;

    uint16_t start_index =
        BYTES_SHIFT(p->y) + p->x * HEIGHT_COL_BYTES(canvas->height);

    /* Check start_index, p->x, p->y are within bounds */
    if (start_index >= (HEIGHT_COL_BYTES(canvas->height) * DISPLAY_WIDTH)) {
        NRF_LOG_ERROR("draw char position fault:(%d, %d), start_index:%d", p->x,
                      p->y, start_index);
        return false;
    }

    uint8_t *canvas_end =
        canvas->buffer + canvas->width * HEIGHT_COL_BYTES(canvas->height);

    /* Check that this was a character that we have in the font */
    if (img != NULL) {
        uint16_t start_row = p->y;
        uint16_t end_row = start_row + img->height;

        uint16_t start_col = p->x;
        uint16_t end_col = p->x + img->width;
        /* Check that it's within bounds. */
        if ((end_col <= canvas->width) && (end_row <= canvas->height)) {
            for (int x = start_col; x < end_col; x++) {
                uint8_t *canvas_col_start =
                    canvas->buffer + (x)*HEIGHT_COL_BYTES(canvas->height);
                uint8_t *img_col_start =
                    img->data + (x - start_col) * HEIGHT_COL_BYTES(img->height);

                for (int y = start_row; y < end_row; y++) {
                    uint8_t *canvas_cur_byte =
                        canvas_col_start + BYTES_SHIFT(y);
                    if (canvas_cur_byte >= canvas_end) {
                        NRF_LOG_ERROR("draw char position fault:(%d, %d)",
                                      canvas_cur_byte, canvas_end);
                        return false;  // defensive bounds check
                    }
                    uint8_t img_byte_val =
                        *(img_col_start + BYTES_SHIFT(y - start_row));

                    uint8_t canvas_cur_bit = BITS_SHIFT(y);
                    uint8_t canvas_pix_mask = BITS_MASK_SHIFT(canvas_cur_bit);

                    uint8_t img_cur_bit = BITS_SHIFT(y - start_row);
                    uint8_t img_pix_mask = BITS_MASK_SHIFT(img_cur_bit);

                    if (img_byte_val & img_pix_mask) {
                        *canvas_cur_byte |= canvas_pix_mask;
                    } else {
                        *canvas_cur_byte &= ~canvas_pix_mask;
                    }
                }
            }

            if (x_shift != NULL) {
                *x_shift += img->width;
            }

            if (y_shift != NULL) {
                *y_shift += img->height;
            }

            ret_stat = true;
        }
        canvas->dirty = true;
    }

    return (ret_stat);
}

/*
 * draw_string() - Draw string with provided font
 *
 * INPUT
 *     - canvas: canvas
 *     - font: pointer to font size
 *     - str_write: pointer to string to shown on display
 *     - p: pointer to Margins and text color
 *     - width: row width allocated for drawing
 *     - line_height: offset from top of screen
 * OUTPUT
 *     none
 */
void draw_string(canvas_t *canvas, const Font *font, const char *str_write,
                 DrawableParams *p, uint16_t width, uint16_t line_height) {
    bool have_space = true;
    uint16_t x_offset = 0;
    DrawableParams char_params = *p;

    int next_pos = 0;
    int ch = 0;
    while (*str_write && have_space) {
        next_pos = 0;
        const CharacterImage *img =
            get_charactor_image(font, str_write, &next_pos, &ch);
        uint16_t word_width = img->width;
        char *next_c = (char *)str_write + next_pos;

        /* Allow line breaks */
        if (*str_write == '\n') {
            char_params.y += line_height;
            x_offset = 0;
            str_write += next_pos;
            continue;
        }

        /*
         * Calculate the next word width while
         * removing spacings at beginning of lines
         */
        if (*str_write == ' ') {
            while (*next_c && *next_c != ' ' && *next_c != '\n') {
                word_width += font_width(font);
                next_c++;
            }
        }

        /* Determine if we need a line break */
        if ((width != 0) && (width <= canvas->width) &&
            (x_offset + word_width > width)) {
            char_params.y += line_height;
            x_offset = 0;
        }

        /* Remove spaces from beginning of of line */
        if (x_offset == 0 && *str_write == ' ') {
            str_write++;
            continue;
        }

        /* Draw Character */
        char_params.x = x_offset + p->x;
        have_space =
            draw_char_with_shift(canvas, &char_params, &x_offset, NULL, img);
        str_write += next_pos;
    }

    canvas->dirty = true;
}

/*
 * draw_char() - Draw a single character to the display
 *
 * INPUT
 *     - canvas: canvas
 *     - font: font to use for drawing
 *     - c: character to draw
 *     - p: loccation of character placement
 * OUTPUT
 *     none
 */
void draw_char(canvas_t *canvas, const Font *font, char c, DrawableParams *p) {
    char charactor = c;
    const CharacterImage *img = get_charactor_image(font, &charactor, NULL, NULL);
    uint16_t x_offset = 0;

    /* Draw Character */
    draw_char_with_shift(canvas, p, &x_offset, NULL, img);

    canvas->dirty = true;
}

/*
 * draw_char_simple() - Draw a single character to the display
 * without having to create box param object
 *
 * INPUT
 *     - canvas: canvas
 *     - font: font to use for drawing
 *     - c: character to draw
 *     - color: color of character
 *     - x: x position
 *     - y: y position
 * OUTPUT
 *     none
 */
void draw_char_simple(canvas_t *canvas, const Font *font, char c, uint8_t color,
                      uint16_t x, uint16_t y) {
    DrawableParams p;
    p.color = color;
    p.x = x;
    p.y = y;
    draw_char(canvas, font, c, &p);
}

/*
 * draw_box() - Draw box on display
 *
 * INPUT
 *     - canvas: canvas
 *     - p: pointer to Margins and text color
 * OUTPUT
 *     none
 */
void draw_box(canvas_t *canvas, BoxDrawableParams *p) {
    uint16_t start_row = p->base.y;
    uint16_t end_row = start_row + p->height;
    end_row = (end_row > canvas->height) ? canvas->height : end_row;

    uint16_t start_col = p->base.x;
    uint16_t end_col = p->base.x + p->width;
    end_col = (end_col > canvas->width) ? canvas->width : end_col;

    uint8_t *canvas_end =
        canvas->buffer + canvas->width * HEIGHT_COL_BYTES(canvas->height);

    for (uint16_t x = start_col; x < end_col; x++) {
        uint8_t *canvas_col_start =
            canvas->buffer + HEIGHT_COL_BYTES(canvas->height) * x;

        for (uint16_t y = start_row; y < end_row; y++) {
            uint8_t *canvas_cur_byte = canvas_col_start + BYTES_SHIFT(y);
            if (canvas_cur_byte >= canvas_end) {
                return;  // defensive bounds check
            }

            uint8_t canvas_cur_bit = BITS_SHIFT(y);

            if (p->base.color != 0) {
                *canvas_cur_byte |= (BITS_MASK_SHIFT(canvas_cur_bit));
            } else {
                *canvas_cur_byte &= ~(BITS_MASK_SHIFT(canvas_cur_bit));
            }
        }
    }
    canvas->dirty = true;
}

/*
 * draw_box_simple() - Draw box without having to create box param object
 *
 * INPUT
 *     canvas: canvas
 *     color: color of box
 *     x: x position
 *     y: y position
 *     width: width of box
 *     height: height of box
 * OUTPUT
 *     none
 */
void draw_box_simple(canvas_t *canvas, uint8_t color, uint16_t x, uint16_t y,
                     uint16_t width, uint16_t height) {
    BoxDrawableParams box_params = {{color, x, y}, height, width};
    draw_box(canvas, &box_params);
}

void draw_box_contour_simple(canvas_t *canvas, uint8_t color, uint16_t x,
                             uint16_t y, uint16_t width, uint16_t height,
                             uint8_t thickness) {
    draw_box_simple(canvas, color, x, y, width, height);
    if (color == 0) {
        draw_box_simple(canvas, 255, x + thickness, y + thickness,
                        width - 2 * thickness, height - 2 * thickness);
    } else {
        draw_box_simple(canvas, 0, x + thickness, y + thickness,
                        width - 2 * thickness, height - 2 * thickness);
    }
}

void draw_box_contour(canvas_t *canvas, BoxDrawableParams *params,
                      uint8_t thickness) {
    uint8_t color = params->base.color;
    uint16_t x = params->base.x;
    uint16_t y = params->base.y;
    uint16_t width = params->width;
    uint16_t height = params->height;
    draw_box_contour_simple(canvas, color, x, y, width, height, thickness);
}

/*
 * draw_bitmap_mono_rle() - Draw image
 *
 * INPUT
 *     - canvas: canvas
 *     - frame: pointer to animation frame
 * OUTPUT
 *     true/false whether image was drawn
 */
bool draw_bitmap_mono_rle(canvas_t *canvas, const AnimationFrame *frame)
{
    if (frame == NULL) {
        return false;
    }

    const Image *img = frame->image;
    if (img == NULL) {
        return false;
    }
    /* Check that image will fit in bounds */
    if (((img->w + frame->x) > canvas->width) ||
        ((img->h + frame->y) > canvas->height)) {
        return false;
    }

    uint16_t start_index =
        BYTES_SHIFT(frame->y) + frame->x * HEIGHT_COL_BYTES(canvas->height);

    /* Check start_index, p->x, p->y are within bounds */
    if (start_index >= (HEIGHT_COL_BYTES(canvas->height) * DISPLAY_WIDTH)) {
        return false;
    }

    uint8_t *canvas_end =
        canvas->buffer + canvas->width * HEIGHT_COL_BYTES(canvas->height);

    /* Check that this was a character that we have in the font */
    if (img != NULL) {
        uint16_t start_row = frame->y;
        uint16_t end_row = start_row + img->h;

        uint16_t start_col = frame->x;
        uint16_t end_col = frame->x + img->w;
        /* Check that it's within bounds. */
        if ((end_col <= canvas->width) && (end_row <= canvas->height)) {
            for (int x = start_col; x < end_col; x++) {
                uint8_t *canvas_col_start =
                    canvas->buffer + (x)*HEIGHT_COL_BYTES(canvas->height);
                for (int y = start_row; y < end_row; y++) {
                    uint8_t *canvas_cur_byte =
                        canvas_col_start + BYTES_SHIFT(y);
                    if (canvas_cur_byte >= canvas_end) {
                        return false;  // defensive bounds check
                    }
                    int canvas_cur_bit = BITS_SHIFT(y);

                    uint8_t pix_val =
                        img->data[(y - frame->y) * img->w + (x - frame->x)];

                    uint8_t pix_mask = (BITS_MASK_SHIFT(canvas_cur_bit));
                    if (0 == pix_val) {
                        *canvas_cur_byte |= pix_mask;
                    } else {
                        *canvas_cur_byte &= ~pix_mask;
                    }
                }
            }
        }
    }

    canvas->dirty = true;
    return true;
}

bool draw_bitmap_mono_bit(canvas_t *canvas, const AnimationFrame *frame) 
{
    if (frame == NULL) {
        return false;
    }

    const Image *img = frame->image;
    if (img == NULL) {
        return false;
    }
    /* Check that image will fit in bounds */
    if (((img->w + frame->x) > canvas->width) ||
        ((img->h + frame->y) > canvas->height)) {
        return false;
    }

    uint16_t start_index =
        BYTES_SHIFT(frame->y) + frame->x * HEIGHT_COL_BYTES(canvas->height);

    /* Check start_index, p->x, p->y are within bounds */
    if (start_index >= (HEIGHT_COL_BYTES(canvas->height) * DISPLAY_WIDTH)) {
        return false;
    }

    uint8_t *canvas_end =
        canvas->buffer + canvas->width * HEIGHT_COL_BYTES(canvas->height);

    /* Check that this was a character that we have in the font */
    if (img != NULL) {
        uint16_t start_row = frame->y;
        uint16_t end_row = start_row + img->h;

        uint16_t start_col = frame->x;
        uint16_t end_col = frame->x + img->w;
        /* Check that it's within bounds. */
        if ((end_col <= canvas->width) && (end_row <= canvas->height)) {
            for (int x = start_col; x < end_col; x++) {
                uint8_t *canvas_col_start =
                    canvas->buffer + (x)*HEIGHT_COL_BYTES(canvas->height);
                const uint8_t *img_col_start =
                    img->data + (x - start_col) * HEIGHT_COL_BYTES(img->h);

                for (int y = start_row; y < end_row; y++) {
                    uint8_t *canvas_cur_byte =
                        canvas_col_start + BYTES_SHIFT(y);
                    if (canvas_cur_byte >= canvas_end) {
                        return false;  // defensive bounds check
                    }
                    uint8_t canvas_cur_bit = BITS_SHIFT(y);
                    uint8_t canvas_pix_mask = BITS_MASK_SHIFT(canvas_cur_bit);
                    
                    uint8_t img_byte_val =
                        *(img_col_start + BYTES_SHIFT(y - start_row));
                    uint8_t img_cur_bit = BITS_SHIFT(y - start_row);
                    uint8_t img_pix_mask = BITS_MASK_SHIFT(img_cur_bit);

                    if (img_byte_val & img_pix_mask) {
                        *canvas_cur_byte |= canvas_pix_mask;
                    } else {
                        *canvas_cur_byte &= ~canvas_pix_mask;
                    }
                }
            }
        }
    }

    canvas->dirty = true;
    return true;
}

