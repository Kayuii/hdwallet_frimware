/*
 * This file is part of the SoterWallet project.
 *
 * Copyright (C) 2018 DigBig LTD
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
#include "board/resources.h"
#include <string.h>

/* === Private Variables =================================================== */

/* --- Confirm Icon Animation ---------------------------------------------- */

static const uint8_t logo_data[250] = {
    0X7C, 0X78, 0XFF, 0X7C, 0XFF, 0X7C, 0XC3, 0X0C, 0XC3, 0X0C, 0XFB, 0XFC,
    0XFB, 0XFC, 0X78, 0XF8, 0X00, 0X00, 0X00, 0X00, 0X7F, 0XF8, 0XFF, 0XFC,
    0XFF, 0XFC, 0XC0, 0X0C, 0XC0, 0X0C, 0XFF, 0XFC, 0XFF, 0XFC, 0X7F, 0XF8,
    0X00, 0X00, 0X00, 0X00, 0XC0, 0X00, 0XC0, 0X00, 0XC0, 0X00, 0XFF, 0XFC,
    0XFF, 0XFC, 0XFF, 0XFC, 0XC0, 0X00, 0XC0, 0X00, 0XC0, 0X00, 0X00, 0X00,
    0X00, 0X00, 0XFF, 0XFC, 0XFF, 0XFC, 0XFF, 0XFC, 0XC3, 0X0C, 0XC3, 0X0C,
    0XC3, 0X0C, 0XC3, 0X0C, 0XC3, 0X0C, 0XC3, 0X0C, 0X00, 0X00, 0X00, 0X00,
    0XFF, 0XFC, 0XFF, 0XFC, 0XFF, 0XFC, 0XC1, 0X80, 0XC1, 0X80, 0XFF, 0XFC,
    0XFF, 0XFC, 0X7E, 0X7C, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0XFF, 0XF8, 0XFF, 0XFC, 0XFF, 0XFC, 0X00, 0X0C, 0X00, 0X0C, 0XFF, 0XFC,
    0XFF, 0XF8, 0XFF, 0XFC, 0X00, 0X0C, 0X00, 0X0C, 0XFF, 0XFC, 0XFF, 0XFC,
    0XFF, 0XF8, 0X00, 0X00, 0X00, 0X00, 0X7F, 0XFC, 0XFF, 0XFC, 0XFF, 0XFC,
    0XC1, 0X80, 0XC1, 0X80, 0XFF, 0XFC, 0XFF, 0XFC, 0X7F, 0XFC, 0X00, 0X00,
    0X00, 0X00, 0XFF, 0XFC, 0XFF, 0XFC, 0XFF, 0XFC, 0X00, 0X0C, 0X00, 0X0C,
    0X00, 0X0C, 0X00, 0X0C, 0X00, 0X0C, 0X00, 0X00, 0X00, 0X00, 0XFF, 0XFC,
    0XFF, 0XFC, 0XFF, 0XFC, 0X00, 0X0C, 0X00, 0X0C, 0X00, 0X0C, 0X00, 0X0C,
    0X00, 0X0C, 0X00, 0X00, 0X00, 0X00, 0XFF, 0XFC, 0XFF, 0XFC, 0XFF, 0XFC,
    0XC3, 0X0C, 0XC3, 0X0C, 0XC3, 0X0C, 0XC3, 0X0C, 0XC3, 0X0C, 0XC3, 0X0C,
    0X00, 0X00, 0X00, 0X00, 0XC0, 0X00, 0XC0, 0X00, 0XC0, 0X00, 0XFF, 0XFC,
    0XFF, 0XFC, 0XFF, 0XFC, 0XC0, 0X00, 0XC0, 0X00, 0XC0, 0X00};
static const Image logo_image = {125, 14, sizeof(logo_data), logo_data};
const AnimationFrame logo_frame = {62, 54, 20, &logo_image};

/* --- Confirming Animation ------------------------------------------------ */
static const uint8_t confirming_1_data[32] = {
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X60, 0X01,
    0XE0, 0X07, 0XC0, 0X1F, 0XC0, 0X1F, 0XC0, 0X07, 0XC0, 0X01, 0XE0,
    0X00, 0X60, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00};
static const Image confirming_1_image = {16, 16, sizeof(confirming_1_data),
                                         confirming_1_data};

const AnimationFrame confirm_icon_frame = {55, STATUS_BAR_MARGIN, 220,
                                           &confirming_1_image};

static const uint8_t confirming_2_data[32] = {
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X60, 0X01,
    0XE0, 0X07, 0XC0, 0X1F, 0XC0, 0X9F, 0XC0, 0X87, 0XC0, 0XC1, 0XE0,
    0X40, 0X60, 0X60, 0X00, 0X20, 0X00, 0X00, 0X00, 0X00, 0X00};
static const Image confirming_2_image = {16, 16, sizeof(confirming_2_data),
                                         confirming_2_data};

static const uint8_t confirming_3_data[32] = {
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X60, 0X01,
    0XE0, 0X07, 0XC0, 0X1F, 0XC0, 0X9F, 0XC0, 0X87, 0XC0, 0XC1, 0XE0,
    0X40, 0X60, 0X60, 0X00, 0X30, 0X00, 0X1C, 0X00, 0X07, 0X00};
static const Image confirming_3_image = {16, 16, sizeof(confirming_3_data),
                                         confirming_3_data};

static const uint8_t confirming_4_data[32] = {
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X60, 0X01,
    0XE0, 0X07, 0XC0, 0X1F, 0XC0, 0X9F, 0XC0, 0X87, 0XC0, 0XC1, 0XE0,
    0X40, 0X60, 0X60, 0X00, 0X30, 0X0C, 0X1C, 0X38, 0X07, 0XE0};
static const Image confirming_4_image = {16, 16, sizeof(confirming_4_data),
                                         confirming_4_data};

static const uint8_t confirming_5_data[32] = {
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X60, 0X01,
    0XE0, 0X07, 0XC0, 0X1F, 0XC0, 0X9F, 0XC1, 0X87, 0XC1, 0XC1, 0XE3,
    0X40, 0X62, 0X60, 0X06, 0X30, 0X0C, 0X1C, 0X38, 0X07, 0XE0};
static const Image confirming_5_image = {16, 16, sizeof(confirming_5_data),
                                         confirming_5_data};

static const uint8_t confirming_6_data[32] = {
    0X00, 0X00, 0X00, 0X00, 0X00, 0X04, 0X00, 0X06, 0X00, 0X62, 0X01,
    0XE3, 0X07, 0XC1, 0X1F, 0XC1, 0X9F, 0XC1, 0X87, 0XC1, 0XC1, 0XE3,
    0X40, 0X62, 0X60, 0X06, 0X30, 0X0C, 0X1C, 0X38, 0X07, 0XE0};
static const Image confirming_6_image = {16, 16, sizeof(confirming_6_data),
                                         confirming_6_data};

static const uint8_t confirming_7_data[32] = {
    0X00, 0XE0, 0X00, 0X38, 0X00, 0X0C, 0X00, 0X06, 0X00, 0X62, 0X01,
    0XE3, 0X07, 0XC1, 0X1F, 0XC1, 0X9F, 0XC1, 0X87, 0XC1, 0XC1, 0XE3,
    0X40, 0X62, 0X60, 0X06, 0X30, 0X0C, 0X1C, 0X38, 0X07, 0XE0};
static const Image confirming_7_image = {16, 16, sizeof(confirming_7_data),
                                         confirming_7_data};

static const uint8_t confirming_8_data[32] = {
    0X07, 0XE0, 0X1C, 0X38, 0X30, 0X0C, 0X00, 0X06, 0X00, 0X62, 0X01,
    0XE3, 0X07, 0XC1, 0X1F, 0XC1, 0X9F, 0XC1, 0X87, 0XC1, 0XC1, 0XE3,
    0X40, 0X62, 0X60, 0X06, 0X30, 0X0C, 0X1C, 0X38, 0X07, 0XE0};
static const Image confirming_8_image = {16, 16, sizeof(confirming_8_data),
                                         confirming_8_data};

static const uint8_t confirming_9_data[32] = {
    0X07, 0XE0, 0X1C, 0X38, 0X30, 0X0C, 0X60, 0X06, 0X40, 0X62, 0XC1,
    0XE3, 0X87, 0XC1, 0X9F, 0XC1, 0X9F, 0XC1, 0X87, 0XC1, 0XC1, 0XE3,
    0X40, 0X62, 0X60, 0X06, 0X30, 0X0C, 0X1C, 0X38, 0X07, 0XE0};
static const Image confirming_9_image = {16, 16, sizeof(confirming_9_data),
                                         confirming_9_data};

const VariantAnimation confirming = {
    9,
    {
        {55, STATUS_BAR_MARGIN, 220, &confirming_1_image},
        {55, STATUS_BAR_MARGIN, 220, &confirming_2_image},
        {55, STATUS_BAR_MARGIN, 220, &confirming_3_image},
        {55, STATUS_BAR_MARGIN, 220, &confirming_4_image},
        {55, STATUS_BAR_MARGIN, 220, &confirming_5_image},
        {55, STATUS_BAR_MARGIN, 220, &confirming_6_image},
        {55, STATUS_BAR_MARGIN, 220, &confirming_7_image},
        {55, STATUS_BAR_MARGIN, 220, &confirming_8_image},
        {55, STATUS_BAR_MARGIN, 220, &confirming_9_image},
    }};

/* --- Loading Animation --------------------------------------------------- */
static const uint8_t loading_1_data[2] = {0};
static const Image loading_1_image = {2, 8, sizeof(loading_1_data),
                                      loading_1_data};

const VariantAnimation loading = {1,
                                  {
                                      {83, 29, 40, &loading_1_image}
                                  }};

/* --- Warning Animation --------------------------------------------------- */

static const uint8_t warning_1_data[2] = {0};
static const Image warning_1_image = {2, 8, sizeof(warning_1_data),
                                      warning_1_data};

const VariantAnimation warning = {1,
                                  {
                                      {107, 7, 500, &warning_1_image}
                                  }};

/* --- Unplug Image -------------------------------------------------------- */

static const uint8_t unplug_data[32] = {0};
static const Image unplug_image = {45, 27, sizeof(unplug_data), unplug_data};
const AnimationFrame unplug_frame = {208, 21, 20, &unplug_image};

/* --- Recovery Image ------------------------------------------------------ */

static const uint8_t recovery_data[32] = {0};
static const Image recovery_image = {29, 29, sizeof(recovery_data),
                                     recovery_data};
const AnimationFrame recovery_frame = {221, 20, 20, &recovery_image};

static const uint8_t battery_1_data[44] = {
    0X1F, 0XF8, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20,
    0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04,
    0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20,
    0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X1F, 0XF8, 0X03, 0XC0};
static const Image battery_1_image = {22, 16, sizeof(battery_1_data),
                                      battery_1_data};

static const uint8_t battery_2_data[44] = {
    0X1F, 0XF8, 0X20, 0X04, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F,
    0XF4, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04,
    0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20,
    0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X1F, 0XF8, 0X03, 0XC0};
static const Image battery_2_image = {22, 16, sizeof(battery_2_data),
                                      battery_2_data};

static const uint8_t battery_3_data[44] = {
    0X1F, 0XF8, 0X20, 0X04, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F,
    0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X20, 0X04,
    0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X20,
    0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X1F, 0XF8, 0X03, 0XC0};
static const Image battery_3_image = {22, 16, sizeof(battery_3_data),
                                      battery_3_data};

static const uint8_t battery_4_data[44] = {
    0X1F, 0XF8, 0X20, 0X04, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F,
    0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4,
    0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X20, 0X04, 0X20, 0X04, 0X20,
    0X04, 0X20, 0X04, 0X20, 0X04, 0X20, 0X04, 0X1F, 0XF8, 0X03, 0XC0};
static const Image battery_4_image = {22, 16, sizeof(battery_4_data),
                                      battery_4_data};

static const uint8_t battery_5_data[44] = {
    0X1F, 0XF8, 0X20, 0X04, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F,
    0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4,
    0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X2F,
    0XF4, 0X2F, 0XF4, 0X2F, 0XF4, 0X20, 0X04, 0X1F, 0XF8, 0X03, 0XC0};

static const Image battery_5_image = {22, 16, sizeof(battery_5_data),
                                      battery_5_data};

VariantAnimation battery_animation = {
    5,
    {
        {213, STATUS_BAR_MARGIN, 500, &battery_1_image},
        {213, STATUS_BAR_MARGIN, 500, &battery_2_image},
        {213, STATUS_BAR_MARGIN, 500, &battery_3_image},
        {213, STATUS_BAR_MARGIN, 500, &battery_4_image},
        {213, STATUS_BAR_MARGIN, 500, &battery_5_image},
    }};

static const uint8_t BATTERY_PERCENTAGE_INDEX[10] = {0, 1, 1, 2, 2,
                                                     3, 3, 3, 4, 4};

static const uint8_t battery_charging[18] = {
    0X00, 0X00, 0X00, 0X80, 0X01, 0X88, 0X03, 0XB8, 0X07,
    0XF0, 0X0E, 0XE0, 0X08, 0XC0, 0X00, 0X80, 0X00, 0X00};

static const Image battery_charging_image = {9, 16, sizeof(battery_charging),
                                             battery_charging};
const AnimationFrame battery_charging_frame = {238, STATUS_BAR_MARGIN, 500,
                                               &battery_charging_image};

static const uint8_t ble_icon_data[32] = {
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X18, 0X18, 0X0C,
    0X30, 0X06, 0X60, 0X7F, 0XFE, 0X31, 0X8C, 0X1B, 0XD8, 0X0E, 0X70,
    0X04, 0X20, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00};

static const Image ble_icon_image = {16, 16, sizeof(ble_icon_data),
                                     ble_icon_data};
const AnimationFrame ble_icon_frame = {190, STATUS_BAR_MARGIN, 500,
                                       &ble_icon_image};

static const uint8_t ble_connected_data[32] = {
    0X00, 0X00, 0X00, 0X00, 0X01, 0X80, 0X01, 0X80, 0X18, 0X18, 0X0C,
    0X30, 0X06, 0X60, 0X7D, 0XBE, 0X31, 0X8C, 0X1A, 0X58, 0X0E, 0X70,
    0X04, 0X20, 0X01, 0X80, 0X01, 0X80, 0X00, 0X00, 0X00, 0X00};

static const Image ble_connected_image = {16, 16, sizeof(ble_connected_data),
                                          ble_connected_data};
const AnimationFrame ble_connected_frame = {190, STATUS_BAR_MARGIN, 500,
                                            &ble_connected_image};
/* === Functions =========================================================== */

/*
 * get_confirm_icon_frame() - Get confirm icon frame
 *
 * INPUT
 *     none
 * OUTPUT
 *     confirm icon frame
 */
const AnimationFrame *get_confirm_icon_frame(void) {
    return &confirm_icon_frame;
}

const AnimationFrame *get_battery_frame(uint8_t percent) {
    if (percent > 100 || percent < 10) {
        return &(battery_animation.frames[0]);
    }
    uint8_t index = BATTERY_PERCENTAGE_INDEX[(percent - 1) / 10];
    return &battery_animation.frames[index];
}

const AnimationFrame *get_battery_charging_frame(void) {
    return &battery_charging_frame;
}

const AnimationFrame *get_ble_frame(uint8_t connected) {
    if (connected) {
        return &ble_connected_frame;
    }

    return &ble_icon_frame;
}
/*
 * get_confirmed_frame() - Get confirmed frame
 *
 * INPUT
 *     none
 * OUTPUT
 *     confirmed frame
 */
const AnimationFrame *get_confirmed_frame(void) {
    // the confirmed image is the final frame of the confirming animation
    return &confirming.frames[confirming.count - 1];
}


const AnimationFrame *get_logo_frame(void) { return &logo_frame; }
/*
 * get_unplug_frame() - Get device unplug image
 *
 * INPUT
 *     none
 * OUTPUT
 *     unplug frame
 */
const AnimationFrame *get_unplug_frame(void) { return &unplug_frame; }

/*
 * get_recovery_frame() - Get recovery icon frame
 *
 * INPUT
 *     none
 * OUTPUT
 *     recovery frame
 */
const AnimationFrame *get_recovery_frame(void) { return &recovery_frame; }

/*
 * get_warning_frame() - Get warning icon frame
 *
 * INPUT
 *     none
 * OUTPUT
 *     warining frame
 */
const AnimationFrame *get_warning_frame(void) { return &warning.frames[0]; }

/*
 * get_confirming_animation() - Get confirming animation
 *
 * INPUT
 *     none
 * OUTPUT
 *     confirming animation
 */
const VariantAnimation *get_confirming_animation(void) { return &confirming; }

/*
 * get_loading_animation() - Get loading animation
 *
 * INPUT
 *     none
 * OUTPUT
 *     loading animation
 */
const VariantAnimation *get_loading_animation(void) { return &loading; }

/*
 * get_warning_animation() - Get warning animation
 *
 * INPUT
 *     none
 * OUTPUT
 *     warning animation
 */
const VariantAnimation *get_warning_animation(void) { return &warning; }

/*
 * get_image_animation_duration() - Calculate animation duration
 *
 * INPUT
 *     Animation
 * OUTPUT
 *    animation duration
 */
uint32_t get_image_animation_duration(const VariantAnimation *animation) {
    uint32_t duration = 0;

    for (int i = 0; i < animation->count; i++) {
        duration += animation->frames[i].duration;
    }

    return duration;
}

/*
 * get_image_animation_frame() - Get an animation frame
 *
 * INPUT
 *     - img_animation: animation to pull frame from
 *     - elapsed: how long has animation elapsed
 *     - loop: is this animation looping?
 * OUTPUT
 *    number of the frame that should be displayed
 */
int get_image_animation_frame(const VariantAnimation *animation,
                              const uint32_t elapsed, bool loop) {
    uint32_t duration = get_image_animation_duration(animation);
    uint32_t adjusted_elapsed = (loop) ? elapsed % duration : elapsed;
    uint32_t current_time = 0;

    for (int i = 0; i < animation->count; i++) {
        current_time += animation->frames[i].duration;

        if (adjusted_elapsed <= current_time) {
            return i;
        }
    }

    return -1;
}
