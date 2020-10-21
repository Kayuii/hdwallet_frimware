#include "driver/gt24l24a2y_hal.h"
#include "driver/gt24l24a2y.h"

#ifdef ZK_SPI_HW
#include "nrf_spi_mngr.h"

#define SPI_INSTANCE_GT24L24A2Y 2

static const nrf_drv_spi_t spi =
    NRF_DRV_SPI_INSTANCE(SPI_INSTANCE_GT24L24A2Y); /**< SPI instance. */

static uint8_t spisendbuf[256];
static uint8_t spirecvbuf[256];

#ifdef FREERTOS
TaskHandle_t hvGT24L24A2Y_Task;  //!< Reference to task.
SemaphoreHandle_t xSemaphore_gt24l24a2y_spi;
#endif

static void spi_event_handler(nrf_drv_spi_evt_t const *p_event,
                              void *p_context) {
#ifdef FREERTOS
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xSemaphore_gt24l24a2y_spi, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif
}

static void ClearSpicomflag(void) {
#ifdef FREERTOS
    xSemaphoreTake(xSemaphore_gt24l24a2y_spi, 0);
#endif
}

static uint8_t WaitSpicomcomplete(uint16_t ms) {
#ifdef FREERTOS
    if (pdFALSE == xSemaphoreTake(xSemaphore_gt24l24a2y_spi, ms))
        return 2;
    else
        return 0;
#else
    return 0;  // Ok;
#endif
}

uint8_t gt24l24a2y_hal_SPI_WriteRead(uint8_t *wbuf, uint16_t wlen,
                                     uint8_t *rbuf, uint16_t rlen) {
    uint8_t *buftosend = rbuf;

    const uint8_t cmaxframlen = 0xFF;

    GT24L24A2Y_CS_0;

    while (rlen > cmaxframlen) {
        if (wlen > cmaxframlen) {
            memcpy(spisendbuf, buftosend, cmaxframlen);
        }

        ClearSpicomflag();
        APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, spisendbuf, cmaxframlen,
                                             spirecvbuf, cmaxframlen));
        if (0 != WaitSpicomcomplete(10)) {
            NRF_LOG_WARNING("epd_w21WriteCmd time out!");
            return 2;
        }

        rlen -= cmaxframlen;
        buftosend += cmaxframlen;
    }

    memcpy(spisendbuf, buftosend, rlen);

    ClearSpicomflag();
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, spisendbuf, rlen, NULL, 0));
    if (0 != WaitSpicomcomplete(10)) {
        NRF_LOG_WARNING("epd_w21WriteCmd time out!");

        //		if(resetcs)
        GT24L24A2Y_CS_1;
        return 2;
    }

    //	if(resetcs)
    GT24L24A2Y_CS_1;

    return 0;
}

void gt24l24a2y_hal_init(void) {
#ifdef FREERTOS
    xSemaphore_gt24l24a2y_spi = xSemaphoreCreateBinary();

    if (xSemaphore_gt24l24a2y_spi == NULL) {
        NRF_LOG_ERROR("xSemaphore_gt24l24a2y_spi Creat Fail");
    }
#endif
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin = NRF_DRV_SPI_PIN_NOT_USED;
    spi_config.mosi_pin = FONT_SI;
    spi_config.miso_pin = FONT_SO;
    spi_config.sck_pin = FONT_SCLK;
    spi_config.frequency = GT24L24A2YL_SPI_SPEED;

    APP_ERROR_CHECK(
        nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

    nrf_gpio_cfg_output(FONT_CS);
}

void gt24l24a2y_hal_Deinit(void) {
    nrf_drv_spi_uninit(&spi);
    vSemaphoreDelete(xSemaphore_gt24l24a2y_spi);
}

void gt24l24a2y_hal_delay_us(unsigned int xus) { nrf_delay_us(xus); }

void gt24l24a2y_hal_delay_ms(unsigned int xms) {
#ifndef FREERTOS
    nrf_delay_ms(xms);
#else
    vTaskDelay(xms);
#endif
}

#elif defined(ZK_SPI_SW)

void zk_init(void) {
    nrf_gpio_cfg_output(ZK_MOSI_PIN);
    nrf_gpio_cfg_output(ZK_SCK_PIN);
    nrf_gpio_cfg_output(ZK_CS_PIN);

    nrf_gpio_cfg_input(ZK_MISO_PIN, NRF_GPIO_PIN_PULLUP);

    Rom_csH;
    MOSIH;
    Rom_sckH;
}

/*******************************************************************************/
// Send data sub-pro (STM8,STM32等双向口)   SPI发送地址的时序算法 /
/*******************************************************************************/

void Send_Byte(unsigned char out) {
    unsigned char i = 0;

    for (i = 0; i < 8; i++) {
        Rom_sckL;  //字库芯片时钟置低
        if (((out << i) & 0x80) == 0)
            MOSIL;
        else
            MOSIH;
        Rom_sckH;
    }
}

/*******************************************************************************/
// Get data sub-pro (STM8,STM32等双向口)    SPI接收点阵数据的算法 /
/*******************************************************************************/
unsigned char Get_Byte(void) {
    unsigned char i;
    unsigned char read_dat = 0;

    Rom_sckH;
    for (i = 0; i < 8; i++) {
        Rom_sckL;
        read_dat = read_dat << 1;
        if (MISO)
            read_dat |= 0x01;
        else
            read_dat &= 0xfe;
        Rom_sckH;
    }
    return (read_dat);
}
/*******************************************************************************/
//                    Send address sub-pro (STM8,STM32，51) /
/*******************************************************************************/
void SPI_Address(unsigned char AddH, unsigned char AddM, unsigned char AddL) {
    Send_Byte(AddH);
    Send_Byte(AddM);
    Send_Byte(AddL);
}
/*******************************************************************************/
//                    Get N bytes sub-pro  (STM8,STM32，51) //
/*******************************************************************************/
//从address地址读取len个字节的数据并存入到DZ_Data数组当中
void r_dat_bat(unsigned long TAB_addr, unsigned int Num, unsigned char *p_arr)
// unsigned char r_dat_bat(unsigned long TAB_addr,unsigned long Num,unsigned
// char *p_arr)
{
    unsigned long i;
    unsigned char addrHigh;
    unsigned char addrMid;
    unsigned char addrLow;
    addrHigh = TAB_addr >> 16;
    addrMid = TAB_addr >> 8;
    addrLow = (unsigned char)TAB_addr;

    Rom_csL;  //片选选中字库芯片
    Send_Byte(
        0x03);  //普通读取首先送0X03,然后发送地址高八位addrHigh，中八位addrMid，低八位addrLow。
    SPI_Address(addrHigh, addrMid, addrLow);
    for (i = 0; i < Num; i++) *(p_arr + i) = Get_Byte();
    Rom_csH;
    //	return 0;
}

//客户自己实现，从address地址读取一个字节的数据并返回该数据
unsigned char r_dat(uint32_t address) {
    unsigned char buff;
    unsigned char addrHigh;
    unsigned char addrMid;
    unsigned char addrLow;
    addrHigh = address >> 16;
    addrMid = address >> 8;
    addrLow = (unsigned char)address;

    Rom_csL;
    Send_Byte(0x03);
    SPI_Address(addrHigh, addrMid, addrLow);
    buff = Get_Byte();
    Rom_csL;
    return buff;
}

/******************************************************
客户自己实现, 库文件函数内部需要调用该函数匹配芯片ID号
根据说明文件或头文件是否需要, 没有就不需要实现
******************************************************/
unsigned char CheckID(unsigned char CMD, unsigned long address,
                      unsigned long byte_long, unsigned char *p_arr) {
    unsigned long j;
    Rom_csL;
    Send_Byte(CMD);
    Send_Byte((unsigned char)((address) >> 16));
    Send_Byte((unsigned char)((address) >> 8));
    Send_Byte((unsigned char)address);
    for (j = 0; j < byte_long; j++) {
        p_arr[j] = Get_Byte();
    }
    Rom_csH;
    return 1;
}

#endif
