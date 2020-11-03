#include "driver/display.h"
#include "board/canvas.h"

#define DISPLAY_TASK_NAME "DISPLAY"
#define DISPLAY_TASK_STACK (128)
#define DISPLAY_TASK_PRIO 2

static TaskHandle_t display_task_handle;
static SemaphoreHandle_t display_refresh_semaphore;
static SemaphoreHandle_t display_flush_semaphore;

static uint8_t display_buffer[EPD_RES_ROW * EPD_RES_WIDTH];

static inline void lsh_arry_bits(unsigned char *arry, unsigned int len,
                                 unsigned char lshbits) {
    uint16_t i;
    for (i = 0; i < len - 1; i++) {
        *(arry + i) =
            ((*(arry + i)) << lshbits) | ((*(arry + i + 1)) >> (8 - lshbits));
    }
    *(arry + len - 1) = (*(arry + len - 1)) << lshbits;
}

void display_clear(void) {
    for (uint16_t i = 0; i < EPD_FULLBUF_SIZE; i++) {
        display_buffer[i] = 0xFF;
        // display_buffer[i] = 0x00;
    }
    epd_refresh_full(display_buffer);
}

void display_refresh(uint8_t flush) {
    bool semaphore_error = false;
    if (0 == __get_CONTROL()) {
        BaseType_t xYieldRequired;
        if (flush) {
            if (pdPASS !=
                xSemaphoreGiveFromISR(display_flush_semaphore, NULL)) {
                semaphore_error = true;
            }
        }
        if (pdPASS != xSemaphoreGiveFromISR(display_refresh_semaphore, NULL)) {
            semaphore_error = true;
        }
        xYieldRequired = xTaskResumeFromISR(display_task_handle);
        portYIELD_FROM_ISR(xYieldRequired);
    } else {
        if (flush) {
            if (pdPASS != xSemaphoreGive(display_flush_semaphore)) {
                semaphore_error = true;
            }
        }
        if (pdPASS != xSemaphoreGive(display_refresh_semaphore)) {
            semaphore_error = true;
        }
    }
    if (semaphore_error) {
        NRF_LOG_ERROR("Display Refresh ERR");
    }
}

static inline uint32_t reverse_bits(uint32_t v) {
    uint32_t input = v;
    uint32_t output;
    __asm__("rbit %0, %1\n" : "=r"(output) : "r"(input));
    return output;
}

static inline void display_canvas_to_buffer(void) {
    uint16_t i = 0;

    uint16_t buf_size_word = EPD_FULLBUF_SIZE / 4;
    uint32_t *p_canvas_buf_word = (uint32_t *)(canvas_get()->buffer);
    uint32_t *p_disp_buf_word = (uint32_t *)display_buffer;

    p_canvas_buf_word += buf_size_word - 1;

    for (i = 0; i < buf_size_word; i++) {
        *p_disp_buf_word = reverse_bits(*p_canvas_buf_word);
#if (DOT_WHITE_VALUE == 1)
        *p_disp_buf_word = ~(*p_disp_buf_word);
#endif
        p_disp_buf_word++;
        p_canvas_buf_word--;
    }

    lsh_arry_bits(display_buffer, EPD_FULLBUF_SIZE, 6);
}

static inline void display_paint_from_canvas(bool flush) {
    display_canvas_to_buffer();
    if (flush) {
        epd_init_full();
        epd_refresh_full(display_buffer);
        epd_init_part();
    } else {
        epd_refresh_partial(display_buffer);
    }
}

void display_task(void *params) {
    NRF_LOG_DEBUG("Starting display task...");

    // First refresh shall always flush the display.
    if (pdPASS != xSemaphoreGive(display_flush_semaphore)) {
        NRF_LOG_ERROR("xSemaphoreGive ERR\r\n");
    }
    while (1) {
        if (pdTRUE ==
            xSemaphoreTake(display_refresh_semaphore, portMAX_DELAY)) {
            display_paint_from_canvas(
                pdTRUE == xSemaphoreTake(display_flush_semaphore, 0));
        }
    }
}

void display_init(void) {
    epd_wxx_hal_init();
    display_refresh_semaphore = xSemaphoreCreateBinary();
    display_flush_semaphore = xSemaphoreCreateBinary();
    if (pdPASS != xTaskCreate(display_task, DISPLAY_TASK_NAME,
                              DISPLAY_TASK_STACK, (void *)NULL,
                              DISPLAY_TASK_PRIO, &display_task_handle)) {
        NRF_LOG_ERROR("Test task not created.");
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
}
