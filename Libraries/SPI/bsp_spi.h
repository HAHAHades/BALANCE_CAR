#ifndef _BSP_SPI_H
#define _BSP_SPI_H

#include "stm32f10x.h"
#include "stdio.h"
/*

*/



/*等待超时时间*/
#define SPIx_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIx_LONG_TIMEOUT         ((uint32_t)(10 * SPIx_FLAG_TIMEOUT))


/*信息输出*/
#define SPIx_DEBUG_ON         0  //输出调试信息

#define SPIx_INFO(fmt,arg...)           printf("<<-SPIx-INFO->> "fmt"\n",##arg)
#define SPIx_ERROR(fmt,arg...)          printf("<<-SPIx-ERROR->> "fmt"\n",##arg)
#define SPIx_DEBUG(fmt,arg...)          do{\
                                          if(SPIx_DEBUG_ON)\
                                          printf("<<-SPIx-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)


/*************errorCode************/			  
#define ERRCODE_GET_TX_STA_TIMEOUT	0x00000001 										  
#define ERRCODE_GET_RX_STA_TIMEOUT	0x00000002


/*************************/
#define SPIx_SSOE_ENABLE 0x01  //主模式NSS输出使能
#define SPIx_SSOE_DISABLE 0x00  //禁止主模式NSS脚输出

/**************************************SPI预设******************************************/
#define NSSP_MODE 0 //是否开启主设备软件模拟NSS脉冲模式，NSSP主设备发送时NSS自动拉低，发送完成自动拉高，
                    //但F1系列没有NSSP功能，只有主模式SSM=0，SSOE=1时NSS持续拉低，因此采用硬件NSS脚上拉加主设备SSOE=1及SPI使能/失能模拟NSSP，此功能必须硬件NSS脚上拉。

/****************************************************************************************/

#define SPI1_DMA_REQ_Tx 1 //是否使用SPI1的 Tx DMA 
#define SPI1_DMA_REQ_Rx 0 //是否使用SPI1的 Rx DMA 

#define SPI2_DMA_REQ_Tx 0 //是否使用SPI2的 Tx DMA 
#define SPI2_DMA_REQ_Rx 0 //是否使用SPI2的 Rx DMA 

#define SPI_DUMMY 0x00 //定义假写数据



/***************************************************************/

#if SPI1_SINGLE_LINE //SPI1单线模式
void SPI1_WriteByte(uint8_t Byte);		
void SPI1_ReadByte(uint8_t Byte,uint8_t* RByte);		
#else //双线模式
void 		SPI1_WriteByte	(uint8_t* WByte,uint8_t* RByte);
uint8_t 	SPI1_ReadByte	(uint8_t* WByte,uint8_t* RByte);
#endif /*SINGLE_LINE*/


void BSP_SPIx_Init(SPI_TypeDef* SPIx , uint16_t SPI_Mode, uint16_t SPI_Direction, uint16_t SPI_NSS, uint8_t SPI_SSOE, 
					uint32_t SPI_GPIO_Remap,uint16_t SPI_CPOL, uint16_t SPI_CPHA, uint16_t SPI_FirstBit, uint16_t SPI_BaudRate_Prescaler);
uint32_t SPIx_2Line_RWBytes(SPI_TypeDef* SPIx, uint8_t* WByte, uint8_t* RByte, uint8_t ByteLen);


void SPI1_DMA_SendBytes(uint8_t *SendBuff, uint32_t len, _Bool MemInc);

void 		SPI_Test		(uint16_t* RData,uint16_t SData);
//static void SPI_NVIC_Config(void);
#endif /*_BSP_SPI_H*/
