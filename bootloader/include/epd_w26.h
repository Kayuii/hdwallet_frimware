#ifndef _EPD_Wxx_H_
#define _EPD_Wxx_H_

#define EPD_RES_WIDTH 296
#define EPD_RES_HIGHT 152
#define EPD_RES_ROW 19

#define EPD_FULLBUF_SIZE (EPD_RES_WIDTH * EPD_RES_HIGHT / 8)

#ifndef ErrorStatus
typedef enum
{
	ERROR = 0,
	SUCCESS = !ERROR
} ErrorStatus;
#endif

typedef enum
{
	EPD_Wxx_CMD_PSR = 0x00,  //Panel Setting (PSR)
	EPD_Wxx_CMD_PWR = 0x02,  //Power Setting (PWR)
	EPD_Wxx_CMD_POF = 0x03,  //Power OFF (POF)
	EPD_Wxx_CMD_PFS = 0x04,  //Power OFF Sequence Setting (PFS)
	EPD_Wxx_CMD_PON = 0x05,  //Power ON (PON)
	EPD_Wxx_CMD_BTST = 0x06, //Booster Soft Start (BTST)

	EPD_Wxx_CMD_DTM1 = 0x10, //Data Start Transmission 1 (DTM1)
	EPD_Wxx_CMD_DSP = 0x11,  //Data Stop (DSP)
	EPD_Wxx_CMD_DRF = 0x12,  //Display Refresh (DRF)
	EPD_Wxx_CMD_DTM2 = 0x13, //DATA START TRANSMISSION 2 (DTM2)

	EPD_Wxx_CMD_LUTC = 0x20, //VCOM LUT (LUTC) (45-byte command, bytes 2~7 repeated 7 times)
	EPD_Wxx_CMD_PLL = 0x30,  //PLL control (PLL)

	EPD_Wxx_CMD_CDI = 0x50, //Vcom and data interval setting (CDI)

	EPD_Wxx_CMD_SETRES = 0x61,

	EPD_Wxx_CMD_REV = 0x70, //Revision
	EPD_Wxx_CMD_FLG = 0x71, // reads the IC status

	EPD_Wxx_CMD_VDCS = 0x82, //VCM_DC Setting (VDCS)

	EPD_Wxx_CMD_PTL = 0x90,  //PARTIAL WINDOW (PTL) (R90H)
	EPD_Wxx_CMD_PTIN = 0x91, //PARTIAL IN (PTIN) (R91H)
} EPD_Wxx_CMD_EN;

void EPD_display_init(void);
void EPD_Wxx_Sleep(void);

void EPD_Display_Full(const uint8_t *buf);
void EPD_Display_Partial(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end, uint8_t *buf, uint16_t size); //partial display

#endif