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

#include "firmware/app_layout.h"
#include "board/board.h"
#include "board/font.h"
#include "driver/display.h"
#include "firmware/fsm.h"
#include "firmware/qr_encode.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* === Private Functions =================================================== */

/*
 * layout_animate_pin() - Animate pin scramble
 *
 * INPUT
 *     - data: pointer to pin array
 *     - duration: duration of the pin scramble animation
 *     - elapsed: how long we have animating
 * OUTPUT
 *     none
 */
static void layout_animate_pin(void *data, uint32_t duration,
                               uint32_t elapsed) {
    (void)duration;
    BoxDrawableParams box_params = {{0x00, 0, 0}, 64, 256};
    DrawableParams sp;
    canvas_t *canvas = layout_get_canvas();
    char *pin = (char *)data;
    const uint8_t color_stepping[] = {PIN_MATRIX_STEP1, PIN_MATRIX_STEP2,
                                      PIN_MATRIX_STEP3, PIN_MATRIX_STEP4,
                                      PIN_MATRIX_FOREGROUND};

    const Font *pin_font = get_font(PIN_TEXT);

    /* Draw matrix */
    box_params.base.color = PIN_MATRIX_BACKGROUND;

    for (uint8_t row = 0; row < 3; row++) {
        for (uint8_t col = 0; col < 3; col++) {
            box_params.base.y = 5 + row * 19;
            box_params.base.x = PIN_LEFT_MARGIN + col * 19;
            box_params.height = 18;
            box_params.width = 18;
            draw_box_contour(canvas, &box_params, 1);
        }
    }

    /* Configure each PIN digit animation settings */
    const PINAnimationConfig pin_animation_cfg[] = {
        {SLIDE_RIGHT, 8 * PIN_SLIDE_DELAY},  // 1
        {SLIDE_UP, 7 * PIN_SLIDE_DELAY},     // 2
        {SLIDE_DOWN, 6 * PIN_SLIDE_DELAY},   // 3
        {SLIDE_LEFT, 5 * PIN_SLIDE_DELAY},   // 4
        {SLIDE_UP, 4 * PIN_SLIDE_DELAY},     // 5
        {SLIDE_RIGHT, 3 * PIN_SLIDE_DELAY},  // 6
        {SLIDE_UP, 0 * PIN_SLIDE_DELAY},     // 7
        {SLIDE_RIGHT, 1 * PIN_SLIDE_DELAY},  // 8
        {SLIDE_DOWN, 2 * PIN_SLIDE_DELAY}    // 9
    };

    /* Draw each pin digit individually base on animation config on matrix
     * position */
    for (uint8_t row = 0; row < 3; row++) {
        for (uint8_t col = 0; col < 3; col++) {
            uint8_t cur_pos = col + (2 - row) * 3;
            const PINAnimationConfig *cur_pos_cfg = &pin_animation_cfg[cur_pos];
            uint32_t cur_pos_elapsed = elapsed - cur_pos_cfg->elapsed_start_ms;

            /* Skip position is enough time has not passed */
            if (cur_pos_cfg->elapsed_start_ms > elapsed) {
                continue;
            }

            /* Determine color */
            sp.color = PIN_MATRIX_FOREGROUND;

            for (uint8_t color_index = 0;
                 color_index <
                 sizeof(color_stepping) / sizeof(color_stepping[0]);
                 color_index++) {
                if (cur_pos_elapsed <
                    (color_index * PIN_MATRIX_ANIMATION_FREQUENCY_MS)) {
                    sp.color = color_stepping[color_index];
                    break;
                }
            }

            uint8_t pad = 7;

            /* Adjust pad */
            if (pin[cur_pos] == '1') {
                pad++;
            }

            sp.y = 8 + row * 19;
            sp.x = (PIN_LEFT_MARGIN - 2) + pad + col * 19;

            uint8_t adj_pos = cur_pos_elapsed / 40;

            if (adj_pos <= 5) {
                adj_pos = 5 - adj_pos;

                switch (cur_pos_cfg->direction) {
                    case SLIDE_DOWN:
                        sp.y -= adj_pos;
                        break;

                    case SLIDE_LEFT:
                        sp.x += adj_pos;
                        break;

                    case SLIDE_UP:
                        sp.y += adj_pos;
                        break;

                    case SLIDE_RIGHT:
                    default:
                        sp.x -= adj_pos;
                        break;
                }
            }

            draw_char(canvas, pin_font, pin[cur_pos], &sp);
        }
    }

    /* Mask horizontally */
    draw_box_simple(canvas, MATRIX_MASK_COLOR, PIN_LEFT_MARGIN - 3, 2,
                    PIN_MATRIX_GRID_SIZE * 3 + 8, MATRIX_MASK_MARGIN);
    draw_box_simple(canvas, MATRIX_MASK_COLOR, PIN_LEFT_MARGIN - 3,
                    5 + PIN_MATRIX_GRID_SIZE, PIN_MATRIX_GRID_SIZE * 3 + 8, 1);
    draw_box_simple(canvas, MATRIX_MASK_COLOR, PIN_LEFT_MARGIN - 3,
                    6 + PIN_MATRIX_GRID_SIZE * 2, PIN_MATRIX_GRID_SIZE * 3 + 8,
                    1);
    draw_box_simple(canvas, MATRIX_MASK_COLOR, PIN_LEFT_MARGIN - 3,
                    7 + PIN_MATRIX_GRID_SIZE * 3, PIN_MATRIX_GRID_SIZE * 3 + 8,
                    MATRIX_MASK_MARGIN);

    /* Mask vertically */
    draw_box_simple(canvas, MATRIX_MASK_COLOR, PIN_LEFT_MARGIN - 3, 2,
                    MATRIX_MASK_MARGIN, 18 * 3 + 8);
    draw_box_simple(canvas, MATRIX_MASK_COLOR,
                    PIN_LEFT_MARGIN + PIN_MATRIX_GRID_SIZE, 2, 1,
                    PIN_MATRIX_GRID_SIZE * 3 + 8);
    draw_box_simple(canvas, MATRIX_MASK_COLOR,
                    PIN_LEFT_MARGIN + 1 + PIN_MATRIX_GRID_SIZE * 2, 2, 1,
                    PIN_MATRIX_GRID_SIZE * 3 + 8);
    draw_box_simple(canvas, MATRIX_MASK_COLOR,
                    PIN_LEFT_MARGIN + 2 + PIN_MATRIX_GRID_SIZE * 3, 2,
                    MATRIX_MASK_MARGIN, PIN_MATRIX_GRID_SIZE * 3 + 8);
}

/*
 * layout_animate_cipher() - Animate recovery cipher
 *
 * INPUT
 *     - data: pointer to pin array
 *     - duration: duration of the pin scramble animation
 *     - elapsed: how long we have been animating
 * OUTPUT
 *     none
 */
static void layout_animate_cipher(void *data, uint32_t duration,
                                  uint32_t elapsed) {
    (void)duration;
    canvas_t *canvas = layout_get_canvas();
    int row, letter, x_padding, cur_pos_elapsed, adj_pos, adj_x, adj_y,
        cur_index;
    char *cipher = (char *)data;
    char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
    char *current_letter = alphabet;

    DrawableParams sp;
    const Font *title_font = get_font(TITLE_TEXT);
    const Font *cipher_font = get_font(BODY_TEXT);

    /* Clear area behind cipher */
    draw_box_simple(canvas, CIPHER_MASK_COLOR, CIPHER_START_X, 0,
                    DISPLAY_WIDTH - CIPHER_START_X, DISPLAY_HEIGHT);

    /* Draw grid */
    sp.y = CIPHER_START_Y;
    sp.x = CIPHER_START_X;

    for (row = 0; row < CIPHER_ROWS; row++) {
        for (letter = 0; letter < CIPHER_LETTER_BY_ROW; letter++) {
            cur_index = (row * CIPHER_LETTER_BY_ROW) + letter;
            cur_pos_elapsed =
                elapsed - cur_index * CIPHER_ANIMATION_FREQUENCY_MS;
            sp.x = CIPHER_START_X +
                   (letter * (CIPHER_GRID_SIZE + CIPHER_GRID_SPACING));
            x_padding = 0;

            /* Draw grid */
            draw_box_simple(canvas, CIPHER_STEP_1, sp.x - 4,
                            sp.y + CIPHER_GRID_SIZE, CIPHER_GRID_SIZE,
                            CIPHER_GRID_SIZE);

            x_padding = 0;

            if (*current_letter == 'i' || *current_letter == 'l') {
                x_padding = 2;
            } else if (*current_letter == 'm' || *current_letter == 'w') {
                x_padding = -1;
            }

            /* Draw map */
            draw_char_simple(canvas, title_font, *current_letter++,
                             CIPHER_MAP_FONT_COLOR, sp.x + x_padding, sp.y);

            x_padding = 0;

            if (*cipher == 'i' || *cipher == 'l') {
                x_padding = 2;
            } else if (*cipher == 'k' || *cipher == 'j' || *cipher == 'r' ||
                       *cipher == 'f') {
                x_padding = 1;
            } else if (*cipher == 'm' || *cipher == 'w') {
                x_padding = -1;
            }

            /* Draw cipher */
            if (cur_pos_elapsed > 0) {
                adj_pos = cur_pos_elapsed / CIPHER_ANIMATION_FREQUENCY_MS;

                adj_x = 0;
                adj_y = 0;

                if (adj_pos < 5) {
                    if (cur_index % 4 == 0) {
                        adj_y = -(5 - adj_pos);
                    } else if (cur_index % 4 == 1) {
                        adj_x = 5 - adj_pos;
                    } else if (cur_index % 4 == 2) {
                        adj_y = 5 - adj_pos;
                    } else {
                        adj_x = -(5 - adj_pos);
                    }
                }

                draw_char_simple(
                    canvas, cipher_font, *cipher, CIPHER_FONT_COLOR,
                    sp.x + x_padding + adj_x,
                    sp.y + (CIPHER_GRID_SIZE + CIPHER_GRID_SPACING) + adj_y);
            }

            /* Draw grid mask between boxes */
            draw_box_simple(canvas, CIPHER_MASK_COLOR, sp.x - 5,
                            sp.y + CIPHER_GRID_SIZE, 1, CIPHER_GRID_SIZE);

            cipher++;
        }

        sp.x = CIPHER_START_X;
        sp.y += 31;
    }

    /* Draw mask */
    draw_box_simple(canvas, CIPHER_MASK_COLOR, CIPHER_START_X - 4, 14,
                    CIPHER_HORIZONTAL_MASK_WIDTH,
                    CIPHER_HORIZONTAL_MASK_HEIGHT_2);
    draw_box_simple(canvas, CIPHER_MASK_COLOR, CIPHER_START_X - 4, 45,
                    CIPHER_HORIZONTAL_MASK_WIDTH,
                    CIPHER_HORIZONTAL_MASK_HEIGHT_2);
    draw_box_simple(canvas, CIPHER_MASK_COLOR, CIPHER_START_X - 4, 29,
                    CIPHER_HORIZONTAL_MASK_WIDTH,
                    CIPHER_HORIZONTAL_MASK_HEIGHT_3);
    draw_box_simple(canvas, CIPHER_MASK_COLOR, CIPHER_START_X - 4, 59,
                    CIPHER_HORIZONTAL_MASK_WIDTH,
                    CIPHER_HORIZONTAL_MASK_HEIGHT_4);
    draw_box_simple(canvas, CIPHER_MASK_COLOR,
                    DISPLAY_WIDTH - CIPHER_HORIZONTAL_MASK_WIDTH_3, 0,
                    CIPHER_HORIZONTAL_MASK_WIDTH_3, DISPLAY_HEIGHT);
}

/* === Functions =========================================================== */

/*
 *  layout_screensaver() - Displays screensaver
 *
 *  INPUT
 *      none
 *  OUTPUT
 *      none
 */
void layout_screensaver(void) {
    //     draw_box_simple(layout_get_canvas(), 0x00, 0, 0,
    //                     DISPLAY_WIDTH,
    //                     DISPLAY_HEIGHT);
    //
    //     layout_add_animation(
    //         &layout_animate_images,
    //         (void *)variant_getScreensaver(),
    //         0);
}

/*
 * layout_notification_no_title() - Display notification without title
 *
 * INPUT
 *     - title
 *     - body
 * OUTPUT
 *     none
 */
void layout_notification_no_title(const char *title, const char *body,
                                  NotificationType type, bool bold) {
    (void)title;
    layout_clear();

    canvas_t *canvas = layout_get_canvas();
    DrawableParams sp;
    const Font *font = get_font(TITLE_TEXT);

    if (!bold) {
        font = get_font(BODY_TEXT);
    }

    /* Determine vertical alignment and body width */
    sp.y = TOP_MARGIN + BODY_TOP_MARGIN;

    /* Draw */
    sp.x = LEFT_MARGIN;
    sp.color = TITLE_COLOR;
    draw_string(canvas, font, body, &sp, BODY_WIDTH, font_height(font));

    layout_notification_icon(type, &sp);
}

/*
 * layout_notification_no_title_bold() - Display notification without title in
 * bold
 *
 * INPUT
 *     - title
 *     - body
 * OUTPUT
 *     none
 */
void layout_notification_no_title_bold(const char *title, const char *body,
                                       NotificationType type) {
    layout_notification_no_title(title, body, type, true);
}

/*
 * layout_notification_no_title_no_bold() - Display notification without title
 * without bold
 *
 * INPUT
 *     - title
 *     - body
 * OUTPUT
 *     none
 */
void layout_notification_no_title_no_bold(const char *title, const char *body,
                                          NotificationType type) {
    layout_notification_no_title(title, body, type, false);
}

/*
 * layout_xpub_notification() - Display extended public address (xpub)
 * notification
 *
 * INPUT
 *     - xpub: address to display both as string
 *     - type: notification type
 * OUTPUT
 *      none
 */
void layout_xpub_notification(const char *desc, const char *xpub,
                              NotificationType type) {
    (void)desc;
    layout_clear();

    canvas_t *canvas = layout_get_canvas();
    DrawableParams sp;
    const Font *xpub_font = get_font(BODY_TEXT);

    if (strcmp(desc, "") != 0) {
        const Font *title_font = get_font(TITLE_TEXT);
        sp.y = TOP_MARGIN;
        sp.x = LEFT_MARGIN;
        sp.color = BODY_COLOR;
        draw_string(canvas, title_font, desc, &sp, BODY_WIDTH,
                    font_height(title_font) + BODY_FONT_LINE_PADDING);
    }

    /* Determine vertical alignment and body width */
    sp.y = TOP_MARGIN + ADDRESS_XPUB_TOP_MARGIN;
    sp.x = LEFT_MARGIN;
    sp.color = BODY_COLOR;
    draw_string(canvas, xpub_font, xpub, &sp, BODY_WIDTH,
                font_height(xpub_font) + BODY_FONT_LINE_PADDING);

    layout_notification_icon(type, &sp);
}

/*
 * layout_ethereum_address_notification() - Display ethereum address
 * notification
 *
 * INPUT
 *     - desc: description of address being shown (normal or multisig)
 *     - address: ethereum address to display both as string and QR
 *     - type: notification type
 * OUTPUT
 *      none
 */
void layout_ethereum_address_notification(const char *desc, const char *address,
                                          NotificationType type) {
    (void)desc;
    DrawableParams sp;
    const Font *address_font = get_font(ADDR_TEXT);
    canvas_t *canvas = layout_get_canvas();

    layout_clear();

    /* Body */
    sp.y = TOP_MARGIN + BODY_TOP_MARGIN;
    sp.x = LEFT_MARGIN + 65;
    sp.color = BODY_COLOR;

    draw_string(canvas, address_font, address, &sp, 140,
                font_height(address_font) + BODY_FONT_LINE_PADDING);

    layout_address(address);
    layout_notification_icon(type, &sp);
}

/*
 * layout_address_notification() - Display address notification
 *
 * INPUT
 *     - desc: description of address being shown (normal or multisig)
 *     - address: address to display both as string and QR
 *     - type: notification type
 * OUTPUT
 *      none
 */
void layout_address_notification(const char *desc, const char *address,
                                 NotificationType type) {
    layout_clear();

    canvas_t *canvas = layout_get_canvas();
    DrawableParams sp;
    const Font *address_font = get_font(ADDR_TEXT);

    /* Determine vertical alignment and body width */
    sp.y = TOP_MARGIN + BODY_TOP_MARGIN;

    /* Draw description */
    if (strcmp(desc, "") != 0) {
        sp.x = LEFT_MARGIN + QR_DISPLAY_WIDTH;
        sp.color = BODY_COLOR;
        draw_string(canvas, address_font, desc, &sp,
                    BODY_WIDTH - QR_DISPLAY_WIDTH,
                    font_height(address_font) + BODY_FONT_LINE_PADDING);
    }

    /* Draw address */
    sp.y += font_height(address_font) + BODY_TOP_MARGIN;
    sp.x = LEFT_MARGIN + QR_DISPLAY_WIDTH;
    sp.color = BODY_COLOR;
    draw_string(canvas, address_font, address, &sp,
                BODY_WIDTH - QR_DISPLAY_WIDTH,
                font_height(address_font) + BODY_FONT_LINE_PADDING);

    layout_address(address);
    layout_notification_icon(type, &sp);
}

/*
 * layout_pin() - Draws pin matrix
 *
 * INPUT
 *     - str: string prompt to display next to pin matrix
 *     - pin: randomized pin matric
 * OUTPUT
 *     none
 */
void layout_pin(const char *str, char pin[]) {
    DrawableParams sp;
    canvas_t *canvas = layout_get_canvas();

    layout_clear();

    /* Draw prompt */
    const Font *font = get_font(BODY_TEXT);
    sp.y = 29 + TOP_MARGIN;
    sp.x = (140 - calc_str_width(font, str)) / 2;
    sp.color = BODY_COLOR;
    draw_string(canvas, font, str, &sp, TITLE_WIDTH, font_height(font));

    const Font *pin_font = get_font(PIN_TEXT);
    /* Draw matrix */

    BoxDrawableParams box_params = {{0x00, 0, 0}, 64, 256};
    box_params.base.color = PIN_MATRIX_BACKGROUND;
    const static int pin_box_size = 22;

    for (uint8_t row = 0; row < 3; row++) {
        for (uint8_t col = 0; col < 3; col++) {
            box_params.base.y = 5 + TOP_MARGIN + row * pin_box_size;
            box_params.base.x = PIN_LEFT_MARGIN + col * pin_box_size;
            box_params.height = pin_box_size - 1;
            box_params.width = pin_box_size - 1;
            draw_box_contour(canvas, &box_params, 1);

            uint8_t cur_pos = col + (2 - row) * 3;

            uint8_t pad = 8;
            /* Adjust pad */
            if (pin[cur_pos] == '1') {
                pad = 8;
            }

            sp.y = 7 + TOP_MARGIN + row * pin_box_size;
            sp.x = (PIN_LEFT_MARGIN - 2) + pad + col * pin_box_size;
            draw_char(canvas, pin_font, pin[cur_pos], &sp);
        }
    }
    layout_commit(false);
}

/*
 * layout_cipher() - Draws recover cipher
 *
 * INPUT
 *     - current_word: current word that is being typed in at this point in
 * recovery
 *     - cipher: randomized cipher
 * OUTPUT
 *     none
 */
void layout_cipher(const char *current_word, const char *cipher) {
    DrawableParams sp;
    const Font *title_font = get_font(BODY_TEXT);
    canvas_t *canvas = layout_get_canvas();

    layout_clear();

    /* Draw prompt */
    sp.y = 11;
    sp.x = 4;
    sp.color = BODY_COLOR;
    draw_string(canvas, title_font, "Recovery Cipher:", &sp, 58,
                font_height(title_font) + 3);

    /* Draw current word */
    sp.y = 46;
    sp.x = 4;
    sp.color = BODY_COLOR;
    draw_string(canvas, title_font, current_word, &sp, 68,
                font_height(title_font));
    layout_commit(false);

    /* Animate cipher */
    layout_add_animation(&layout_animate_cipher, (void *)cipher,
                         CIPHER_ANIMATION_FREQUENCY_MS * 30);
}

/*
 * layout_address() - Draws QR code of address
 *
 * INPUT
 *     - address: address to QR code for
 * OUTPUT
 *     none
 */
void layout_address(const char *address) {
    static unsigned char bitdata[QR_MAX_BITDATA];
    canvas_t *canvas = layout_get_canvas();

    int a, i, j, side;

    side = qr_encode(QR_LEVEL_M, QR_DISPLAY_VERSION, address, 0, bitdata);

    /* Limit QR to version 1-9 (QR size <= 53) */
    if (side > 0 && side <= 53) {
        /* Draw QR background */
        draw_box_simple(canvas, 0x00, QR_DISPLAY_X, QR_DISPLAY_Y,
                        (side + 2) * QR_DISPLAY_SCALE,
                        (side + 2) * QR_DISPLAY_SCALE);

        /* Fill in QR */
        for (i = 0; i < side; i++) {
            for (j = 0; j < side; j++) {
                a = j * side + i;

                if (bitdata[a / 8] & (1 << (7 - a % 8))) {
                    draw_box_simple(canvas, 0xFF,
                                    (i * QR_DISPLAY_SCALE) + QR_DISPLAY_X,
                                    (j * QR_DISPLAY_SCALE) + QR_DISPLAY_Y,
                                    QR_DISPLAY_SCALE, QR_DISPLAY_SCALE);
                }
            }
        }
    }
}
