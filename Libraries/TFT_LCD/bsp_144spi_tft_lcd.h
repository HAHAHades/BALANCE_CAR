#ifndef _144SPI_TFT_LCD_H
#define _144SPI_TFT_LCD_H


/*
本文件适用于  四线（有DC命令数据切换线）串行控制1.44‘ 128*128 单线SPI TFT
驱动芯片为ST7735S读取最小周期为150ns（6.7MHz）读取时，片选引脚需保持65ns
*/

//本程序适用与STM32F103C8      SPI1		        SPI2
//       	             GND   电源地
//      	             VCC   接5V或3.3v电源
//      	             SCL   接PA5（SCL）		PB13
//      		         SDA   接PA7（SDA）		PB15
//       	  	 	     RES   接PB0
//        	 		     DC    接PB1
//       	 		     CS    接PA4 （NSS）    PB12
//						 BL	   接PB10

/**********************************************************************************************/
#include "stm32f10x.h"
#include "bsp_TFT_display.h"
#include "bsp_spi.h"
#include "delay.h" 




#define X_MAX_PIXEL 128 //定义X轴最大像素范围

#define Y_MAX_PIXEL 128 //定义Y轴最大像素范围

/**************定义颜色*******************/
#define RED  	0xf800		//1111 1_000 000_0 0000
#define GREEN	0x07e0		//0000 0_111 111_0 0000 
#define BLUE 	0x001f		//0000 0_000 000_1 1111 
#define WHITE	0xffff		//1111 1_111 111_1 1111 
#define BLACK	0x0000		//0000 0_000 000_0 0000 
#define YELLOW  0xFFE0		//1111 1_111 111_0 0000
#define GRAY0   0xEF7D   	//1110 1_111 011_1 1101
#define GRAY1   0x8410      //1000 0_100 000_1 0000
#define GRAY2   0x4208      //0100 0_010 000_0 1000
                        

/*********************定义LCD引脚接口**********************/
///*时钟引脚*/
//#define LCD_SCL_PORT 			SPI_CK_PORT
//#define LCD_SCL_PIN 			SPI_CK_PIN
//#define LCD_SCL_SCLK 			SPI_CK_CLK
//#define LCD_SCL_SCLK_CMDFUNC 	SPI_CK_CLKCMD_FUNC

///*数据引脚*/
//#define LCD_SDA_PORT 			SPI_MOSI_PORT
//#define LCD_SDA_PIN 			SPI_MOSI_PIN
//#define LCD_SDA_SCLK 			SPI_MOSI_CLK
//#define LCD_SDA_SCLK_CMDFUNC 	SPI_MOSI_CLKCMD_FUNC

/*片选引脚*/ //CS脚直接使用SPI的NSS，在spi里配置其IO模式
//#define LCD_CS_PORT 			SPI_NSS_PORT
//#define LCD_CS_PIN 				SPI_NSS_PIN
//#define LCD_CS_SCLK 			SPI_NSS_CLK
//#define LCD_CS_SCLK_CMDFUNC 	SPI_NSS_CLKCMD_FUNC

/*********************使用的SPI************************/
#define LCD_SPIx SPI1  //使用SPI1
#define LCD_CS_LOW() 			SPI1_NSS_LOW()	
#define LCD_CS_HIGH() 			SPI1_NSS_HIGH()	


/*复位引脚*/
#define LCD_RES_PORT 			GPIOB
#define LCD_RES_PIN 			GPIO_Pin_8
#define LCD_RES_SCLK 			RCC_APB2Periph_GPIOB
#define LCD_RES_SCLK_CMDFUNC 	RCC_APB2PeriphClockCmd
#define LCD_RES_LOW()			GPIO_ResetBits(LCD_RES_PORT, LCD_RES_PIN)
#define LCD_RES_HIGH() 			GPIO_SetBits(LCD_RES_PORT, LCD_RES_PIN)

/*数据/命令切换引脚*/
#define LCD_DC_PORT 			GPIOB
#define LCD_DC_PIN 				GPIO_Pin_9
#define LCD_DC_SCLK 			RCC_APB2Periph_GPIOB
#define LCD_DC_SCLK_CMDFUNC 	RCC_APB2PeriphClockCmd
#define LCD_DC_LOW()			GPIO_ResetBits(LCD_DC_PORT, LCD_DC_PIN)
#define LCD_DC_HIGH()			GPIO_SetBits(LCD_DC_PORT, LCD_DC_PIN)


/*背光调解引脚*/
#define LCD_BL_PORT 			GPIOB
#define LCD_BL_PIN 				GPIO_Pin_10
#define LCD_BL_SCLK 			RCC_APB2Periph_GPIOB
#define LCD_BL_SCLK_CMDFUNC 	RCC_APB2PeriphClockCmd
#define LCD_BL_LOW()			GPIO_ResetBits(LCD_BL_PORT, LCD_BL_PIN)
#define LCD_BL_HIGH()			GPIO_SetBits(LCD_BL_PORT, LCD_BL_PIN)



void LCD_Test(void);
void LCD_WriteCMD(uint8_t Byte);
void LCD_WriteData_8bit(uint8_t Byte);
void LCD_WriteData_16bit(uint16_t Byte);
void LCD_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end);
void LCD_ClearGlass(uint16_t Color);
void LCD_DMA_ClearGlass(uint16_t Color);
void LCD_DispDot(uint8_t x,uint8_t y,uint16_t Color);
void LCD_GPIO_Init(void);
void LCD_Reset(void);
void LCD_Init(void);
void delay(uint32_t count);
uint8_t LCD_ReadData8bit(uint8_t CMD);
uint32_t LCD_GetDotRGB(uint16_t x,uint16_t y,uint16_t* RByte);
uint16_t LCD_ReadData16bit(uint8_t CMD);
void LCD_ReadData32bit(uint8_t CMD,uint16_t* RByte);
#endif /*_144SPI_TFT_LCD_H*/
