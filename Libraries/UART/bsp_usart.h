#ifndef _BSP_USART_H
#define _BSP_USART_H

#include "main.h"
#include "stdio.h"

/*                  USARTx_IO_Reamp 对照表
STM32F103C8T6--STM32F401RCT6
USARTx       CTS     RTS     TX       RX      CK          IO_Reamp 
USART1       A11     A12     A9       A10     A8       USART1_IO_Reamp0
USART1       A11     A12     B6       B7      A8       USART1_IO_Reamp1
USART2       A0      A1      A2       A3      A4       USART2_IO_Reamp0
USART2       D3      D4      D5       D6      D7       USART2_IO_Reamp1
USART3       B13     B14     B10      B11     B12      USART3_IO_Reamp0
USART6       --      --      C6       C7      C8       USART6_IO_Reamp0
USART6       --      --      A11      A12     C8       USART6_IO_Reamp1

*/
# define DEBUG_USART   USART1

# define USART_DEBUG  DEBUG_USART
# define  DEBUG_USARTx DEBUG_USART
#define USART_PRINTF_LEN 296 //串口单次发送最大字节

#define USE_USART1 1 //使用USART1
#define USE_USART2 1 //使用USART2
#define USE_USART3 0 //使用USART3
#define USE_USART6 0 //使用USART6

#define USE_SCLK1 0 //不使用同步模式
#define USE_SCLK2 0 //不使用同步模式
#define USE_SCLK3 0 //不使用同步模式
#define USE_SCLK6 0 //不使用同步模式

#define User_BaudRate1 		115200 //定义用户自选波特率
#define User_BaudRate2 		115200 //定义用户自选波特率
#define User_BaudRate3 		115200 //定义用户自选波特率
#define User_BaudRate6 		115200 //定义用户自选波特率


#if USE_USART1
# define USART_REC_LEN1 200 //USART 接受缓存容量byte 应<0x3fff
extern u16 USART_RX_STA1;//串口接受状态 
extern u8 USART_RX_BUF1[USART_REC_LEN1];//保存接收的数据
#endif //USE_USART1

#if USE_USART2
# define USART_REC_LEN2 200 //USART 接受缓存容量byte 应<0x3fff
extern u16 USART_RX_STA2;//串口接受状态 
extern u8 USART_RX_BUF2[USART_REC_LEN2];//保存接收的数据
#endif //USE_USART2

#if USE_USART6
# define USART_REC_LEN6 200 //USART 接受缓存容量byte 应<0x3fff
extern u16 USART_RX_STA6;//串口接受状态 
extern u8 USART_RX_BUF6[USART_REC_LEN6];//保存接收的数据
#endif //USE_USART6



/** @defgroup USART_IO_Reamp
  * @{
  */

#define USART1_IO_Reamp0  ((uint32_t)0x00010000)  //
#define USART1_IO_Reamp1  ((uint32_t)0x00010001)  //
#define USART2_IO_Reamp0  ((uint32_t)0x00020000)  //
#define USART2_IO_Reamp1  ((uint32_t)0x00020001)  //
#define USART3_IO_Reamp0  ((uint32_t)0x00030000)  //
#define USART6_IO_Reamp0  ((uint32_t)0x00060000)  //
#define USART6_IO_Reamp1  ((uint32_t)0x00060001)  //
/**
  * @}
  */




/** @defgroup USART_RX_STA
  * @{
  */
#define USART_RX_STA_E   0 //串口接收为空
#define USART_RX_STA_H   0x4000 //串口接收即将完成
#define USART_RX_STA_C   0x8000 //串口接收完成
#define USART_RX_STA_HCMask   0x3fff //串口接收数据有效位

/**
  * @}
  */


/***************函数申明***************/

#ifdef BSP_USE_F103

USART_InitTypeDef BSP_USARTxStructInit(uint32_t BaudRate);
void BSP_USARTx_Init(USART_TypeDef *USARTx, USART_InitTypeDef USART_InitStruct, uint32_t IO_Reamp);

#endif //BSP_USE_F103

#ifdef BSP_USE_F401
void BSP_USARTx_Init(USART_TypeDef *USARTx, USART_InitTypeDef USART_InitStruct, uint32_t IO_Reamp);
#endif //BSP_USE_F401

#if USE_USART1
void vUSART1_IRQHandler(void);
#endif //#if USE_USART1

#if USE_USART2
void vUSART2_IRQHandler(void);
#endif //#if USE_USART2

#if USE_USART6
void vUSART6_IRQHandler(void);
#endif //#if USE_USART6

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);
uint16_t UsartRecv(USART_TypeDef *USARTx, uint8_t* Rdata);
void UsartSend(USART_TypeDef *USARTx, uint8_t* Sdata, uint16_t Slen);
/*************************************/

#endif /*_BSP_USART_H*/



