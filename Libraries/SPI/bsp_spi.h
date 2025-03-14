#ifndef _BSP_SPI_H
#define _BSP_SPI_H

#include "stm32f10x.h"

/*
SPI双线模式：
双机共地
主机发送正常，从机接收正常
主机接收异常
*/


/**************************************SPI预设******************************************/
#define USE_SPI1    //使用SPI1

#define SPI1_Master 1//定义SPI1为主机

#define SPI1_NSS_Soft  1//SPI1  NSS为软件控制

#define SPI1_SINGLE_LINE 0  //SPI1 为单线模式




#define USE_SPI2    //使用SPI2

#define SPI2_Master 0 //定义SPI2为从机

#define SPI2_NSS_Soft  0 //SPI2  NSS为硬件控制

#define SPI2_SINGLE_LINE 0  //SPI2 为单线模式

/****************************************************************************************/

#ifdef USE_SPI1


#define SPI1_DMA_REQ_Tx 1 //是否使用SPI1的 Tx DMA 
#define SPI1_DMA_REQ_Rx 0 //是否使用SPI1的 Rx DMA 

///***************************SPI1的CS/NSS片选引脚定义*******************************/
#define SPI1_NSS_PORT GPIOA //NSS片选引脚端口
#define SPI1_NSS_PIN GPIO_Pin_4//NSS片选引脚
#define SPI1_NSS_CLK RCC_APB2Periph_GPIOA//定义使用的SPI时钟端口
#define SPI1_NSS_CLKCMD_FUNC RCC_APB2PeriphClockCmd//定义使用的NSS时钟端口使能函数

/*************************SPI的NSS片选脚电平函数定义*****************************/
	#if SPI1_Master
		#define SPI1_NSS_LOW() GPIO_ResetBits(SPI1_NSS_PORT, SPI1_NSS_PIN) //NSS片选引脚端口置0
		#define SPI1_NSS_HIGH() GPIO_SetBits(SPI1_NSS_PORT, SPI1_NSS_PIN) //NSS片选引脚端口置1
	#else
		#define SPI1_NSS_LOW() SPI_NSSInternalSoftwareConfig(SPI1, SPI1_NSSInternalSoft_Reset) //NSS片选引脚端口置0
		#define SPI1_NSS_HIGH() SPI_NSSInternalSoftwareConfig(SPI1, SPI1_NSSInternalSoft_Set) //NSS片选引脚端口置1
	#endif /*SPI1_Master*/

#endif //USE_SPI1



#ifdef USE_SPI2

#define SPI2_DMA_REQ_Tx 0 //是否使用SPI2的 Tx DMA 
#define SPI2_DMA_REQ_Rx 0 //是否使用SPI2的 Rx DMA 

/***************************SPI的CS/NSS片选引脚定义*******************************/
#define SPI2_NSS_PORT GPIOB //NSS片选引脚端口
#define SPI2_NSS_PIN GPIO_Pin_12//NSS片选引脚
#define SPI2_NSS_CLK RCC_APB2Periph_GPIOB//定义使用的SPI时钟端口
#define SPI2_NSS_CLKCMD_FUNC RCC_APB2PeriphClockCmd//定义使用的NSS时钟端口使能函数

/*************************SPI2的NSS片选脚电平函数定义*****************************/
	#if SPI2_Master
		#define SPI2_NSS_LOW() GPIO_ResetBits(SPI2_NSS_PORT, SPI2_NSS_PIN) //NSS片选引脚端口置0
		#define SPI2_NSS_HIGH() GPIO_SetBits(SPI2_NSS_PORT, SPI2_NSS_PIN) //NSS片选引脚端口置1
	#else
		#define SPI2_NSS_LOW() SPI_NSSInternalSoftwareConfig(SPI2, SPI_NSSInternalSoft_Reset) //NSS片选引脚端口置0
		#define SPI2_NSS_HIGH() SPI_NSSInternalSoftwareConfig(SPI2, SPI_NSSInternalSoft_Set) //NSS片选引脚端口置1
	#endif /*SPI2_Master*/

#endif //USE_SPI2





#define DUMMY 0x00 //定义假写数据




void delay(uint32_t count);
#if SPI1_SINGLE_LINE //SPI1单线模式
void SPI1_WriteByte(uint8_t Byte);		
void SPI1_ReadByte(uint8_t Byte,uint8_t* RByte);		
#else //双线模式
void 		SPI1_WriteByte	(uint8_t* WByte,uint8_t* RByte);
uint8_t 	SPI1_ReadByte	(uint8_t* WByte,uint8_t* RByte);
#endif /*SINGLE_LINE*/


void BSP_SPIx_Init(SPI_TypeDef* SPIx , uint16_t SPI_Mode, uint16_t SPI_Direction, uint16_t SPI_NSS );

void SPI1_DMA_SendBytes(uint8_t *SendBuff, uint32_t len, _Bool MemInc);

void 		SPI_Test		(uint16_t* RData,uint16_t SData);
//static void SPI_NVIC_Config(void);
#endif /*_BSP_SPI_H*/
