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

#include "board/canvas.h"
#include "driver/display.h"

static uint8_t m_canvas_buffer[DISPLAY_WIDTH * ((DISPLAY_HEIGHT + 7) / 8)];
static canvas_t m_canvas;

/*
 * display_canvas_init() - Display canvas initialization
 *
 * INPUT
 *     none
 * OUTPUT
 *     pointer to canvas
 */
canvas_t *canvas_init(void) {
    /* Prepare the canvas */
    memset(m_canvas_buffer, 0, sizeof(m_canvas_buffer));
    m_canvas.buffer = m_canvas_buffer;
    m_canvas.width = DISPLAY_WIDTH;
    m_canvas.height = DISPLAY_HEIGHT;
    m_canvas.dirty = false;

    return &m_canvas;
}

canvas_t *canvas_get(void) { return &m_canvas; }

void canvas_clear(void) {
    memset(m_canvas_buffer, 0, sizeof(m_canvas_buffer));
}