#include "driver/epd_wxx_hal.h"

#include "nrf_drv_spi.h"

#define SPI_INSTANCE 1

static const nrf_drv_spi_t spi =
    NRF_DRV_SPI_INSTANCE(SPI_INSTANCE); /**< SPI instance. */

static uint8_t spisendbuf[256] __attribute((aligned(4)));

#ifdef FREERTOS1
static SemaphoreHandle_t xSemaphore_epd_wxx_spi;
#else
static uint8_t fspicplted = 0;
#endif

static void spi_event_handler(nrf_drv_spi_evt_t const *p_event,
                              void *p_context) {
					NRF_LOG_DEBUG("---spi_event_handler--");
#ifdef FREERTOS1
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xSemaphore_epd_wxx_spi, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#else
	fspicplted = 1;
#endif
}

static void ClearSpicomflag(void) {
#ifdef FREERTOS1
    xSemaphoreTake(xSemaphore_epd_wxx_spi, 0);
#else
    fspicplted = 0;
#endif
}

static uint8_t WaitSpicomcomplete(uint16_t ms) {
#ifdef FREERTOS1
    if (pdFALSE == xSemaphoreTake(xSemaphore_epd_wxx_spi, ms))
        return 2;
    else
        return 0;
#else
	uint16_t tmp = 0;
	while(fspicplted == 0)
	{
		nrf_delay_us(1);
		tmp++;
		if(tmp > 10000)
		{
			NRF_LOG_DEBUG("ERR!WaitSpicomcomplete...");
			return 1;
		}
	}
    return 0;  // Ok;
#endif
}

static uint8_t SPI_WriteBuf(const uint8_t *buf, uint16_t len, uint8_t resetcs) {
    uint8_t *buftosend = buf;

    uint8_t framlen = 0xFF;

    ret_code_t ret;

    // NRF_LOG_DEBUG("SPI_WriteBuf %d", len);NRF_LOG_PROCESS();

    EPD_Wxx_CS_0;

    while (len > framlen) {
        memcpy(spisendbuf, buftosend, framlen);

        ClearSpicomflag();
		ret = nrf_drv_spi_transfer(&spi, spisendbuf, framlen, NULL, 0);
		// NRF_LOG_DEBUG("A: %d", ret);NRF_LOG_PROCESS();
        APP_ERROR_CHECK(ret);
        if (0 != WaitSpicomcomplete(40)) {
            NRF_LOG_WARNING("epd_w21WriteCmd time out!");
            if (resetcs) EPD_Wxx_CS_1;
            return 2;
        }

        len -= framlen;
        buftosend += framlen;
    }

    memcpy(spisendbuf, buftosend, len);

    ClearSpicomflag();
    ret = nrf_drv_spi_transfer(&spi, spisendbuf, len, NULL, 0);
	// NRF_LOG_DEBUG("B: %d", ret);NRF_LOG_PROCESS();
    APP_ERROR_CHECK(ret);
    if (0 != WaitSpicomcomplete(40)) {
        NRF_LOG_WARNING("epd_w21WriteCmd time out!");
        if (resetcs) EPD_Wxx_CS_1;
        return 2;
    }

    if (resetcs) EPD_Wxx_CS_1;

    return 0;
}

void epd_wxx_hal_init(void) {
#ifdef FREERTOS1
    xSemaphore_epd_wxx_spi = xSemaphoreCreateBinary();

    if (xSemaphore_epd_wxx_spi == NULL) {
        NRF_LOG_ERROR("xSemaphore_epd_wxx_spi Creat Fail");
    }
#endif
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin = NRF_DRV_SPI_PIN_NOT_USED;
    spi_config.mosi_pin = EPD_Wxx_MOSI_PIN;
    spi_config.sck_pin = EPD_Wxx_CLK_PIN;
    spi_config.frequency = SPIM_FREQUENCY_FREQUENCY_M16;

    APP_ERROR_CHECK(
        nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

    nrf_gpio_cfg_output(EPD_Wxx_CS_PIN);
    nrf_gpio_cfg_output(EPD_Wxx_DC_PIN);
    nrf_gpio_cfg_output(EPD_Wxx_RST_PIN);
    nrf_gpio_cfg_input(EPD_Wxx_BS_PIN, GPIO_PIN_CNF_PULL_Disabled);
}

void epd_wxx_hal_uninit(void) {
    nrf_drv_spi_uninit(&spi);
    nrf_gpio_cfg_default(EPD_Wxx_MOSI_PIN);
    nrf_gpio_cfg_default(EPD_Wxx_CLK_PIN);
    nrf_gpio_cfg_default(EPD_Wxx_CS_PIN);
    nrf_gpio_cfg_default(EPD_Wxx_DC_PIN);
    nrf_gpio_cfg_default(EPD_Wxx_RST_PIN);
}

void epd_wxx_hal_write_cmd(unsigned char command) {
    EPD_Wxx_DC_C;  // command write
    NRF_LOG_DEBUG("SPI write cmd");
    SPI_WriteBuf(&command, 1, true);
    EPD_Wxx_DC_D;  // command write
}

void epd_wxx_hal_write_data_char(unsigned char data) {
    NRF_LOG_DEBUG("SPI write char");
    SPI_WriteBuf(&data, 1, true);
}

void epd_wxx_hal_write_data_buffer(const uint8_t *buf, uint16_t len) {
    NRF_LOG_DEBUG("SPI write buffer");
    SPI_WriteBuf(buf, len, true);
}

void epd_wxx_hal_delay_us(unsigned int xus) 
{ 
	nrf_delay_us(xus); 
}

void epd_wxx_hal_delay_ms(unsigned int xms) {
#ifndef FREERTOS
    nrf_delay_ms(xms);
#else
    vTaskDelay(xms);
#endif
}
