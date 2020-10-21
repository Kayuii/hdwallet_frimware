#if (EPD_W26)

#include "epd_wxx_hal.h"
#include "epd_w26.h"

const static uint8_t cgnPic_white[EPD_FULLBUF_SIZE] = {0};
uint8_t cgnDisplayBuff[EPD_FULLBUF_SIZE] = {0};


//////////////////////////////////////full screen update LUT////////////////////////////////////////////
const unsigned char lut_vcomDC[] ={
0x00	,0x08	,0x00	,0x00	,0x00	,0x02,	
0x60	,0x28	,0x28	,0x00	,0x00	,0x01,	
0x00	,0x14	,0x00	,0x00	,0x00	,0x01,	
0x00	,0x12	,0x12	,0x00	,0x00	,0x01,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	
,0x00	,0x00,					};
const unsigned char lut_ww[] ={	
0x40	,0x08	,0x00	,0x00	,0x00	,0x02,	
0x90	,0x28	,0x28	,0x00	,0x00	,0x01,	
0x40	,0x14	,0x00	,0x00	,0x00	,0x01,	
0xA0	,0x12	,0x12	,0x00	,0x00	,0x01,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	};
const unsigned char lut_bw[] ={	
0x40	,0x17	,0x00	,0x00	,0x00	,0x02	,
0x90	,0x0F	,0x0F	,0x00	,0x00	,0x03	,
0x40	,0x0A	,0x01	,0x00	,0x00	,0x01	,
0xA0	,0x0E	,0x0E	,0x00	,0x00	,0x02	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,					};
const unsigned char lut_wb[] ={	
0x80	,0x08	,0x00	,0x00	,0x00	,0x02,	
0x90	,0x28	,0x28	,0x00	,0x00	,0x01,	
0x80	,0x14	,0x00	,0x00	,0x00	,0x01,	
0x50	,0x12	,0x12	,0x00	,0x00	,0x01,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	};
const unsigned char lut_bb[] ={	
0x80	,0x08	,0x00	,0x00	,0x00	,0x02,	
0x90	,0x28	,0x28	,0x00	,0x00	,0x01,	
0x80	,0x14	,0x00	,0x00	,0x00	,0x01,	
0x50	,0x12	,0x12	,0x00	,0x00	,0x01,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	};



/////////////////////////////////////partial screen update LUT///////////////////////////////////////////
const unsigned char lut_vcom1[] ={
0x00	,0x19	,0x01	,0x00	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00
	,0x00	,0x00,					};
const unsigned char lut_ww1[] ={
0x00	,0x19	,0x01	,0x00	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,};
const unsigned char lut_bw1[] ={
0x80	,0x19	,0x01	,0x00	,0x00	,0x01,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	};
const unsigned char lut_wb1[] ={
0x40	,0x19	,0x01	,0x00	,0x00	,0x01,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	};
const unsigned char lut_bb1[] ={
0x00	,0x19	,0x01	,0x00	,0x00	,0x01,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	};

void EPD_Wxx_Reset(void)
{
	EPD_Wxx_BS_0;		// 4 wire spi mode selected

	EPD_Wxx_RST_0;		// Module reset
	epd_wxx_hal_delay_ms(10);
	EPD_Wxx_RST_1;
//	epd_wxx_hal_delay_ms(100);
	
//	EPD_Wxx_DispInit();		// pannel configure

//	EPD_Wxx_WirteLUT(LUTDefault);	// update wavefrom

}

void EPD_Wxx_SetRes(uint8_t hres, uint16_t vres)
{
	epd_wxx_hal_write_cmd(EPD_Wxx_CMD_SETRES); 		//resolution setting
	epd_wxx_hal_write_data_char (hres);			 
	epd_wxx_hal_write_data_char ((vres >> 8) & 0x01); 	
	epd_wxx_hal_write_data_char (vres&0xFF);
}

//Detection busy
void lcd_chkstatus(void)
{
	uint16_t i = 0;
	unsigned char busy = true;
	while(busy)
	{
		epd_wxx_hal_write_cmd(0x71);
		busy = isEPD_Wxx_BUSY;
		busy =!(busy & 0x01);    
		epd_wxx_hal_delay_ms(1);    

		i++;
		if(i > 10000)
		{
			NRF_LOG_WARNING("lcd_chkstatus TIMEOUT .");
			break;;
		}
	}
	NRF_LOG_INFO("lcd_chkstatus %d .", i);
	epd_wxx_hal_delay_ms(2);           //200            
}


//LUT download
void lut_full(void)
{
	unsigned int count;
	epd_wxx_hal_write_cmd(0x20);
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&lut_vcomDC, 44, true);

	epd_wxx_hal_write_cmd(0x21);
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&lut_ww, sizeof(lut_ww), true);

	epd_wxx_hal_write_cmd(0x22);
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&lut_bw, sizeof(lut_bw), true);

	epd_wxx_hal_write_cmd(0x23);
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&lut_wb, sizeof(lut_wb), true);

	epd_wxx_hal_write_cmd(0x24);
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&lut_bb, sizeof(lut_bb), true);
}


/////////////////////////////////////partial screen update LUT///////////////////////////////////////////
void lut_part(void)
{
	unsigned int count;
	epd_wxx_hal_write_cmd(0x20);
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&lut_vcom1, sizeof(lut_vcom1), true);

	epd_wxx_hal_write_cmd(0x21);
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&lut_ww1, sizeof(lut_ww1), true);
	
	epd_wxx_hal_write_cmd(0x22);
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&lut_bw1, sizeof(lut_bw1), true);

	epd_wxx_hal_write_cmd(0x23);
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&lut_wb1, sizeof(lut_wb1), true);

	epd_wxx_hal_write_cmd(0x24);
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&lut_bb1, sizeof(lut_bb1), true);
}

/*************************EPD display init function******************************************************/
void EPD_display_init(void)
{	
		EPD_Wxx_Reset();
		
		epd_wxx_hal_write_cmd	(EPD_Wxx_CMD_PWR);			//POWER SETTING 
		epd_wxx_hal_write_data_char (0x03);	          
		epd_wxx_hal_write_data_char (0x00);
		epd_wxx_hal_write_data_char (0x2b);	//101011 11.0 V  dft:100110 10.0V 
		epd_wxx_hal_write_data_char (0x2b);	//101011 11.0 V  dft:100110 10.0V 
		epd_wxx_hal_write_data_char (0x03);

		epd_wxx_hal_write_cmd(EPD_Wxx_CMD_BTST);         //boost soft start
		epd_wxx_hal_write_data_char (0x17);		//A
		epd_wxx_hal_write_data_char (0x17);		//B
		epd_wxx_hal_write_data_char (0x17);		//C       

		epd_wxx_hal_write_cmd(EPD_Wxx_CMD_PFS);  
		lcd_chkstatus();

		epd_wxx_hal_write_cmd(EPD_Wxx_CMD_PSR);			//panel setting
		epd_wxx_hal_write_data_char(0xd3);		//LUT from OTP��0xbf: 128x296  0xdf:160x296 Bean 20181113
		epd_wxx_hal_write_data_char(0x0d);		//VCOM to 0V fast
		
		epd_wxx_hal_write_cmd(EPD_Wxx_CMD_PLL);			//PLL setting
		epd_wxx_hal_write_data_char (39);   // 3a 100HZ   29 150Hz 39 200HZ	31 171HZ

		EPD_Wxx_SetRes(EPD_RES_HIGHT, EPD_RES_WIDTH);	//resolution setting
//		epd_wxx_hal_write_cmd(0x61);			//resolution setting
//		epd_wxx_hal_write_data_char (HRES);        	 
//		epd_wxx_hal_write_data_char (VRES_byte1);		
//		epd_wxx_hal_write_data_char (VRES_byte2);
		
		epd_wxx_hal_write_cmd(EPD_Wxx_CMD_VDCS);			//vcom_DC setting  	
    	epd_wxx_hal_write_data_char (0x28);	

		epd_wxx_hal_write_cmd(EPD_Wxx_CMD_CDI);			//VCOM AND DATA INTERVAL SETTING			
		epd_wxx_hal_write_data_char(0x97);		//WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
	}

void EPD_Wxx_Sleep(void)
{
	/////////////////////////////Enter deep sleep mode////////////////////////
	epd_wxx_hal_write_cmd(0X50);
	epd_wxx_hal_write_data_char(0xf7);	
	epd_wxx_hal_write_cmd(0X02);  	//power off
	epd_wxx_hal_write_cmd(0X07);  	//deep sleep
	epd_wxx_hal_write_data_char(0xA5);
}



/***************************full display function*************************************/
void EPD_Display_Full(const uint8_t *buf)
{
	epd_wxx_hal_write_cmd(0x10);
		EPD_Wxx_DC_D;		// data write
		SPI_WriteBuf(cgnPic_white, EPD_RES_ROW*EPD_RES_WIDTH, true);
	epd_wxx_hal_delay_ms(2);

	epd_wxx_hal_write_cmd(0x13);  
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(buf, EPD_RES_ROW*EPD_RES_WIDTH, true);
	epd_wxx_hal_delay_ms(2);	 	 

	lut_full(); //Power settings
	epd_wxx_hal_write_cmd(EPD_Wxx_CMD_DRF);			//DISPLAY REFRESH 	
	epd_wxx_hal_delay_ms(100);	    //!!!The delay here is necessary, 200uS at least!!!     
	lcd_chkstatus();
}

/***************************partial display function*************************************/
void EPD_Display_Partial(uint16_t x_start,uint16_t x_end,uint16_t y_start,uint16_t y_end ,uint8_t *buf,uint16_t size) //partial display
{
	epd_wxx_hal_write_cmd(0x82);			//vcom_DC setting  	
	epd_wxx_hal_write_data_char (0x08);	
	epd_wxx_hal_write_cmd(0X50);
	epd_wxx_hal_write_data_char(0x47);		
	lut_part();
	
	epd_wxx_hal_write_cmd(0x91);		//This command makes the display enter partial mode
	epd_wxx_hal_write_cmd(0x90);		//resolution setting
	epd_wxx_hal_write_data_char (x_start);   //x-start     
	epd_wxx_hal_write_data_char (x_end-1);	 //x-end	

	epd_wxx_hal_write_data_char (y_start >> 8);
	epd_wxx_hal_write_data_char (y_start & 0xFF);   //y-start    
	
	epd_wxx_hal_write_data_char (y_end >> 8);		
	epd_wxx_hal_write_data_char (y_end & 0xFF - 1);  //y-end
	epd_wxx_hal_write_data_char (0x28);	

	epd_wxx_hal_write_cmd(0x10);	  		//writes Old data to SRAM for programming
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&cgnPic_white, size, true);
	epd_wxx_hal_write_cmd(0x11);

	epd_wxx_hal_write_cmd(0x13);			//writes New data to SRAM.
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(buf, size, true);
	epd_wxx_hal_write_cmd(0x11);

	epd_wxx_hal_write_cmd(0x12);		//DISPLAY REFRESH 		             
	epd_wxx_hal_delay_ms(10);     	//!!!The delay here is necessary, 200uS at least!!!     
	lcd_chkstatus();
  
}




//main
unsigned int size;
//unsigned char HRES,VRES_byte1,VRES_byte2;





void EPD_display_init(void);//EPD init 
void full_display(void pic_display(void)); //full  display
void partial_display(uint16_t x_start,uint16_t x_end,uint16_t y_start,uint16_t y_end ,void partial_old(void),void partial_new(void)); //partial display
void lut(void);
void lut1(void);
void lcd_chkstatus(void);
// full display
void pic_display_white(void);
void pic_display1(void);
void pic_display2(void);
void pic_display3(void);
//partial display
void partial_full00(void);
void partial_full01(void);
void partial_full02(void);
void partial_full03(void);
void partial00(void);
void partial01(void);
void partial02(void);
void partial03(void);
void partial04(void);
void partial05(void);
void partial06(void);
void partial07(void);
void partial08(void);
void partial09(void);


unsigned char  EPD_Wxx_ReadDATA(void);


/***************** full screen display picture*************************/
void pic_display_white(void)
{
	unsigned int i;
	epd_wxx_hal_write_cmd(0x10);
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&cgnPic_white, EPD_RES_ROW*EPD_RES_WIDTH, true);
	epd_wxx_hal_delay_ms(2);

	epd_wxx_hal_write_cmd(0x13);
	for(i=0;i<5624;i++)	     
	{
		epd_wxx_hal_write_data_char(0xff);  
	}  
	epd_wxx_hal_delay_ms(2);		 
}

/***************** partial full screen display picture*************************/
void partial_full00(void)
{
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&cgnPic_white, EPD_RES_ROW*EPD_RES_WIDTH, true);
}

void partial_full01(void)
{
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&gImage_1, EPD_RES_ROW*EPD_RES_WIDTH, true);
}
void partial_full02(void)
{
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&gImage_2, EPD_RES_ROW*EPD_RES_WIDTH, true);
}

void partial_full03(void)
{
	EPD_Wxx_DC_D;		// data write
	SPI_WriteBuf(&gImage_3, EPD_RES_ROW*EPD_RES_WIDTH, true);
}

/***************************full display function*************************************/
void full_display(void pic_display(void))
{
	pic_display(); //picture
	lut_full(); //Power settings
	epd_wxx_hal_write_cmd(EPD_Wxx_CMD_DRF);			//DISPLAY REFRESH 	
	epd_wxx_hal_delay_ms(100);	    //!!!The delay here is necessary, 200uS at least!!!     
	lcd_chkstatus();
}

/***************************partial display function*************************************/

void partial_display(uint16_t x_start,uint16_t x_end,uint16_t y_start,uint16_t y_end ,void partial_old(void),void partial_new(void)) //partial display
{
	epd_wxx_hal_write_cmd(0x82);			//vcom_DC setting  	
	epd_wxx_hal_write_data_char (0x08);	
	epd_wxx_hal_write_cmd(0X50);
	epd_wxx_hal_write_data_char(0x47);		
	lut_part();
	
	epd_wxx_hal_write_cmd(0x91);		//This command makes the display enter partial mode
	epd_wxx_hal_write_cmd(0x90);		//resolution setting
	epd_wxx_hal_write_data_char (x_start);   //x-start     
	epd_wxx_hal_write_data_char (x_end-1);	 //x-end	

	epd_wxx_hal_write_data_char (y_start/256);
	epd_wxx_hal_write_data_char (y_start%256);   //y-start    
	
	epd_wxx_hal_write_data_char (y_end/256);		
	epd_wxx_hal_write_data_char (y_end%256-1);  //y-end
	epd_wxx_hal_write_data_char (0x28);	

	epd_wxx_hal_write_cmd(0x10);	       //writes Old data to SRAM for programming
	partial_old();
	epd_wxx_hal_write_cmd(0x13);				 //writes New data to SRAM.
	partial_new();
	
	epd_wxx_hal_write_cmd(0x12);		 //DISPLAY REFRESH 		             
	epd_wxx_hal_delay_ms(10);     //!!!The delay here is necessary, 200uS at least!!!     
	lcd_chkstatus();
  
}


#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
	while (1)
	{
		NRF_LOG_INFO("Wrong parameters value: file %s on line %d\r\n", file, line);
	}
}
#endif

#endif
