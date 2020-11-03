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

#include "board/layout.h"
#include "board/board.h"
#include "board/canvas.h"
#include "board/draw.h"
#include "board/font.h"
#include "board/resources.h"
#include "driver/battery.h"
#include "driver/button.h"
#include "driver/display.h"
#include "driver/soter_ble.h"
#include "firmware/fsm.h"
#include "firmware/storage.h"

#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static AnimationQueue m_active_queue = {NULL, 0};
static AnimationQueue m_free_queue = {NULL, 0};
static Animation m_animations[MAX_ANIMATIONS];
static canvas_t *m_canvas = NULL;
static uint32_t layout_home_tick = 0;

extern uint8_t g_product_id[10];

static char info[MEDIUM_STR_BUF];

extern uint8_t g_new_home_status;

#ifdef SES
void strupr(char *str) {
    for (; *str; str++) *str = toupper(*str);
}
#endif

static void handle_screen_press(void *context) {}

static void handle_screen_release(void *context) {
    uint8_t *home_status = (uint8_t *)context;
    *home_status = LAYOUT_HOME;
}

/// User has held down the push button for duration as requested.
/// \param context current state context.
static void handle_confirm_timeout(void *context) {
    uint8_t *home_status = (uint8_t  *)context;
    switch (*home_status) {
        case LAYOUT_HOME: {
            *home_status = LAYOUT_INFO;
        } break;
        case LAYOUT_INFO: {
            *home_status = LAYOUT_HOME;
        } break;
        default: {
			*home_status = LAYOUT_HOME;
        } break;
    }
}

/*
 *  layout_home_helper() - Splash home screen helper
 *
 *  INPUT
 *      - reversed: true/false whether splash animation is played in reverse
 *  OUTPUT
 *      none
 */
static void layout_home_helper(bool flush) {
    DrawableParams sp;
    layout_clear();
	if (!flush)
	{
        const Font *id_font = get_font(BODY_TEXT);
        sp.y = PRODUCT_ID_TOP;
        sp.x = PRODUCT_ID_LEFT;
        sp.color = 0xff;
        draw_string(m_canvas, id_font, g_product_id, &sp, TITLE_WIDTH,
                    font_height(id_font));
	}
    const AnimationFrame *logo = get_logo_frame();
    draw_bitmap_mono_bit(m_canvas, logo);
    layout_commit(flush);
    button_set_confirm_on_longpush_handler(handle_confirm_timeout,
                                           &g_new_home_status);
    button_set_confirm_on_release_handler(handle_screen_release,
                                          &g_new_home_status);
    layout_home_tick = xTaskGetTickCount();
}

/*
 * animation_queue_peek() - Get current animation node in head pointer
 *
 * INPUT
 *     - queue: pointer to animation queue
 * OUTPUT
 *     node pointed to by head pointer
 */
static Animation *animation_queue_peek(AnimationQueue *queue) {
    return queue->head;
}

/*
 * animation_queue_push() - Push animation into queue
 *
 * INPUT
 *     - queue: pointer to animation queue
 * OUTPUT
 *     none
 */
static void animation_queue_push(AnimationQueue *queue, Animation *node) {
    if (queue->head != NULL) {
        node->next = queue->head;
    } else {
        node->next = NULL;
    }

    queue->head = node;
    queue->size += 1;
}

/*
 * animation_queue_pop() - Pop a node from animation queue
 *
 * INPUT
 *     - queue: pointer to animation queue
 * OUTPUT
 *     pointer to a node from the queue
 */
static Animation *animation_queue_pop(AnimationQueue *queue) {
    Animation *animation = queue->head;

    if (animation != NULL) {
        queue->head = animation->next;
        queue->size -= 1;
    }

    return (animation);
}

/*
 * animation_queue_get() - Get a queue containg the callback function
 *
 * INPUT
 *     - queue: pointer to animation queue
 *     - callback: animation callback function
 * OUTPUT
 *     pointer to Animation node
 */
static Animation *animation_queue_get(AnimationQueue *queue,
                                      AnimateCallback callback) {
    Animation *current = queue->head;
    Animation *result = NULL;

    if (current != NULL) {
        if (current->animate_callback == callback) {
            result = current;
            queue->head = current->next;
        } else {
            Animation *previous = current;
            current = current->next;

            while ((current != NULL) && (result == NULL)) {
                // Found the node!

                NRF_LOG_DEBUG("animation_queue_get while");
                if (current->animate_callback == callback) {
                    result = current;
                    previous->next = current->next;
                    result->next = NULL;
                }

                previous = current;
                current = current->next;
            }
        }
    }

    if (result != NULL) {
        queue->size -= 1;
    }

    return (result);
}

/* === Functions =========================================================== */

/*
 * layout_init() - Initialize layout subsystem
 *
 * INPUT
 *     - new_canvas: lay out info for specific image
 * OUTPUT
 *     none
 */
void layout_init(canvas_t *new_canvas) {
    m_canvas = new_canvas;

    int i;
#ifdef DEBUG
    draw_box_contour_simple(m_canvas, 0xff, 0, 0, 250, 122, 1);
#endif

    for (i = 0; i < MAX_ANIMATIONS; i++) {
        animation_queue_push(&m_free_queue, &m_animations[i]);
    }
}

/*
 * layout_get_canvas() - Returns canvas for drawing to display
 *
 * INPUT
 *     none
 * OUTPUT
 *     pointer to canvas
 */
canvas_t *layout_get_canvas(void) { return m_canvas; }

/*
 * layout_standard_notification() - Display standard notification
 *
 * INPUT
 *     - str1: title string
 *     - str2: body string
 *     - type: notification type
 * OUTPUT
 *     none
 */
void layout_standard_notification(const char *str1, const char *str2,
                                  NotificationType type) {
    layout_clear();

    DrawableParams sp;
    const Font *title_font = get_font(TITLE_TEXT);
    const Font *body_font = get_font(BODY_TEXT);
    const uint32_t body_line_count = calc_str_line(body_font, str2, BODY_WIDTH);

    /* Determine vertical alignment and body width */
    sp.y = TOP_MARGIN;

    /* Format Title */
    char upper_str1[TITLE_CHAR_MAX];
    strlcpy(upper_str1, str1, TITLE_CHAR_MAX);
    strupr(upper_str1);

    /* Title */
    sp.x = LEFT_MARGIN;
    sp.color = TITLE_COLOR;
    draw_string(m_canvas, title_font, upper_str1, &sp, TITLE_WIDTH,
                font_height(title_font));

    /* Body */
    sp.y += font_height(title_font) + BODY_TOP_MARGIN;
    sp.x = LEFT_MARGIN;
    sp.color = BODY_COLOR;
    draw_string(m_canvas, body_font, str2, &sp, BODY_WIDTH,
                font_height(body_font) + BODY_FONT_LINE_PADDING);

    layout_notification_icon(type, &sp);
    if (type != NOTIFICATION_CONFIRM_ANIMATION) {
        layout_commit(false);
    }
}

void layout_ble_pairing_code(const char *code) {
    layout_clear();

    DrawableParams sp;
    const Font *title_font = get_font(TITLE_TEXT);
    const Font *pin_font = get_font(BLE_PIN_TEXT);

    /* Title */
    sp.color = TITLE_COLOR;
    const char *ble_pairing_title = "BLE PAIRING PIN";
    uint32_t str_width = calc_str_width(title_font, ble_pairing_title);
    sp.x = (DISPLAY_WIDTH - str_width) / 2;
    sp.y = TOP_MARGIN;
    draw_string(m_canvas, title_font, ble_pairing_title, &sp, TITLE_WIDTH,
                font_height(title_font));

    /* PIN-Code */
    str_width = calc_str_width(pin_font, code);
    sp.y += font_height(title_font) + BODY_TOP_MARGIN + 5;
    sp.x = (DISPLAY_WIDTH - str_width) / 2;
    sp.color = BODY_COLOR;
    draw_string(m_canvas, pin_font, code, &sp, DISPLAY_WIDTH,
                font_height(pin_font));

    layout_commit(false);
}

/*
 * layout_notification_icon() - Display notification icon
 *
 * INPUT
 *     - type: notification type
 *     - sp: drawable parameters for icon notification placement
 * OUTPUT
 *     none
 */
void layout_notification_icon(NotificationType type, DrawableParams *sp) {
    switch (type) {
        case NOTIFICATION_REQUEST:
        case NOTIFICATION_REQUEST_NO_ANIMATION:
            draw_box_simple(m_canvas, 0x00, PRODUCT_ID_LEFT, PRODUCT_ID_TOP,
                            PRODUCT_ID_WIDTH, PRODUCT_ID_HEIGHT);
            draw_bitmap_mono_bit(m_canvas, get_confirm_icon_frame());
            break;

        case NOTIFICATION_CONFIRM_ANIMATION: {
            const VariantAnimation *anim = get_confirming_animation();

            layout_add_animation(&layout_animate_images, (void *)anim,
                                 get_image_animation_duration(anim));
        } break;
        case NOTIFICATION_CONFIRMED:
            draw_bitmap_mono_bit(m_canvas, get_confirmed_frame());
            break;

        case NOTIFICATION_UNPLUG:
            sp->x = 208;
            sp->y = 21;
            // draw_bitmap_mono_bit(m_canvas, get_unplug_frame(), false);
            break;

        case NOTIFICATION_RECOVERY:
            sp->x = 221;
            sp->y = 20;
            // draw_bitmap_mono_rle(m_canvas, get_recovery_frame(), false);
            break;

        case NOTIFICATION_INFO:
        default:
            /* no action requires */
            break;
    }
}

/*
 * layout_warning() - Display warning message
 *
 * INPUT
 *     - prompt: string to display
 * OUTPUT
 *     none
 */
void layout_warning(const char *str) {
    layout_clear();

    const Font *font = get_font(BODY_TEXT);

    /* Title */
    DrawableParams sp;
    sp.x = (DISPLAY_WIDTH - calc_str_width(font, str)) / 2;
    sp.y = 50;
    sp.color = TITLE_COLOR;
    draw_string(m_canvas, font, str, &sp, DISPLAY_WIDTH, font_height(font));

    //     const VariantAnimation *warning = get_warning_animation();
    //     layout_add_animation(&layout_animate_images, (void *)warning, 0);
    layout_commit(false);
}

/*
 * layout_warning_static() - Display warning message on display without
 * animation
 *
 * INPUT
 *     - prompt: string to display
 * OUTPUT
 *     none
 */
void layout_warning_static(const char *str) {
    layout_clear();

    const Font *font = get_font(BODY_TEXT);

    /* Title */
    DrawableParams sp;
    sp.x = (DISPLAY_WIDTH - calc_str_width(font, str)) / 2;
    sp.y = (DISPLAY_HEIGHT - font_height(font)) / 2;
    sp.color = TITLE_COLOR;
    draw_string(m_canvas, font, str, &sp, DISPLAY_WIDTH, font_height(font));
    layout_commit(false);
}

/*
 * layout_simple_message() - Displays a simple one line message
 *
 * INPUT
 *     - str: string to display
 * OUTPUT
 *     none
 */
void layout_simple_message(const char *str) {
    layout_clear();

    const Font *font = get_font(TITLE_TEXT);

    /* Format Message */
    char upper_str[TITLE_CHAR_MAX];
    strlcpy(upper_str, str, TITLE_CHAR_MAX);
    strupr(upper_str);

    /* Draw Message */
    DrawableParams sp;
    sp.x = (DISPLAY_WIDTH - calc_str_width(font, upper_str)) / 2;
    sp.y = (DISPLAY_HEIGHT / 2) - (font_height(font) / 2);
    sp.color = TITLE_COLOR;
    draw_string(m_canvas, font, upper_str, &sp, DISPLAY_WIDTH,
                font_height(font));
    layout_commit(false);
}

/*
 * layout_version() - Displays version
 *
 * INPUT
 *     - major: major version number
 *     - minor: minor version number
 *     - patch: patch version number
 * OUTPUT
 *     none
 */
void layout_product_info(int32_t major, int32_t minor, int32_t patch) {
    layout_clear();

    const Font *title_font = get_font(TITLE_TEXT);
    const Font *body_font = get_font(BODY_TEXT);

    /* Draw product id information */
    DrawableParams sp;
    sp.x = (DISPLAY_WIDTH - calc_str_width(title_font, g_product_id)) / 2;
    sp.y = TOP_MARGIN + BODY_TOP_MARGIN;
    sp.color = TITLE_COLOR;
    draw_string(m_canvas, title_font, g_product_id, &sp, DISPLAY_WIDTH,
                font_height(title_font));

    // Draw version infomation
    snprintf(info, MEDIUM_STR_BUF, "version: v%lu.%lu.%lu",
             (unsigned long)major, (unsigned long)minor, (unsigned long)patch);
    sp.x = (DISPLAY_WIDTH - calc_str_width(body_font, info)) / 2;
    sp.y = sp.y + font_height(title_font) + BODY_TOP_MARGIN;
    draw_string(m_canvas, body_font, info, &sp, TITLE_WIDTH,
                font_height(body_font));

    // Draw uuid information
    //const char *uuid_str = storage_getUuidStr();
    const char *uuid_str = "XX";
    if (strlen(uuid_str) > 0) {
        snprintf(info, MEDIUM_STR_BUF, "UUID: %s", uuid_str);
        sp.x = (DISPLAY_WIDTH - calc_str_width(body_font, info)) / 2;
        sp.y = sp.y + font_height(body_font) + BODY_TOP_MARGIN;
        draw_string(m_canvas, body_font, info, &sp, DISPLAY_WIDTH,
                    font_height(body_font));
    }

    layout_commit(false);
}

/*
 *  layout_home() - Splash home screen
 *
 *  INPUT
 *      none
 *  OUTPUT
 *      none
 */
void layout_home(void) { layout_home_helper(false); }

/*
 *  layout_home_reversed() - Splash home screen in reverse
 *
 *  INPUT
 *      none
 *  OUTPUT
 *      none
 */
void layout_home_flush(void) { layout_home_helper(true); }

/*
 * layout_loading() - Loading animation
 *
 * INPUT
 *     none
 * OUTPUT
 *     none
 *
 */
void layout_loading(void) {}

/*
 * animate() - Attempt to animate if there are animations in the queue
 *
 * INPUT
 *     none
 * OUTPUT
 *     none
 */
void animate(void) {
    Animation *animation = animation_queue_peek(&m_active_queue);
    while (animation != NULL) {
        Animation *next = animation->next;

        animation->elapsed += TIME_TASK_DELAY;

        animation->animate_callback(animation->data, animation->duration,
                                    animation->elapsed);

        if ((animation->duration > 0) &&
            (animation->elapsed >= animation->duration)) {
            animation_queue_push(
                &m_free_queue,
                animation_queue_get(&m_active_queue,
                                    animation->animate_callback));
        }

        animation = next;
    }
}

/*
 * is_animating() - Get animation status
 *
 * INPUT
 *     none
 * OUTPUT
 *     true/false whether there are animations in the queue
 */
bool is_animating(void) {
    return animation_queue_peek(&m_active_queue) != NULL;
}

/*
 * layout_animate_images() - Animate image on display
 *
 * INPUT
 *      TODO: remove void pointer
 *     - data: pointer to image
 *     - duration: duration of the image animation
 *     - elapsed: delay before drawing the image
 * OUTPUT
 *     none
 */
void layout_animate_images(void *data, uint32_t duration, uint32_t elapsed) {
    const VariantAnimation *animation = (const VariantAnimation *)data;

    bool looping = (duration == 0);
    int frameNum = get_image_animation_frame(animation, elapsed, looping);
    if (frameNum != -1 && frameNum < animation->count) {
        draw_bitmap_mono_bit(m_canvas, &animation->frames[frameNum]);
    }
}

/*
 * layout_clear() - Clear animation queue and clear display
 *
 * INPUT
 *     none
 * OUTPUT
 *     none
 */
void layout_clear(void) {
    layout_clear_animations();
#if (DOT_WHITE_VALUE == 1)
    draw_box_simple(m_canvas, 0x00, 5, 5, 100, TOP_MARGIN - 5);
    draw_box_simple(m_canvas, 0x00, LEFT_MARGIN, TOP_MARGIN,
                    250 - (LEFT_MARGIN + RIGHT_MARGIN),
                    122 - (TOP_MARGIN + BOTTOM_MARGIN));
#else
    // draw_box_simple(m_canvas, 0x00, 0, 0 100, TOP_MARGIN);
    draw_box_simple(m_canvas, 0x00, 0, 0, 250, 122);
#endif
}

/*
 * animating_progress_handler() - Animate storage update progress
 *
 * INPUT
 *     none
 * OUTPUT
 *     none
 */
void animating_progress_handler(void) {
    // if (is_animating()) {
    //     animate();
    //     layout_commit(false);
    // }
}

/*
 * layout_add_animation() - Queue up the animation in active_queue
 *
 * INPUT
 *     - callback: animation callback function
 *     - data: pointer to image
 *     - duration: duration of animation
 * OUTPUT
 *     none
 */
void layout_add_animation(AnimateCallback callback, void *data,
                          uint32_t duration) {
    Animation *animation = animation_queue_get(&m_active_queue, callback);

    if (animation == NULL) {
        animation = animation_queue_pop(&m_free_queue);
    }

    animation->data = data;
    animation->duration = duration;
    animation->elapsed = 0;
    animation->animate_callback = callback;
    animation_queue_push(&m_active_queue, animation);
}

/*
 * layout_clear_animations() - Clear all animation from queue
 *
 * INPUT
 *     none
 * OUTPUT
 *     none
 */
void layout_clear_animations(void) {
    Animation *animation = animation_queue_pop(&m_active_queue);

    while (animation != NULL) {
        animation_queue_push(&m_free_queue, animation);
        animation = animation_queue_pop(&m_active_queue);
    }
}

void layout_battery_icon(uint16_t percent, uint16_t charging) {
    const AnimationFrame *charging_frame = get_battery_charging_frame();
    if (charging) {
        draw_bitmap_mono_bit(m_canvas, charging_frame);
    } else {
        draw_box_simple(m_canvas, 0x00, charging_frame->x, charging_frame->y,
                        charging_frame->image->w, charging_frame->image->h);
    }
    const AnimationFrame *battery_frame = get_battery_frame(percent);
    draw_bitmap_mono_bit(m_canvas, battery_frame);
}

void layout_ble_icon(bool connected) {
    const AnimationFrame *ble_frame = get_ble_frame(connected);
    draw_bitmap_mono_bit(m_canvas, ble_frame);
}

void layout_confirm_finished(void) {
    const AnimationFrame *frame = get_confirmed_frame();
    draw_box_simple(layout_get_canvas(), 0x00, frame->x, frame->y,
                    frame->image->w, frame->image->h);
    layout_commit(false);
    vTaskDelay(220);
}

void layout_commit(uint8_t flush) {
    if (m_canvas->dirty) {
        display_refresh(flush);
        m_canvas->dirty = false;
    }
}

uint32_t layout_get_home_tick(void) { return layout_home_tick; }
