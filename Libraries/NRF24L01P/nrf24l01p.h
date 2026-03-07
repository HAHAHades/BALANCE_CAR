#ifndef _NRF24L01P_H
#define _NRF24L01P_H

#include "stm32f10x.h"
#include "stdio.h"
#include "bsp_usart.h"
/*
  推荐频率10Mhz以内
*/
/*  引脚：  NRF24L01    SPI1	SPI2	SPI1 Remap
			1  GND		
			2  3.3		
			3  CE		
			4  CSN		PA4		PB12		PA15
			5  SCK		PA5		PB13		PB3
			6  MOSI		PA7		PB15		PB5
			7  MISO		PA6		PB14		PB4
			8  IRQ		
*/

#define NRF_DEBUG_ON         0  //输出调试信息
#define TEST_NRF24L01  0  //是否仅测试NRF24L01模块

#define NRF24L01_IRQ_EXTI 1 //是否使用外部中断处理NRF状态

#define Use_Default_Pin_Config 1 //是否使用默认的引脚配置，包括SPI，中断，片选等引脚

/**************定义NRF使用的SPI**************/
#if Use_Default_Pin_Config //使用默认引脚配置

#define NRF24L01_SPIx  				SPI1 
#define NRF24L01_SPIx_Direction  	SPI_Direction_2Lines_FullDuplex 
#define NRF24L01_SPIx_GPIO_Remap  	0x00 
#define NRF24L01_SPIx_CPOL  		SPI_CPOL_Low 
#define NRF24L01_SPIx_CPHA 			SPI_CPHA_1Edge 
#define NRF24L01_SPIx_FirstBit 		SPI_FirstBit_MSB 
#define NRF24L01_SPIx_BaudRate_Psc 	SPI_BaudRatePrescaler_8 

#else //Use_Default_Pin_Config 用户自定义引脚配置
/***********************用户自定义引脚**********************************/
#define NRF24L01_SPIx  				SPI1 
#define NRF24L01_SPIx_Direction  	SPI_Direction_2Lines_FullDuplex 
#define NRF24L01_SPIx_GPIO_Remap  	0x00 
#define NRF24L01_SPIx_CPOL  		SPI_CPOL_Low 
#define NRF24L01_SPIx_CPHA 			SPI_CPHA_1Edge 
#define NRF24L01_SPIx_FirstBit 		SPI_FirstBit_MSB 
#define NRF24L01_SPIx_BaudRate_Psc 	SPI_BaudRatePrescaler_8 


#endif //Use_Default_Pin_Config

/*************定义NRF24L01引脚***************/

#if Use_Default_Pin_Config

//CE模块控制引脚
#define NRF24L01_CE_PORT 				GPIOB
#define NRF24L01_CE_PIN 				GPIO_Pin_1
#define NRF24L01_CE_CLK 				RCC_APB2Periph_GPIOB
#define NRF24L01_CE_CLKCMD_FUNC 	    RCC_APB2PeriphClockCmd//定义CE时钟端口使能函数
#define NRF24L01_CE_LOW()  				GPIO_ResetBits(NRF24L01_CE_PORT, NRF24L01_CE_PIN) //CE引脚端口置0
#define NRF24L01_CE_HIGH()				GPIO_SetBits(NRF24L01_CE_PORT, NRF24L01_CE_PIN)   //CE引脚端口置1


//IRQ中断输出引脚
#define NRF24L01_IRQ_PORT 				GPIOB
#define NRF24L01_IRQ_PIN 				GPIO_Pin_0
#define NRF24L01_IRQ_CLK 				RCC_APB2Periph_GPIOB//定义时钟端口
#define NRF24L01_IRQ_CLKCMD_FUNC 	    RCC_APB2PeriphClockCmd//定义时钟端口使能函数

#if NRF24L01_IRQ_EXTI
#define NRF24L01_IRQ_NVIC_IRQChannel	EXTI0_IRQn//定义中断通道
#define NRF24L01_IRQ_EXTI_Line			EXTI_Line0//定义中断线
#endif //NRF24L01_IRQ_EXTI

//NSS片选引脚
#define NRF24L01_NSS_PORT 				GPIOA
#define NRF24L01_NSS_PIN 				GPIO_Pin_4
#define NRF24L01_NSS_CLK 				RCC_APB2Periph_GPIOA//定义时钟端口
#define NRF24L01_NSS_CLKCMD_FUNC 	    RCC_APB2PeriphClockCmd//定义时钟端口使能函数

#define NRF24L01_NSS_LOW()				GPIO_ResetBits(NRF24L01_NSS_PORT, NRF24L01_NSS_PIN) //NSS片选引脚端口置0
#define NRF24L01_NSS_HIGH()				GPIO_SetBits(NRF24L01_NSS_PORT, NRF24L01_NSS_PIN) //NSS片选引脚端口置1

#else //Use_Default_Pin_Config

//CE模块控制引脚
#define NRF24L01_CE_PORT 				GPIOB
#define NRF24L01_CE_PIN 				GPIO_Pin_3
#define NRF24L01_CE_CLK 				RCC_APB2Periph_GPIOB
#define NRF24L01_CE_CLKCMD_FUNC 	    RCC_APB2PeriphClockCmd//定义CE时钟端口使能函数
#define NRF24L01_CE_LOW()  				GPIO_ResetBits(NRF24L01_CE_PORT, NRF24L01_CE_PIN) //CE引脚端口置0
#define NRF24L01_CE_HIGH()				GPIO_SetBits(NRF24L01_CE_PORT, NRF24L01_CE_PIN)   //CE引脚端口置1


//IRQ中断输出引脚
#define NRF24L01_IRQ_PORT 				GPIOB
#define NRF24L01_IRQ_PIN 				GPIO_Pin_4
#define NRF24L01_IRQ_CLK 				RCC_APB2Periph_GPIOB//定义时钟端口
#define NRF24L01_IRQ_CLKCMD_FUNC 	    RCC_APB2PeriphClockCmd//定义时钟端口使能函数

#if NRF24L01_IRQ_EXTI
#define NRF24L01_IRQ_NVIC_IRQChannel	EXTI4_IRQn//定义中断通道
#define NRF24L01_IRQ_EXTI_Line			EXTI_Line4//定义中断线
#endif //NRF24L01_IRQ_EXTI

//NSS片选引脚
#define NRF24L01_NSS_PORT 				GPIOA
#define NRF24L01_NSS_PIN 				GPIO_Pin_4
#define NRF24L01_NSS_CLK 				RCC_APB2Periph_GPIOA//定义时钟端口
#define NRF24L01_NSS_CLKCMD_FUNC 	    RCC_APB2PeriphClockCmd//定义时钟端口使能函数

#define NRF24L01_NSS_LOW()				GPIO_ResetBits(NRF24L01_NSS_PORT, NRF24L01_NSS_PIN) //NSS片选引脚端口置0
#define NRF24L01_NSS_HIGH()				GPIO_SetBits(NRF24L01_NSS_PORT, NRF24L01_NSS_PIN) //NSS片选引脚端口置1

#endif //Use_Default_Pin_Config

/*等待超时时间*/
#define NRF_LONG_TIMEOUT         ((uint32_t)0x00100000)
#define NRF_FAST_TIMEOUT         ((uint32_t)0x00000002)


/*信息输出*/


#define NRF_INFO(fmt,arg...)           printf("<<-NRF-INFO->> "fmt"\n",##arg)
#define NRF_ERROR(fmt,arg...)          printf("<<-NRF-ERROR->> "fmt"\n",##arg)
#define NRF_DEBUG(fmt,arg...)          do{\
                                          if(NRF_DEBUG_ON)\
                                          UsartPrintf(USART_DEBUG , "<<-NRF-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)


/*************errorCode************/			  
#define NRF_ERRCODE_GET_TX_STA_TIMEOUT	0x01 										  
#define NRF_ERRCODE_GET_RX_STA_TIMEOUT	0x02
#define NRF_ERRCODE_TX_MAX				0x03


/***************************传输模式******************************/
#define NRF_MODE_TX     0x01 //发送模式
#define NRF_MODE_RX     0x00 //接收模式

/***************************定义模式切换Flag******************************/
#define SWFlag_2RxSwAddr     0x11 //切换为接收并修改发送地址
#define SWFlag_2TxSwAddr     0x01 //切换为发送并修改发送地址									  
#define SWFlag_Only2Rx       0x10 //只切换为接收
#define SWFlag_Only2Tx       0x00 //只切换为发送


										  
/****************************定义NRF24L01指令与寄存器地址********************************/
//****************************************************************//
// SPI(nRF24L01) commands 
#define NRF_READ_REG_CMD        0x00  // Define read command to register  //写入000X XXXX(X为寄存器地址)表示读对应寄存器，返回对应寄存器的值
#define NRF_WRITE_REG_CMD        0x20  // Define write command to register //写入001X XXXX(X为寄存器地址)表示写对应寄存器
#define RD_RX_PLOAD_CMD     0x61  // Define RX payload register address
#define WR_TX_PLOAD_CMD      0xA0  // Define TX payload register address
#define FLUSH_TX_CMD         0xE1  // Define flush TX register command
#define FLUSH_RX_CMD         0xE2  // Define flush RX register command
#define REUSE_TX_PL_CMD      0xE3  // Define reuse TX payload register command
#define NOP             0xFF  // Define No Operation, might be used to read status register



//***************************************************//
// SPI(nRF24L01) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address

#define NRF_DUMMY NOP //定义假写数据
#define Addr_Width5 0x03
#define Addr_Width4 0x02
#define Addr_Width3 0x01
#define WRX_PAYLOAD_WIDTH 32//定义通道的有效数据宽度（最大32Byte）
#define TX_ADDRESS_WIDTH 5
#define RX_ADDRESS_WIDTH 5//收发节点地址的宽度（最大5Byte）
#define STATUS_MAX_RT 0x10 //状态寄存器达到最大重发次数标志 
#define STATUS_RX_DR 0x40 //状态寄存器接收到有效数据标志
#define STATUS_TX_DS 0x20 //状态寄存器发送完成标志	


					
/***************函数申明***************/

uint8_t NRF24L01P_Init(uint8_t TxMode);
void NRF24L01_RX_ModeConfig(void);
void NRF24L01_TX_ModeConfig(void);
void NRF24L01_TXRX_Mode_Switch(uint8_t* TxAddr, uint8_t SwFlag);
void NRF_W_Reg(uint8_t Reg, uint8_t WData);
uint8_t NRF_R_Reg(uint8_t Reg);
uint8_t NRF_W_nByte(uint8_t Reg, uint8_t* WDataBuf, uint8_t n);
uint8_t NRF_R_nByte(uint8_t Reg, uint8_t* RDataBuf, uint8_t n);

#if NRF24L01_IRQ_EXTI
void NRF24L01_IRQ_Handler(void);
#endif //NRF24L01_IRQ_EXTI

uint8_t NRF24L01_TxPacket(uint8_t* TxBuf);
uint8_t NRF24L01_RxPacket(uint8_t* RxBuf);
uint8_t NRF24L01_FastRxPacket(uint8_t* RxBuf);
uint8_t NRF_TIMEOUT_UserCallback(uint32_t errorCode);
uint8_t NRF24L01_Check(void);

#if TEST_NRF24L01
uint8_t NRF_TXRX_TEST(uint8_t TxMode);
void NRF24L01_TX_TestConfig(void);
void NRF24L01_RX_TestConfig(void);
#endif //TEST_NRF24L01
uint8_t Compare_Array(uint8_t* arr1, uint8_t* arr2, uint8_t len);
/********************************/


#endif //_NRF24L01P_H



