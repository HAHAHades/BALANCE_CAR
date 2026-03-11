#include "bsp_usart.h"//板级支持包board support package\
                      仅支持某一个或一批同样的开发板


#ifdef BSP_USE_F103


/**
  * @brief   测试串口
  * @param   
  * @retval 
**/
void BSP_USART_Test(void)
{

	BSP_USARTx_Init(USART1, BSP_USARTxStructInit(115200) , USART1_IO_Reamp0);
	
	UsartPrintf(USART1, "USART TEST\n");

}


/**
  * @brief   按默认方式配置串口结构体
  * @param   BaudRate ：串口波特率
  * @retval 
**/
USART_InitTypeDef BSP_USARTxStructInit(uint32_t BaudRate)
{

	USART_InitTypeDef USART_InitStruct;

	USART_InitStruct.USART_BaudRate = BaudRate;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//模式配置为收发一起
	
	return USART_InitStruct;
} 


/**
  * @brief   初始化串口
  * @param   USARTx ：需要配置的USART：USART1/2/3
  * @param   USART_InitStruct ：串口的配置 
  * @param   IO_Reamp ：IO重映射 @ref USART_IO_Reamp
  * @retval 
**/
void BSP_USARTx_Init(USART_TypeDef *USARTx, USART_InitTypeDef USART_InitStruct, uint32_t IO_Reamp)
{
	void (*RCC_APBxPeriphClockCmd)(uint32_t RCC_APBxPeriph, FunctionalState NewState);

	uint32_t RCC_APBxPeriph_USARTx;

	GPIO_TypeDef* USART_TX_PORTx;
	uint16_t USART_TX_PINx;
	// uint8_t USART_TX_PinSourcex;
	
	GPIO_TypeDef* USART_RX_PORTx;
	uint16_t USART_RX_PINx;
	// uint8_t USART_RX_PinSourcex;

	uint8_t USART_IRQ;

	if (USARTx == USART1)
	{
		RCC_APBxPeriphClockCmd = RCC_APB2PeriphClockCmd;
		RCC_APBxPeriph_USARTx = RCC_APB2Periph_USART1;
	
		USART_IRQ = USART1_IRQn;

		USART_TX_PORTx = GPIOA;
		USART_TX_PINx = GPIO_Pin_9;
		// USART_TX_PinSourcex = GPIO_PinSource9;
		
		USART_RX_PORTx = GPIOA;
		USART_RX_PINx = GPIO_Pin_10;
		// USART_RX_PinSourcex = GPIO_PinSource10;

		if (IO_Reamp==USART1_IO_Reamp1)
		{
			USART_TX_PORTx = GPIOB;
			USART_TX_PINx = GPIO_Pin_6;
			// USART_TX_PinSourcex = GPIO_PinSource6;
			
			USART_RX_PORTx = GPIOB;
			USART_RX_PINx = GPIO_Pin_7;
			// USART_RX_PinSourcex = GPIO_PinSource7;

			RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,  ENABLE);
			GPIO_PinRemapConfig( GPIO_Remap_USART1,  ENABLE);

		}
	}
	else if (USARTx == USART2)
	{
		RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		RCC_APBxPeriph_USARTx = RCC_APB1Periph_USART2;
		
		USART_IRQ = USART2_IRQn;

		USART_TX_PORTx = GPIOA;
		USART_TX_PINx = GPIO_Pin_2;
		// USART_TX_PinSourcex = GPIO_PinSource2;
		
		USART_RX_PORTx = GPIOA;
		USART_RX_PINx = GPIO_Pin_3;
		// USART_RX_PinSourcex = GPIO_PinSource3;

		if (IO_Reamp==USART2_IO_Reamp1)
		{
			USART_TX_PORTx = GPIOD;
			USART_TX_PINx = GPIO_Pin_5;
			// USART_TX_PinSourcex = GPIO_PinSource5;
			
			USART_RX_PORTx = GPIOD;
			USART_RX_PINx = GPIO_Pin_6;
			// USART_RX_PinSourcex = GPIO_PinSource6;
		}
	}
	else //(USARTx == USART3)
	{
		RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		RCC_APBxPeriph_USARTx = RCC_APB1Periph_USART3;
	
		USART_IRQ = USART3_IRQn;

		USART_TX_PORTx = GPIOB;
		USART_TX_PINx = GPIO_Pin_10;
		// USART_TX_PinSourcex = GPIO_PinSource10;
		
		USART_RX_PORTx = GPIOB;
		USART_RX_PINx = GPIO_Pin_11;
		// USART_RX_PinSourcex = GPIO_PinSource11;

	}

/******************GPIO配置****************/

	BSP_GPIO_Config(USART_TX_PORTx, USART_TX_PINx,  GPIO_Mode_AF_PP);
	BSP_GPIO_Config(USART_RX_PORTx, USART_RX_PINx,  GPIO_Mode_IN_FLOATING);


/******************USART配置****************/
	RCC_APBxPeriphClockCmd(RCC_APBxPeriph_USARTx, ENABLE);
	USART_Init(USARTx, &USART_InitStruct);

	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStruct);

	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);//打开串口的接收寄存器为非空中断
	
	USART_Cmd(USARTx, ENABLE);//打开串口

}


#endif //BSP_USE_F103


#ifdef BSP_USE_F401

/**
  * @brief   初始化串口
  * @param   USARTx ：需要配置的USART：USART1/2/6
  * @param   USART_InitStruct ：串口的配置 
  * @param   IO_Reamp ：IO重映射 @ref USART_IO_Reamp
  * @retval 
**/
void BSP_USARTx_Init(USART_TypeDef *USARTx, USART_InitTypeDef USART_InitStruct, uint32_t IO_Reamp)
{

	void (*RCC_APBxPeriphClockCmd)(uint32_t RCC_APBxPeriph, FunctionalState NewState);

	uint32_t RCC_APBxPeriph_USARTx;

	uint8_t USART_IRQ;

	uint8_t USARTx_GPIO_AF;

	GPIO_TypeDef* USART_TX_PORTx;
	uint16_t USART_TX_PINx;
	uint8_t USART_TX_PinSourcex;
	
	GPIO_TypeDef* USART_RX_PORTx;
	uint16_t USART_RX_PINx;
	uint8_t USART_RX_PinSourcex;



	if (USARTx == USART1)
	{
		RCC_APBxPeriphClockCmd = RCC_APB2PeriphClockCmd;
		RCC_APBxPeriph_USARTx = RCC_APB2Periph_USART1;
		USARTx_GPIO_AF = GPIO_AF_USART1;
		USART_IRQ = USART1_IRQn;

		USART_TX_PORTx = GPIOA;
		USART_TX_PINx = GPIO_Pin_9;
		USART_TX_PinSourcex = GPIO_PinSource9;
		
		USART_RX_PORTx = GPIOA;
		USART_RX_PINx = GPIO_Pin_10;
		USART_RX_PinSourcex = GPIO_PinSource10;

		if (IO_Reamp==USART1_IO_Reamp1)
		{
			USART_TX_PORTx = GPIOB;
			USART_TX_PINx = GPIO_Pin_6;
			USART_TX_PinSourcex = GPIO_PinSource6;
			
			USART_RX_PORTx = GPIOB;
			USART_RX_PINx = GPIO_Pin_7;
			USART_RX_PinSourcex = GPIO_PinSource7;
		}
		

	}
	else if (USARTx == USART2)
	{
		RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		RCC_APBxPeriph_USARTx = RCC_APB1Periph_USART2;
		USARTx_GPIO_AF = GPIO_AF_USART2;
		USART_IRQ = USART2_IRQn;

		USART_TX_PORTx = GPIOA;
		USART_TX_PINx = GPIO_Pin_2;
		USART_TX_PinSourcex = GPIO_PinSource2;
		
		USART_RX_PORTx = GPIOA;
		USART_RX_PINx = GPIO_Pin_3;
		USART_RX_PinSourcex = GPIO_PinSource3;

		if (IO_Reamp==USART2_IO_Reamp1)
		{
			USART_TX_PORTx = GPIOD;
			USART_TX_PINx = GPIO_Pin_5;
			USART_TX_PinSourcex = GPIO_PinSource5;
			
			USART_RX_PORTx = GPIOD;
			USART_RX_PINx = GPIO_Pin_6;
			USART_RX_PinSourcex = GPIO_PinSource6;
		}

	}
	else //(USARTx == USART6)
	{
		RCC_APBxPeriphClockCmd = RCC_APB2PeriphClockCmd;
		RCC_APBxPeriph_USARTx = RCC_APB2Periph_USART6;
		USARTx_GPIO_AF = GPIO_AF_USART6;
		USART_IRQ = USART6_IRQn;

		USART_TX_PORTx = GPIOC;
		USART_TX_PINx = GPIO_Pin_6;
		USART_TX_PinSourcex = GPIO_PinSource6;
		
		USART_RX_PORTx = GPIOC;
		USART_RX_PINx = GPIO_Pin_7;
		USART_RX_PinSourcex = GPIO_PinSource7;

		if (IO_Reamp==USART6_IO_Reamp1)
		{
			USART_TX_PORTx = GPIOA;
			USART_TX_PINx = GPIO_Pin_11;
			USART_TX_PinSourcex = GPIO_PinSource11;
			
			USART_RX_PORTx = GPIOA;
			USART_RX_PINx = GPIO_Pin_12;
			USART_RX_PinSourcex = GPIO_PinSource12;
		}
	}
	



/******************GPIO配置****************/

	BSP_GPIO_Config(USART_TX_PORTx, USART_TX_PINx,  GPIO_Mode_AF, 
					GPIO_OType_PP,  GPIO_PuPd_NOPULL,  GPIO_Speed_50MHz);
	GPIO_PinAFConfig(USART_TX_PORTx, USART_TX_PinSourcex,  USARTx_GPIO_AF);
	
	BSP_GPIO_Config(USART_RX_PORTx, USART_RX_PINx,  GPIO_Mode_AF, 
					GPIO_OType_PP,  GPIO_PuPd_NOPULL,  GPIO_Speed_50MHz);
	GPIO_PinAFConfig(USART_RX_PORTx, USART_RX_PinSourcex,  USARTx_GPIO_AF);


/******************USART配置****************/

	RCC_APBxPeriphClockCmd(RCC_APBxPeriph_USARTx, ENABLE);
	USART_Init(USARTx, &USART_InitStruct);


	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_ITConfig(USARTx, USART_IT_RXNE,  ENABLE);	
	USART_Cmd(USARTx, ENABLE);//打开串口

}


#endif //BSP_USE_F401


#include <stdarg.h>
#include <string.h>
#include <stdio.h>
/**
  * @brief   串口发送字符串
  * @param   USARTx ：需要使用的USART：USART1/2/6
  * @param   fmt ：格式化字符串
  * @retval 
**/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
	unsigned char UsartPrintfBuf[USART_PRINTF_LEN];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;

	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
	va_end(ap);

	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	}
}

/**
  * @brief   串口接收数据
  * @param   USARTx ：需要使用的USART：USART1/2/6
  * @param   Rdata ：接收的数据
  * @retval  0：接收为空；else：接收的数据长度
**/
uint16_t UsartRecv(USART_TypeDef *USARTx, uint8_t* Rdata)
{
	uint16_t ret_val=0;

	#if USE_USART1
	if (USARTx==USART1)
	{
		if (USART_RX_STA1|USART_RX_STA_C)
		{
			ret_val = USART_RX_STA1&USART_RX_STA_HCMask;
			memcpy(Rdata, USART_RX_BUF1, ret_val);
			USART_RX_STA1=0;
		}
	}
	#endif //USE_USART1

	#if USE_USART2
	if (USARTx==USART2)
	{
		if (USART_RX_STA2|USART_RX_STA_C)
		{
			ret_val = USART_RX_STA2|USART_RX_STA_HCMask;
			memcpy(Rdata, USART_RX_BUF2, ret_val);
			USART_RX_STA2=0;
		}
	}
	#endif //USE_USART2

	#if USE_USART6
	if (USARTx==USART6)
	{
		if (USART_RX_STA6|USART_RX_STA_C)
		{
			ret_val = USART_RX_STA6|USART_RX_STA_HCMask;
			memcpy(Rdata, USART_RX_BUF6, ret_val);
			USART_RX_STA6=0;
		}
	}
	#endif //USE_USART6

	return ret_val;
}


/**
  * @brief   串口接收数据
  * @param   USARTx ：需要使用的USART：USART1/2/6
  * @param   Sdata ：发送的数据
  * @param   Slen ：发送的数据长度
  * @retval  
**/
void UsartSend(USART_TypeDef *USARTx, uint8_t* Sdata, uint16_t Slen)
{

	for(uint16_t i=0; i<Slen; i++)
	{
		USART_SendData(USARTx, *Sdata++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	}

}

/*
	中断服务函数
*/
#if USE_USART1
u8 USART_RX_BUF1[USART_REC_LEN1];
u16 USART_RX_STA1 = 0; // 定义串口接受状态 
					  // 从bit0-bit15共16位 bit15:接受完成标志 
					  // 14:接受到'\r'(0X0D)标志 
					  // 0-13bit表示接受的有效数据长度
void vUSART1_IRQHandler(void)
{
	u8 Tem=0;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{		
		Tem = USART_ReceiveData(USART1);
		if(USART_RX_STA1 & 0x8000)//接受完成
			return;
		
		if(USART_RX_STA1 & 0x4000)//接受到'\r'（结束标志0x0d）
		{
			if(Tem==0x0a)
			{
				USART_RX_STA1 |= 0x8000; //'\r'后是'\n'，表示接收完成
				uint8_t Rdata[20];
				uint16_t recvN=UsartRecv(DEBUG_USART, Rdata);
				if (recvN)
				{
					UsartSend(DEBUG_USART, Rdata,  recvN);
					USART_RX_STA1 = 0;
				}
			}
			else
			{
				USART_RX_STA1 &= (~USART_RX_STA_H);
				USART_RX_BUF1[USART_RX_STA1 & USART_RX_STA_HCMask] = Tem;//USART_RX_STA的前两位作为标志位不进行计数
				USART_RX_STA1++;
				if(USART_RX_STA1 > (USART_REC_LEN1-1))USART_RX_STA1=0;//超出接收缓存容量，认为出错
			}
		}
		else
		{
			if(Tem==0x0d)USART_RX_STA1 |= 0x4000;//接收的数据为'\r'结束标志，将USART_RX_STA状态更新为 接受到'\r'
			else //接受的数据不是'\r'，则将数据保存
			{
				USART_RX_BUF1[USART_RX_STA1 & USART_RX_STA_HCMask] = Tem;//USART_RX_STA的前两位作为标志位不进行计数
				USART_RX_STA1++;
				if(USART_RX_STA1 > (USART_REC_LEN1-1))USART_RX_STA1=0;//超出接收缓存容量，认为出错
			}
		} 
	}
}

#endif //#if USE_USART1


#if USE_USART2
u8 USART_RX_BUF2[USART_REC_LEN2];
u16 USART_RX_STA2 = 0; // 定义串口接受状态 
					  // 从bit0-bit15共16位 bit15:接受完成标志 
					  // 14:接受到'\r'(0X0D)标志 
					  // 0-13bit表示接受的有效数据长度
__attribute__((weak)) void vUSART2_IRQHandler(void)
{
	u8 Tem=0;
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
	{		
		Tem = USART_ReceiveData(USART2);
		if(USART_RX_STA2 & USART_RX_STA_C)//接受完成
			return;
		if(USART_RX_STA2 & USART_RX_STA_H)//接受到'\r'（结束标志0x0d）
		{
			if(Tem==0x0a)USART_RX_STA2 |= USART_RX_STA_C; //'\r'后是'\n'，表示接收完成
			else
			{
				USART_RX_STA2 &= (~USART_RX_STA_H);
				USART_RX_BUF2[USART_RX_STA2 & USART_RX_STA_HCMask] = Tem;//USART_RX_STA的前两位作为标志位不进行计数
				USART_RX_STA2++;
				if(USART_RX_STA2 > (USART_REC_LEN2-1))USART_RX_STA2=0;//超出接收缓存容量，认为出错
			}
		}
		else
		{
			if(Tem==0x0d)USART_RX_STA2 |= USART_RX_STA_H;//接收的数据为'\r'结束标志，将USART_RX_STA状态更新为 接受到'\r'
			else //接受的数据不是'\r'，则将数据保存
			{
				USART_RX_BUF2[USART_RX_STA2 & USART_RX_STA_HCMask] = Tem;//USART_RX_STA的前两位作为标志位不进行计数
				USART_RX_STA2++;
				if(USART_RX_STA2 > (USART_REC_LEN2-1))USART_RX_STA2=0;//超出接收缓存容量，认为出错
			}
		}
	}	
}

#endif //#if USE_USART2


#if USE_USART6
u8 USART_RX_BUF6[USART_REC_LEN6];
u16 USART_RX_STA6 = 0; // 定义串口接受状态 
					  // 从bit0-bit15共16位 bit15:接受完成标志 
					  // 14:接受到'\r'(0X0D)标志 
					  // 0-13bit表示接受的有效数据长度
__attribute__((weak)) void vUSART6_IRQHandler(void)
{
	u8 Tem=0;
	if(USART_GetITStatus(USART6,USART_IT_RXNE)!=RESET)
	{		
		Tem = USART_ReceiveData(USART6);
		if(USART_RX_STA6 & 0x8000)//接受完成
			return;
		if(USART_RX_STA6 & 0x4000)//接受到'\r'（结束标志0x0d）
		{
			if(Tem==0x0a)USART_RX_STA6 |= 0x8000; //'\r'后是'\n'，表示接收完成
			else
			{
				USART_RX_STA6 &= (~USART_RX_STA_H);
				USART_RX_BUF6[USART_RX_STA6 & USART_RX_STA_HCMask] = Tem;//USART_RX_STA的前两位作为标志位不进行计数
				USART_RX_STA6++;
				if(USART_RX_STA6 > (USART_REC_LEN6-1))USART_RX_STA6=0;//超出接收缓存容量，认为出错
			}
		}
		else
		{
			if(Tem==0x0d)USART_RX_STA6 |= 0x4000;//接收的数据为'\r'结束标志，将USART_RX_STA状态更新为 接受到'\r'
			else //接受的数据不是'\r'，则将数据保存
			{
				USART_RX_BUF6[USART_RX_STA6 & USART_RX_STA_HCMask] = Tem;//USART_RX_STA的前两位作为标志位不进行计数
				USART_RX_STA6++;
				if(USART_RX_STA6 > (USART_REC_LEN6-1))USART_RX_STA6=0;//超出接收缓存容量，认为出错
			}
		}
	}	
}

#endif //#if USE_USART6







