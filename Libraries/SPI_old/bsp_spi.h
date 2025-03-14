#ifndef _BSP_SPI_H
#define _BSP_SPI_H

#include "stm32f10x.h"

#define Master 1//定义为主机
//#define Master 0//定义为从机


/***************************硬件SPI接口定义*******************************/
#define SPI_NUM SPI1 //指定使用的SPI端口号
#define SPI_CLK_PORT RCC_APB2Periph_SPI1//定义使用的SPI时钟端口
#define SPI_CLKCMD_FUNC RCC_APB2PeriphClockCmd//定义使用的SPI时钟端口使能函数

/***************************SPI的CS/NSS片选引脚定义*******************************/
#define SPI_NSS_PORT GPIOA //NSS片选引脚端口
#define SPI_NSS_PIN GPIO_Pin_4//NSS片选引脚
#define SPI_NSS_CLK RCC_APB2Periph_GPIOA//定义使用的SPI时钟端口
#define SPI_NSS_CLKCMD_FUNC RCC_APB2PeriphClockCmd//定义使用的NSS时钟端口使能函数

/*************************SPI的NSS片选脚电平函数定义*****************************/
#if Master
	#define SPI_NSS_LOW() GPIO_ResetBits(SPI_NSS_PORT, SPI_NSS_PIN) //NSS片选引脚端口置0
	#define SPI_NSS_HIGH() GPIO_SetBits(SPI_NSS_PORT, SPI_NSS_PIN) //NSS片选引脚端口置1
#else
	#define SPI_NSS_LOW() SPI_NSSInternalSoftwareConfig(SPI_NUM, SPI_NSSInternalSoft_Reset) //NSS片选引脚端口置0
	#define SPI_NSS_HIGH() SPI_NSSInternalSoftwareConfig(SPI_NUM, SPI_NSSInternalSoft_Set) //NSS片选引脚端口置1
#endif /*Master*/

/***************************SPI的CK时钟引脚定义*******************************/
#define SPI_CK_PORT GPIOA //定义CK引脚端口
#define SPI_CK_PIN GPIO_Pin_5//定义CK引脚
#define SPI_CK_CLK RCC_APB2Periph_GPIOA//定义CK时钟端口
#define SPI_CK_CLKCMD_FUNC RCC_APB2PeriphClockCmd//定义CK时钟端口使能函数

/***************************SPI的MISO主收从发引脚定义*******************************/
#define SPI_MISO_PORT GPIOA //定义CK引脚端口
#define SPI_MISO_PIN GPIO_Pin_6//定义CK引脚
#define SPI_MISO_CLK RCC_APB2Periph_GPIOA//定义CK时钟端口
#define SPI_MISO_CLKCMD_FUNC RCC_APB2PeriphClockCmd//定义CK时钟端口使能函数

/***************************SPI的MOSI主发从收引脚定义*******************************/
#define SPI_MOSI_PORT GPIOA //定义CK引脚端口
#define SPI_MOSI_PIN GPIO_Pin_7//定义CK引脚
#define SPI_MOSI_CLK RCC_APB2Periph_GPIOA//定义CK时钟端口
#define SPI_MOSI_CLKCMD_FUNC RCC_APB2PeriphClockCmd//定义CK时钟端口使能函数

#define DUMMY 0x00 //定义假写数据







void 	BSP_SPI_Init	(void);
uint8_t Spi_W_RByte		(uint8_t Byte);
void 	Spi_WriteByte	(uint8_t Byte);
uint8_t Spi_ReadByte	(uint8_t Byte);
void 	SPI_Test		(void);


#endif /*_BSP_SPI_H*/
