#include "bsp_usart.h"//板级支持包board support package\
                      仅支持某一个或一批同样的开发板
//#include "delay.h" 


/*printf 重定向  默认重定向到USART2  */
int fputc(int ch, FILE *p)
{
	
	# if printf_mapping==1
	USART_SendData(USART_NUM1, ch);
	while(USART_GetFlagStatus(USART_NUM1, USART_FLAG_TXE)==RESET);//等待发送完成
	return ch;
	#endif
	
	# if printf_mapping==2
	USART_SendData(USART_NUM2, ch);
	while(USART_GetFlagStatus(USART_NUM2, USART_FLAG_TXE)==RESET);//等待发送完成
	return ch;
	#endif
	
	
}




//u8 USART_RX_BUF[USART_REC_LEN];
//u16 USART_RX_STA = 0; // 定义串口接受状态 
//					  // 从bit0-bit15共16位 bit15:接受完成标志 
//					  // 14:接受到'\r'(0X0D)标志 
//					  // 0-13bit表示接受的有效数据长度
static void USART_NVIC_Config(int usart_num)
{
	if(usart_num==1)
	{
		NVIC_InitTypeDef NVIC_InitStruct;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//选择中断优先级分组
		
		NVIC_InitStruct.NVIC_IRQChannel = USART_IRQ1;
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		
		NVIC_Init(&NVIC_InitStruct);
	}
	else if(usart_num==2)
	{
		NVIC_InitTypeDef NVIC_InitStruct;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//选择中断优先级分组
		
		NVIC_InitStruct.NVIC_IRQChannel = USART_IRQ2;
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		
		NVIC_Init(&NVIC_InitStruct);
	}
}


void USART_Config(uint8_t usart_num, int BaudRate)
{
	
	if(usart_num==1)
	{

		#if USART_IO_Remap
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,  ENABLE);
		GPIO_PinRemapConfig( GPIO_Remap_USART1,  ENABLE);
		#endif 
		GPIO_InitTypeDef GPIO_InitStruct;
		USART_InitTypeDef USART_InitStruct;
		/******************GPIO配置****************/
		USART_TXD_CLKCMD_FUNC1(USART_TXD_CLK1,ENABLE);//因为都是同一个GPIO端口所以只开启一次时钟即可
		USART_CLKCMD_FUNC1(USART_CLK1,ENABLE);//打开使用的USART时钟
		/*TXD*/
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Pin = USART_TXD_PIN1;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(USART_TXD_PORT1, &GPIO_InitStruct);
		
		/*RXD*/
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStruct.GPIO_Pin = USART_RXD_PIN1;
		GPIO_Init(USART_RXD_PORT1, &GPIO_InitStruct);


		/******************USART配置****************/
		USART_InitStruct.USART_BaudRate = BaudRate;
		USART_InitStruct.USART_WordLength = USART_WordLength_8b;
		USART_InitStruct.USART_StopBits = USART_StopBits_1;
		USART_InitStruct.USART_Parity = USART_Parity_No;
		USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//模式配置为收发一起
		
		USART_Init(USART_NUM1, &USART_InitStruct);
		
		
		/*不使用同步通信，就不用配置时钟结构体*/
		# if USE_SCLK1
			USART_ClockInitTypeDef USART_ClockInitStruct;
			USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;
			USART_ClockInitStruct.USART_CPHA = USART_CPOL_High;
			USART_ClockInitStruct.USART_CPOL = USART_CPHA_2Edge;
			USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable;
			USART_ClockInit(USART_NUM1, &USART_ClockInitStruct);
		# endif /*USE_SCLK*/	
		USART_NVIC_Config(1);//配置串口中断
		
		USART_ITConfig(USART_NUM1, USART_IT_RXNE, ENABLE);//打开串口的接收寄存器为非空中断
		
		USART_Cmd(USART_NUM1, ENABLE);//打开串口
	
	}
	else if(usart_num==2)
	{
		GPIO_InitTypeDef GPIO_InitStruct;
		USART_InitTypeDef USART_InitStruct;
		/******************GPIO配置****************/
		USART_TXD_CLKCMD_FUNC2(USART_TXD_CLK2,ENABLE);//因为都是同一个GPIO端口所以只开启一次时钟即可
		USART_CLKCMD_FUNC2(USART_CLK2,ENABLE);//打开使用的USART时钟
		/*TXD*/
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Pin = USART_TXD_PIN2;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(USART_TXD_PORT2, &GPIO_InitStruct);
		
		/*RXD*/
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStruct.GPIO_Pin = USART_RXD_PIN2;
		GPIO_Init(USART_RXD_PORT2, &GPIO_InitStruct);
		
		
		/******************USART配置****************/
		USART_InitStruct.USART_BaudRate = BaudRate;
		USART_InitStruct.USART_WordLength = USART_WordLength_8b;
		USART_InitStruct.USART_StopBits = USART_StopBits_1;
		USART_InitStruct.USART_Parity = USART_Parity_No;
		USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//模式配置为收发一起
		
		USART_Init(USART_NUM2, &USART_InitStruct);
		
		
		/*不使用同步通信，就不用配置时钟结构体*/
		# if USE_SCLK2
			USART_ClockInitTypeDef USART_ClockInitStruct;
			USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;
			USART_ClockInitStruct.USART_CPHA = USART_CPOL_High;
			USART_ClockInitStruct.USART_CPOL = USART_CPHA_2Edge;
			USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable;
			USART_ClockInit(USART_NUM2, &USART_ClockInitStruct);
		# endif /*USE_SCLK*/	
		USART_NVIC_Config(2);//配置串口中断
		
		USART_ITConfig(USART_NUM2, USART_IT_RXNE, ENABLE);//打开串口的接收寄存器为非空中断
		
		USART_Cmd(USART_NUM2, ENABLE);//打开串口
	}
	
}


void Usart1_Init(int BaudRate)
{
	USART_Config(1,BaudRate);
}
void Usart2_Init(int BaudRate)
{
	USART_Config(2,BaudRate);
}



/*
发送8位数据到串口
*/
void USART_SendByte(USART_TypeDef* pUSARTx,uint8_t Byte)
{
	
	USART_ClearFlag(pUSARTx, USART_FLAG_TC);
	if(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) != RESET)
	{
		USART_SendData(pUSARTx, Byte);
	}
	
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) != SET);//等待发送完成
	
}


/*
发送数字到串口
*/
void USART_SendNum(USART_TypeDef* pUSARTx,uint16_t Num)
{
	uint16_t k = 0,i=0,m = 0;
	char pStr[4]="";
	static char str[4]="";
	do{
		k++;
		m = Num%10;
		str[k] = m+48;
		Num/=10;
		
	}while(Num);
	do{
		
		*(pStr+i) = str[k];
		k--;
		i++;
	}while(k);
	
	USART_SendString(pUSARTx,pStr);
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) != SET);//等待发送完成
}



/*
发送字符串
*/
void USART_SendString(USART_TypeDef* pUSARTx,char* pStr)
{
	do{
		USART_SendByte(pUSARTx,*pStr);
		pStr++;
	}while(*pStr != '\0');
	
}

/*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//发送数据
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
	}
//	while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
}




#include <stdarg.h>
#include <string.h>
#include <stdio.h>

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{

	unsigned char UsartPrintfBuf[296];
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





/*
	中断服务函数
*/
u8 USART_RX_BUF1[USART_REC_LEN1];
u16 USART_RX_STA1 = 0; // 定义串口接受状态 
					  // 从bit0-bit15共16位 bit15:接受完成标志 
					  // 14:接受到'\r'(0X0D)标志 
					  // 0-13bit表示接受的有效数据长度
void vUSART1_IRQHandler(void)
{
	u8 Tem=0;
	if(USART_GetITStatus(USART_NUM1,USART_IT_RXNE)!=RESET)
	{		
		Tem = USART_ReceiveData(USART_NUM1);
		if(USART_RX_STA1 & 0x8000)//接受完成
			return;
		
		if(USART_RX_STA1 & 0x4000)//接受到'\r'（结束标志0x0d）
		{
			if(Tem==0x0a)USART_RX_STA1 |= 0x8000; //'\r'后是'\n'，表示接收完成
			else USART_RX_STA1 = 0;//否则认为数据错误
		}
		else
		{
			if(Tem==0x0d)USART_RX_STA1 |= 0x4000;//接收的数据为'\r'结束标志，将USART_RX_STA状态更新为 接受到'\r'
			else //接受的数据不是'\r'，则将数据保存
			{
				USART_RX_BUF1[USART_RX_STA1 & 0x3fff] = Tem;//USART_RX_STA的前两位作为标志位不进行计数
				USART_RX_STA1++;
				if(USART_RX_STA1 > (USART_REC_LEN1-1))USART_RX_STA1=0;//超出接收缓存容量，认为出错
			}
		}
	    
	}	
}


u8 USART_RX_BUF2[USART_REC_LEN2];
u16 USART_RX_STA2 = 0; // 定义串口接受状态 
					  // 从bit0-bit15共16位 bit15:接受完成标志 
					  // 14:接受到'\r'(0X0D)标志 
					  // 0-13bit表示接受的有效数据长度
void vUSART2_IRQHandler(void)
{
	u8 Tem=0;
	if(USART_GetITStatus(USART_NUM2,USART_IT_RXNE)!=RESET)
	{		
		Tem = USART_ReceiveData(USART_NUM2);
		if(USART_RX_STA2 & 0x8000)//接受完成
			return;
		
		if(USART_RX_STA2 & 0x4000)//接受到'\r'（结束标志0x0d）
		{
			if(Tem==0x0a)USART_RX_STA2 |= 0x8000; //'\r'后是'\n'，表示接收完成
			else USART_RX_STA2 = 0;//否则认为数据错误
		}
		else
		{
			if(Tem==0x0d)USART_RX_STA2 |= 0x4000;//接收的数据为'\r'结束标志，将USART_RX_STA状态更新为 接受到'\r'
			else //接受的数据不是'\r'，则将数据保存
			{
				USART_RX_BUF2[USART_RX_STA2 & 0x3fff] = Tem;//USART_RX_STA的前两位作为标志位不进行计数
				USART_RX_STA2++;
				if(USART_RX_STA2 > (USART_REC_LEN2-1))USART_RX_STA2=0;//超出接收缓存容量，认为出错
			}
		}
	    
	}	
}


