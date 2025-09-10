#ifndef _BSP_USART_H
#define _BSP_USART_H

#include "stm32f10x.h"
#include "stdio.h"

/* c8t6引脚
	USART1  TXD：PA9  RXD：PA10  SCLK：PA8 nCTS：PA11  nRTS：PA12
	USART2  TXD：PA2  RXD：PA3  SCLK：PA4 
*/

# define printf_mapping 2 //USART printf 重定向串口号
# define DEBUG_USART   USART_NUM2


# define USART_DEBUG  DEBUG_USART
# define  DEBUG_USARTx DEBUG_USART


# define USART_REC_LEN1 200 //USART 接受缓存容量byte
extern u16 USART_RX_STA1;//串口接受状态 
extern u8 USART_RX_BUF1[USART_REC_LEN1];//保存接收的数据

# define USART_REC_LEN2 200 //USART 接受缓存容量byte
extern u16 USART_RX_STA2;//串口接受状态 
extern u8 USART_RX_BUF2[USART_REC_LEN2];//保存接收的数据


#define USE_USART1 1 //使用USART1
#define USE_USART2 1 //使用USART2


#define USE_SCLK1 0 //不使用同步模式
#define USE_SCLK2 0 //不使用同步模式

#define User_BaudRate1 		115200 //定义用户自选波特率
#define User_BaudRate2 		115200 //定义用户自选波特率

/***************************USART接口引脚定义*******************************/
#if USE_USART1

#define USART_NUM1 			USART1
#define USART_CLK1 			RCC_APB2Periph_USART1
#define USART_CLKCMD_FUNC1 	RCC_APB2PeriphClockCmd
#define USART_IRQ1 			USART1_IRQn
#define USART_IRQHandler1 	USART1_IRQHandler


/*发送引脚TXD*/
#define USART_TXD_PORT1 			GPIOA 
#define USART_TXD_PIN1  			GPIO_Pin_9
#define USART_TXD_CLK1  			RCC_APB2Periph_GPIOA
#define USART_TXD_CLKCMD_FUNC1 		RCC_APB2PeriphClockCmd


/*接收引脚RXD*/
#define USART_RXD_PORT1 			GPIOA 
#define USART_RXD_PIN1  			GPIO_Pin_10
#define USART_RXD_CLK1  			RCC_APB2Periph_GPIOA
#define USART_RXD_CLKCMD_FUNC1 		RCC_APB2PeriphClockCmd	


/*时钟引脚SCLK*/
#if USE_SCLK1
#define USART_SCLK_PORT1 			GPIOA 
#define USART_SCLK_PIN1 			GPIO_Pin_8
#define USART_SCLK_CLK1  			RCC_APB2Periph_GPIOA
#define USART_SCLK_CLKCMD_FUNC1 	RCC_APB2PeriphClockCmd						
#endif /*USE_SCLK*/			



/*清除发送引脚nCTS*/
#define USART_nCTS_PORT1 			GPIOA 
#define USART_nCTS_PIN1  			GPIO_Pin_11
#define USART_nCTS_CLK1  			RCC_APB2Periph_GPIOA
#define USART_nCTS_CLKCMD_FUNC1 	RCC_APB2PeriphClockCmd


/*发送请求引脚nRTS*/
#define USART_nRTS_PORT1 			GPIOA 
#define USART_nRTS_PIN1  			GPIO_Pin_12
#define USART_nRTS_CLK1  			RCC_APB2Periph_GPIOA
#define USART_nRTS_CLKCMD_FUNC1 	RCC_APB2PeriphClockCmd

#endif /*USE_USART1*/


#if USE_USART2

#define USART_NUM2 				USART2
#define USART_CLK2 				RCC_APB1Periph_USART2
#define USART_CLKCMD_FUNC2 		RCC_APB1PeriphClockCmd
#define USART_IRQ2 				USART2_IRQn
#define USART_IRQHandler2 		USART2_IRQHandler

/*发送引脚TXD*/
#define USART_TXD_PORT2 			GPIOA 
#define USART_TXD_PIN2  			GPIO_Pin_2
#define USART_TXD_CLK2  			RCC_APB2Periph_GPIOA
#define USART_TXD_CLKCMD_FUNC2 		RCC_APB2PeriphClockCmd


/*接收引脚RXD*/
#define USART_RXD_PORT2 			GPIOA 
#define USART_RXD_PIN2  			GPIO_Pin_3
#define USART_RXD_CLK2  			RCC_APB2Periph_GPIOA
#define USART_RXD_CLKCMD_FUNC2 		RCC_APB2PeriphClockCmd	


/*时钟引脚SCLK*/
#if USE_SCLK2
#define USART_SCLK_PORT2 			GPIOA 
#define USART_SCLK_PIN2  			GPIO_Pin_4
#define USART_SCLK_CLK2  			RCC_APB2Periph_GPIOA
#define USART_SCLK_CLKCMD_FUNC2 	RCC_APB2PeriphClockCmd						
#endif /*USE_SCLK*/	


#endif /*USE_USART2*/



/***************函数申明***************/
void 			Usart1_Init				(int BaudRate);
void 			Usart2_Init				(int BaudRate);
void 			USART_Config			(uint8_t usart_num, int BaudRate);
void 			USART_SendByte			(USART_TypeDef* USARTx,uint8_t Byte);
static void 	USART_NVIC_Config		(int usart_num);
void 			USART_SendString		(USART_TypeDef* pUSARTx,char* pStr);
void 			USART_SendNum			(USART_TypeDef* pUSARTx,uint16_t Num);
void 			vUSART1_IRQHandler		(void);
void 			vUSART2_IRQHandler		(void);
void            UsartPrintf             (USART_TypeDef *USARTx, char *fmt,...);
void 			Usart_SendString		(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);
/*************************************/

#endif /*_BSP_USART_H*/



