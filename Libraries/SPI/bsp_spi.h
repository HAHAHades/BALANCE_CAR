#ifndef _BSP_SPI_H
#define _BSP_SPI_H



#include "main.h"

/*
SPI双线模式：
双机共地
主机发送正常，从机接收正常
主机接收异常
*/

/*            F103C      SPIx_IO_Reamp 对照表
SPIx       NSS     SCK     MISO        MOSI            IO_Reamp 
SPI1       A4      A5      A6          A7          SPI1_IO_Reamp0
SPI1       A15     B3      B4          B5          SPI1_IO_Reamp1
SPI2       B12     B13     B14         B15         SPI2_IO_Reamp0

*/

/*            F401R      SPIx_IO_Reamp 对照表
SPIx       NSS     SCK     MISO        MOSI            IO_Reamp 
SPI1       A4      A5      A6          A7          SPI1_IO_Reamp0
SPI1       A15     B3      B4          B5          SPI1_IO_Reamp1
SPI2       B12     B13     B14         B15         SPI2_IO_Reamp0
SPI2       B9      B10     B14         B15         SPI2_IO_Reamp1
SPI2       B12     B13     C2          C3          SPI2_IO_Reamp2
SPI2       B9      B10     C2          C3          SPI2_IO_Reamp3
SPI3       A15     B3      B4          B5          SPI3_IO_Reamp0
SPI3       A4      B3      B4          B5          SPI3_IO_Reamp1
SPI3       A15     C10     C11         C12         SPI3_IO_Reamp2
SPI3       A4      C10     C11         C12         SPI3_IO_Reamp3
SPI4       E11     E12     E13         E14         SPI4_IO_Reamp0
SPI4       E4      E2      E5          E6          SPI4_IO_Reamp1
*/

/** @defgroup SPIx_IO_Reamp
  * @{
  */

#define SPI1_IO_Reamp0  ((uint32_t)0x00010000)  //
#define SPI1_IO_Reamp1  ((uint32_t)0x00010001)  //
#define SPI2_IO_Reamp0  ((uint32_t)0x00020000)  //
#define SPI2_IO_Reamp1  ((uint32_t)0x00020001)  //
#define SPI2_IO_Reamp2  ((uint32_t)0x00020002)  //
#define SPI2_IO_Reamp3  ((uint32_t)0x00020003)  //
#define SPI3_IO_Reamp0  ((uint32_t)0x00030000)  //
#define SPI3_IO_Reamp1  ((uint32_t)0x00030001)  //
#define SPI3_IO_Reamp2  ((uint32_t)0x00030002)  //
#define SPI3_IO_Reamp3  ((uint32_t)0x00030003)  //
#define SPI4_IO_Reamp0  ((uint32_t)0x00040000)  //
#define SPI4_IO_Reamp1  ((uint32_t)0x00040001)  //

/**
  * @}
  */


typedef struct BSP_spix_TypeDef
{

	SPI_TypeDef* SPIx;
	uint32_t SPI_GPIO_Remap;//@ref SPIx_IO_Reamp
	SPI_InitTypeDef SPI_InitStruct;
	GPIO_TypeDef* SPI_NSS_PORTx;
	uint16_t SPI_NSS_PINx;
	void (*SPI_NSS_LOW)(struct BSP_spix_TypeDef*);
	void (*SPI_NSS_HIGH)(struct BSP_spix_TypeDef*);
}BSP_SPIx_TypeDef;


extern BSP_SPIx_TypeDef BSP_SPI1_Struct;
extern BSP_SPIx_TypeDef BSP_SPI2_Struct;




/* 标志代码 */
#define SPIx_RET_Flag_OK  	        ((uint8_t)0x00)	//								  
#define SPIx_RET_Flag_Init_Error  	((uint8_t)0x01)	//初始化错误


#ifdef SPI1_IO_Reamp0
#define SPI1_NSS_LOW()				GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI1_NSS_HIGH()	 			GPIO_SetBits(GPIOA, GPIO_Pin_4)
#endif //SPI1_IO_Reamp0

// #ifdef SPI1_IO_Reamp1
// #define SPI1_NSS_LOW()				GPIO_ResetBits(GPIOA, GPIO_Pin_15)
// #define SPI1_NSS_HIGH()	 			GPIO_SetBits(GPIOA, GPIO_Pin_15)
// #endif //SPI1_IO_Reamp1

#ifdef SPI2_IO_Reamp0
#define SPI2_NSS_LOW()				GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define SPI2_NSS_HIGH()	 			GPIO_SetBits(GPIOB, GPIO_Pin_12)
#endif //SPI2_IO_Reamp0


#if 0

/**************************************SPI预设******************************************/
#define USE_SPI1 //使用SPI1
//#define USE_SPI2 //使用SPI2


#define Master 1//定义为主机
//#define Master 0//定义为从机

#define NSS_Soft  1//NSS为软件控制
//#define NSS_Soft  0//NSS为硬件控制

#define SINGLE_LINE  1//定义单线
//#define SINGLE_LINE  0//定义双线

#if SINGLE_LINE
	//#define SINGLE_LINE_ReadOnly 1 //定义单线读模式
	#define SINGLE_LINE_ReadOnly 0 //定义单线写模式
#endif /*SINGLE_LINE*/
/****************************************************************************************/

#ifdef USE_SPI1

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
#define SPI_CK_PORT GPIOA 
#define SPI_CK_PIN GPIO_Pin_5
#define SPI_CK_CLK RCC_APB2Periph_GPIOA
#define SPI_CK_CLKCMD_FUNC RCC_APB2PeriphClockCmd

/***************************SPI的MISO主收从发引脚定义*******************************/
#define SPI_MISO_PORT GPIOA 
#define SPI_MISO_PIN GPIO_Pin_6
#define SPI_MISO_CLK RCC_APB2Periph_GPIOA
#define SPI_MISO_CLKCMD_FUNC RCC_APB2PeriphClockCmd

/***************************SPI的MOSI主发从收引脚定义*******************************/
#define SPI_MOSI_PORT GPIOA 
#define SPI_MOSI_PIN GPIO_Pin_7
#define SPI_MOSI_CLK RCC_APB2Periph_GPIOA
#define SPI_MOSI_CLKCMD_FUNC RCC_APB2PeriphClockCmd


#endif //USE_SPI1

#ifdef USE_SPI2
/***************************硬件SPI接口定义*******************************/
#define SPI_NUM SPI2 //指定使用的SPI端口号
#define SPI_CLK_PORT RCC_APB1Periph_SPI2//定义使用的SPI时钟端口
#define SPI_CLKCMD_FUNC RCC_APB1PeriphClockCmd//定义使用的SPI时钟端口使能函数

/***************************SPI的CS/NSS片选引脚定义*******************************/
#define SPI_NSS_PORT GPIOB //NSS片选引脚端口
#define SPI_NSS_PIN GPIO_Pin_12//NSS片选引脚
#define SPI_NSS_CLK RCC_APB2Periph_GPIOB//定义使用的SPI时钟端口
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
#define SPI_CK_PORT GPIOB 
#define SPI_CK_PIN GPIO_Pin_13
#define SPI_CK_CLK RCC_APB2Periph_GPIOB
#define SPI_CK_CLKCMD_FUNC RCC_APB2PeriphClockCmd

/***************************SPI的MISO主收从发引脚定义*******************************/
#define SPI_MISO_PORT GPIOB 
#define SPI_MISO_PIN GPIO_Pin_14
#define SPI_MISO_CLK RCC_APB2Periph_GPIOB
#define SPI_MISO_CLKCMD_FUNC RCC_APB2PeriphClockCmd

/***************************SPI的MOSI主发从收引脚定义*******************************/
#define SPI_MOSI_PORT GPIOB 
#define SPI_MOSI_PIN GPIO_Pin_15
#define SPI_MOSI_CLK RCC_APB2Periph_GPIOB
#define SPI_MOSI_CLKCMD_FUNC RCC_APB2PeriphClockCmd

#endif //USE_SPI2

#endif //if 0

/*等待超时时间*/
#define SPIx_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIx_LONG_TIMEOUT         ((uint32_t)(10 * SPIx_FLAG_TIMEOUT))


// /*信息输出*/
// #define SPIx_DEBUG_ON         0  //输出调试信息

// #define SPIx_INFO(fmt,arg...)           printf("<<-SPIx-INFO->> "fmt"\n",##arg)
// #define SPIx_ERROR(fmt,arg...)          printf("<<-SPIx-ERROR->> "fmt"\n",##arg)
// #define SPIx_DEBUG(fmt,arg...)          do{\
//                                           if(SPIx_DEBUG_ON)\
//                                           printf("<<-SPIx-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
//                                           }while(0)


/*************errorCode************/			  
#define ERRCODE_GET_TX_STA_TIMEOUT	0x00000001 										  
#define ERRCODE_GET_RX_STA_TIMEOUT	0x00000002

#define DUMMY 0x00 //定义假写数据


/**********************************************************************************/

#if SINGLE_LINE //单线模式
void Spi_WriteByte(uint8_t Byte);		
void Spi_ReadByte(uint8_t Byte,uint8_t* RByte);		
#else //双线模式
void 		Spi_WriteByte	(uint8_t* WByte,uint8_t* RByte);
uint8_t 	Spi_ReadByte	(uint8_t* WByte,uint8_t* RByte);
#endif /*SINGLE_LINE*/

void BSP_SPIx_NSS_STA_LOW(BSP_SPIx_TypeDef* BSP_SPIx_Struct);
void BSP_SPIx_NSS_STA_HIGH(BSP_SPIx_TypeDef* BSP_SPIx_Struct);
uint8_t BSP_SPIx_Init(BSP_SPIx_TypeDef* BSP_SPIx_Struct);
// SPI_InitTypeDef BSP_SPIxStructInit(uint16_t SPI_Mode, uint16_t SPI_Direction, uint16_t SPI_NSS, uint8_t SPI_SSOE, uint16_t SPI_CPOL, 
// 								   uint16_t SPI_CPHA, uint16_t SPI_FirstBit, uint16_t SPI_BaudRatePrescaler, uint16_t SPI_DataSize);
SPI_InitTypeDef BSP_SPIxStructInit(uint16_t SPI_Mode, uint16_t SPI_Direction);
uint32_t SPIx_2Line_RWBytes(SPI_TypeDef* SPIx, uint8_t* WByte, uint8_t* RByte, uint8_t ByteLen);
//static void SPI_NVIC_Config(void);
#endif /*_BSP_SPI_H*/
