#ifndef _BSP_SPI_H
#define _BSP_SPI_H


#include "stm32f10x.h"
#include "bsp_usart.h"
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

typedef struct 
{

	SPI_TypeDef* SPIx;
	uint32_t SPI_GPIO_Remap;// @ref SPIx_IO_Reamp
	SPI_InitTypeDef SPI_InitStruct;
	GPIO_TypeDef* SPI_NSS_PORTx;//NSS引脚无需输入，初始化时按SPIx的默认NSS配置
	uint16_t SPI_NSS_PINx;
}BSP_SPIx_TypeDef;


#define SPIx_NSS_LOW(A)				GPIO_ResetBits(A.SPI_NSS_PORTx, A.SPI_NSS_PINx)
#define SPIx_NSS_HIGH(A)	 		GPIO_SetBits(A.SPI_NSS_PORTx, A.SPI_NSS_PINx)


/* 标志代码 */
#define SPIx_RET_Flag_OK  	        ((uint8_t)0x00)	//								  
#define SPIx_RET_Flag_Init_Error  	((uint8_t)0x01)	//初始化错误


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

uint8_t BSP_SPIx_Init(BSP_SPIx_TypeDef* BSP_SPIx_Struct);
// SPI_InitTypeDef BSP_SPIxStructInit(uint16_t SPI_Mode, uint16_t SPI_Direction, uint16_t SPI_NSS, uint8_t SPI_SSOE, uint16_t SPI_CPOL, 
// 								   uint16_t SPI_CPHA, uint16_t SPI_FirstBit, uint16_t SPI_BaudRatePrescaler, uint16_t SPI_DataSize);
SPI_InitTypeDef BSP_SPIxStructInit(uint16_t SPI_Mode, uint16_t SPI_Direction);
uint32_t SPIx_2Line_RWBytes(SPI_TypeDef* SPIx, uint8_t* WByte, uint8_t* RByte, uint8_t ByteLen);
//static void SPI_NVIC_Config(void);
#endif /*_BSP_SPI_H*/
