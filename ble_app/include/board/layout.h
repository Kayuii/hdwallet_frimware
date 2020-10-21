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

#ifndef __LAYOUT_H__
#define __LAYOUT_H__

/* === Includes ============================================================ */

#include "board/canvas.h"
#include "board/draw.h"

#include <stdint.h>

/* === Defines ============================================================= */

#define MAX_ANIMATIONS 5
#define ANIMATION_PERIOD 220

/* Margin */
#define TOP_MARGIN 26
#define BOTTOM_MARGIN 5
#define LEFT_MARGIN 8
#define RIGHT_MARGIN 8

/* Product Id */
#define PRODUCT_ID_TOP 9
#define PRODUCT_ID_LEFT 15
#define PRODUCT_ID_WIDTH 60
#define PRODUCT_ID_HEIGHT 10

/* Title */
#define TITLE_COLOR 0xFF
#define TITLE_WIDTH DISPLAY_WIDTH - LEFT_MARGIN - RIGHT_MARGIN
#define TITLE_ROWS 1
#define TITLE_FONT_LINE_PADDING 0
#define TITLE_CHAR_MAX 128

/* Body */
#define BODY_TOP_MARGIN 10
#define BODY_COLOR 0xFF
#define BODY_WIDTH DISPLAY_WIDTH - LEFT_MARGIN - RIGHT_MARGIN
#define BODY_ROWS 3
#define BODY_FONT_LINE_PADDING 4
#define BODY_CHAR_MAX 352

/* Warning */
#define WARNING_COLOR 0xFF
#define WARNING_ROWS 1
#define WARNING_FONT_LINE_PADDING 0

/* Default Layout */
#define NO_WIDTH 0

typedef enum {
    LAYOUT_HOME,
    LAYOUT_INFO,
}LayoutHomeStatus;

/* === Typedefs ============================================================ */

typedef enum {
    NOTIFICATION_INFO,
    NOTIFICATION_REQUEST,
    NOTIFICATION_REQUEST_NO_ANIMATION,
    NOTIFICATION_RECOVERY,
    NOTIFICATION_UNPLUG,
    NOTIFICATION_CONFIRM_ANIMATION,
    NOTIFICATION_CONFIRMED
} NotificationType;

typedef void (*AnimateCallback)(void *data, uint32_t duration,
                                uint32_t elapsed);
typedef struct Animation Animation;

struct Animation {
    uint32_t duration;
    uint32_t elapsed;
    void *data;
    AnimateCallback animate_callback;
    Animation *next;
};

typedef struct {
    Animation *head;
    int size;

} AnimationQueue;

/* === Functions =========================================================== */

void layout_init(canvas_t *canvas);
canvas_t *layout_get_canvas(void);
void layout_standard_notification(const char *str1, const char *str2,
                                  NotificationType type);
void layout_notification_icon(NotificationType type, DrawableParams *sp);
void layout_warning(const char *prompt);
void layout_warning_static(const char *str);
void layout_simple_message(const char *str);
void layout_product_info(int32_t major, int32_t minor, int32_t patch);
void layout_home(void);
void layout_home_flush(void);
void layout_ble_pairing_code(const char *code);
void layout_loading(void);
void animate(void);
bool is_animating(void);
void animating_progress_handler(void);
void layout_add_animation(AnimateCallback callback, void *data,
                          uint32_t duration);
void layout_animate_images(void *data, uint32_t duration, uint32_t elapsed);
void layout_clear(void);
void layout_clear_animations(void);
void layout_commit(uint8_t flush);
void layout_battery_icon(uint16_t percent, uint16_t charging);
void layout_ble_icon(bool connected);
void layout_confirm_finished(void);
uint32_t layout_get_home_tick(void);
#endif
