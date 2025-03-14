
#include "bsp_spi.h"





/**
  * @brief   初始化SPI
  * @param   
  *		@arg SPIx：需要配置的SPI：SPI1/2/3
  *		@arg SPI_Mode： SPI模式：SPI_Mode_Master/SPI_Mode_Slave 
  *		@arg SPI_Direction：数据传输的方向： SPI_Direction_2Lines_FullDuplex/SPI_Direction_2Lines_RxOnly/SPI_Direction_1Line_Rx/SPI_Direction_1Line_Tx
  *		@arg SPI_NSS：NSS控制方式 : SPI_NSS_Soft / SPI_NSS_Hard  
  *		注：软件NSS与硬件NSS所使用引脚默认相同
  * @retval  
**/

void BSP_SPIx_Init(SPI_TypeDef* SPIx , uint16_t SPI_Mode, uint16_t SPI_Direction, uint16_t SPI_NSS )
{
	
	void (*SPIx_RCC_APBxPeriphClockCmd)(uint32_t RCC_APB2Periph, FunctionalState NewState);
	
	uint32_t RCC_APBxPeriph_CLK;//SPI外设时钟
	
	void (*SPIx_GPIO_RCC_APBxPeriphClockCmd)(uint32_t RCC_APB2Periph, FunctionalState NewState) ;
	
	GPIO_TypeDef* SPI_NSS_PORTx;
	uint16_t SPI_NSS_PINx;
	uint32_t SPI_NSS_RCC_APBxPeriph_CLK;
	
	
	GPIO_TypeDef* SPI_CK_PORTx;
	uint16_t SPI_CK_PINx;
	uint32_t SPI_CK_RCC_APBxPeriph_CLK;
	
	GPIO_TypeDef* SPI_MOSI_PORTx;
	uint16_t SPI_MOSI_PINx;
	uint32_t SPI_MOSI_RCC_APBxPeriph_CLK;
	
	GPIO_TypeDef* SPI_MISO_PORTx;
	uint16_t SPI_MISO_PINx;
	uint32_t SPI_MISO_RCC_APBxPeriph_CLK;
	

	
	if(SPIx == SPI1)
	{
		SPIx_RCC_APBxPeriphClockCmd = RCC_APB2PeriphClockCmd;
		RCC_APBxPeriph_CLK = RCC_APB2Periph_SPI1;
		
		SPI_NSS_PORTx = GPIOA;
		SPI_NSS_PINx = GPIO_Pin_4;
		SPI_NSS_RCC_APBxPeriph_CLK = RCC_APB2Periph_GPIOA;
		
		SPI_CK_PORTx = GPIOA;
		SPI_CK_PINx = GPIO_Pin_5;
		SPI_CK_RCC_APBxPeriph_CLK = RCC_APB2Periph_GPIOA;
		
		SPI_MOSI_PORTx = GPIOA;
		SPI_MOSI_PINx = GPIO_Pin_7;
		SPI_MOSI_RCC_APBxPeriph_CLK = RCC_APB2Periph_GPIOA;
		
		SPI_MISO_PORTx = GPIOA;
		SPI_MISO_PINx = GPIO_Pin_6;
		SPI_MISO_RCC_APBxPeriph_CLK = RCC_APB2Periph_GPIOA;
		
		SPIx_GPIO_RCC_APBxPeriphClockCmd = RCC_APB2PeriphClockCmd;
	}
	else if(SPIx == SPI2)
	{
		SPIx_RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		RCC_APBxPeriph_CLK = RCC_APB1Periph_SPI2;
		
		SPI_NSS_PORTx = GPIOB;
		SPI_NSS_PINx = GPIO_Pin_12;
		SPI_NSS_RCC_APBxPeriph_CLK = RCC_APB2Periph_GPIOB;
		
		SPI_CK_PORTx = GPIOB;
		SPI_CK_PINx = GPIO_Pin_13;
		SPI_CK_RCC_APBxPeriph_CLK = RCC_APB2Periph_GPIOB;
		
		SPI_MOSI_PORTx = GPIOB;
		SPI_MOSI_PINx = GPIO_Pin_15;
		SPI_MOSI_RCC_APBxPeriph_CLK = RCC_APB2Periph_GPIOB;
		
		SPI_MISO_PORTx = GPIOB;
		SPI_MISO_PINx = GPIO_Pin_14;
		SPI_MISO_RCC_APBxPeriph_CLK = RCC_APB2Periph_GPIOB;
		
		SPIx_GPIO_RCC_APBxPeriphClockCmd = RCC_APB2PeriphClockCmd;
	}
	else if(SPIx == SPI3)
	{
		SPIx_RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		RCC_APBxPeriph_CLK = RCC_APB1Periph_SPI3;
		
		SPI_NSS_PORTx = GPIOA;
		SPI_NSS_PINx = GPIO_Pin_15;
		SPI_NSS_RCC_APBxPeriph_CLK = RCC_APB2Periph_GPIOA;
		
		SPI_CK_PORTx = GPIOB;
		SPI_CK_PINx = GPIO_Pin_3;
		SPI_CK_RCC_APBxPeriph_CLK = RCC_APB2Periph_GPIOB;
		
		SPI_MOSI_PORTx = GPIOB;
		SPI_MOSI_PINx = GPIO_Pin_5;
		SPI_MOSI_RCC_APBxPeriph_CLK = RCC_APB2Periph_GPIOB;
		
		SPI_MISO_PORTx = GPIOB;
		SPI_MISO_PINx = GPIO_Pin_4;
		SPI_MISO_RCC_APBxPeriph_CLK = RCC_APB2Periph_GPIOB;
		
		SPIx_GPIO_RCC_APBxPeriphClockCmd = RCC_APB2PeriphClockCmd;
	}
	else
	{return;}
	
	
	/******************************GPIO配置***********************************/
	//模式配置见参考手册（通用复用功能I/O）
	GPIO_InitTypeDef GPIO_InitStruct_NSS;
	GPIO_InitTypeDef GPIO_InitStruct_CK;
	GPIO_InitTypeDef GPIO_InitStruct_MOSI;
	GPIO_InitTypeDef GPIO_InitStruct_MISO;
	
	/*NSS引脚*/
	SPIx_GPIO_RCC_APBxPeriphClockCmd(SPI_NSS_RCC_APBxPeriph_CLK|\
									 SPI_CK_RCC_APBxPeriph_CLK|\
								     SPI_MOSI_RCC_APBxPeriph_CLK|\
									 SPI_MISO_RCC_APBxPeriph_CLK,    ENABLE);//打开引脚端口时钟
	
	if(SPI_Mode == SPI_Mode_Master)
	{
		GPIO_InitStruct_CK.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct_MISO.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStruct_MOSI.GPIO_Mode = GPIO_Mode_AF_PP;
		
		if(SPI_NSS == SPI_NSS_Soft)
		{
			GPIO_InitStruct_NSS.GPIO_Mode = GPIO_Mode_Out_PP;//选择NSS为软件控制
		}
		else
		{
			GPIO_InitStruct_NSS.GPIO_Mode = GPIO_Mode_AF_PP;//选择NSS为硬件控制
		}
	}
	else
	{
		GPIO_InitStruct_CK.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStruct_MISO.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct_MOSI.GPIO_Mode = GPIO_Mode_AF_PP;
		
		if(SPI_NSS == SPI_NSS_Hard)
		{
			GPIO_InitStruct_NSS.GPIO_Mode = GPIO_Mode_IN_FLOATING;//NSS为硬件控制
		}
	
	}

	GPIO_InitStruct_NSS.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct_NSS.GPIO_Pin = SPI_NSS_PINx;
	GPIO_Init(SPI_NSS_PORTx, &GPIO_InitStruct_NSS);//将配置的NSS引脚参数写入寄存器
	
	GPIO_InitStruct_CK.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct_CK.GPIO_Pin = SPI_CK_PINx;
	GPIO_Init(SPI_CK_PORTx, &GPIO_InitStruct_CK);//将配置的NSS引脚参数写入寄存器
	
	GPIO_InitStruct_MISO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct_MISO.GPIO_Pin = SPI_MISO_PINx;
	GPIO_Init(SPI_MISO_PORTx, &GPIO_InitStruct_MISO);//将配置的MISO引脚参数写入寄存器
	
	GPIO_InitStruct_MOSI.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct_MOSI.GPIO_Pin = SPI_MOSI_PINx;
	GPIO_Init(SPI_MOSI_PORTx, &GPIO_InitStruct_MOSI);//将配置的MISO引脚参数写入寄存器
	

	
	/******************************SPI配置************************************/
	SPI_Cmd(SPIx, DISABLE);//关闭SPI
	SPIx_RCC_APBxPeriphClockCmd(RCC_APBxPeriph_CLK, ENABLE);//打开SPI时钟
	SPI_InitTypeDef SPI_InitStruct;

	SPI_InitStruct.SPI_Direction = SPI_Direction ;//定义SPI数据收发方向| 
	SPI_InitStruct.SPI_Mode = SPI_Mode;//
	SPI_InitStruct.SPI_NSS = SPI_NSS;//选择NSS控制方式

	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//选择单次收发数据大小
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;//选择时钟极性(空闲时钟电平)
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;//选择时钟相位(数据奇边沿采样还是偶边沿采样)
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;//设置波特率分频因子
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//设置先发低字节还是高字节
	SPI_InitStruct.SPI_CRCPolynomial = 0;//指定CRC校验的多项式，本实验不校验数据，多项式随便写
	
	SPI_Init(SPIx, &SPI_InitStruct);//将配置的SPI参数写入寄存器
	

	if(SPI_Direction == SPI_Direction_1Line_Rx)
	{
		SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Rx);
	}
	else if(SPI_Direction == SPI_Direction_1Line_Tx)
	{
		SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Tx);
	}
	
	if(SPI_Mode == SPI_Mode_Master)
	{
		if(SPIx == SPI1)
			SPI1_NSS_HIGH();//取消选择从机
		else if(SPIx == SPI2)
			SPI2_NSS_HIGH();//取消选择从机
		
	}
	
	SPI_Cmd(SPIx, DISABLE);
	//SPI_Cmd(SPI_NUM, ENABLE);
	
}


//void BSP_SPI_Init(void)
//{
//	SPI_InitTypeDef SPI_InitStruct;
//	GPIO_InitTypeDef GPIO_InitStruct;
//	
//	
//	/******************************GPIO配置***********************************/
//	//模式配置见参考手册（通用复用功能I/O）
//	/*NSS引脚*/
//	SPI_NSS_CLKCMD_FUNC(SPI_NSS_CLK,ENABLE);//打开NSS引脚端口时钟
//	
//	#if Master //主机
//		#if NSS_Soft //选择NSS控制方式
//			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//选择NSS为软件控制
//		#else
//			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//选择NSS为硬件控制
//		#endif /*NSS_Soft*/
//	#else //从机
//		#if NSS_Soft //选择NSS控制方式
//			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//选择NSS为软件控制（暂无）
//		#else
//			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//选择NSS为硬件控制
//		#endif /*NSS_Soft*/
//	#endif /*Master*/
//	
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStruct.GPIO_Pin = SPI_NSS_PIN;
//	GPIO_Init(SPI_NSS_PORT, &GPIO_InitStruct);//将配置的NSS引脚参数写入寄存器
//	
//	/*CK引脚*/
//	SPI_CK_CLKCMD_FUNC(SPI_CK_CLK,ENABLE);//打开CK引脚端口时钟
//	#if Master //主机
//		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
//	#else //从机
//		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	#endif /*Master*/
//	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStruct.GPIO_Pin = SPI_CK_PIN;
//	GPIO_Init(SPI_CK_PORT, &GPIO_InitStruct);//将配置的NSS引脚参数写入寄存器
//	
//	/*MISO引脚*/
//	SPI_MISO_CLKCMD_FUNC(SPI_MISO_CLK,ENABLE);//打开MISO引脚端口时钟
//	#if Master //主机
//		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	#else //从机
//		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
//	#endif /*Master*/
//	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStruct.GPIO_Pin = SPI_MISO_PIN;
//	GPIO_Init(SPI_MISO_PORT, &GPIO_InitStruct);//将配置的MISO引脚参数写入寄存器
//	
//	/*MOSI引脚*/
//	SPI_MOSI_CLKCMD_FUNC(SPI_MOSI_CLK,ENABLE);//打开MOSI引脚端口时钟
//	#if Master //主机
//		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
//	#else //从机
//		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	#endif /*Master*/
//	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStruct.GPIO_Pin = SPI_MOSI_PIN;
//	GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStruct);//将配置的MISO引脚参数写入寄存器
//	
//	
//	/******************************SPI配置************************************/
//	SPI_CLKCMD_FUNC(SPI_CLK_PORT,ENABLE);//打开SPI外设时钟
//	
//	#if SINGLE_LINE //单线模式
//		SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx ;//定义SPI数据收发方向| SPI_Direction_1Line_Rx
//	#else
//		SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//定义SPI数据收发方向
//	#endif /*SINGLE_LINE*/
//	
//	#if Master //主机模式
//		SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//选择SPI主
//	#else
//		SPI_InitStruct.SPI_Mode = SPI_Mode_Slave;//选择SPI从
//	#endif /*Master*/
//	
//	#if NSS_Soft //选择NSS控制方式
//		SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//选择NSS为软件控制
//	#else
//		SPI_InitStruct.SPI_NSS = SPI_NSS_Hard;//选择NSS为硬件控制
//	#endif /*NSS_Soft*/
//	
//	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//选择单次收发数据大小
//	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;//选择时钟极性(空闲时钟电平)
//	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;//选择时钟相位(数据奇边沿采样还是偶边沿采样)
//	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//设置波特率分频因子
//	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//设置先发低字节还是高字节
//	SPI_InitStruct.SPI_CRCPolynomial = 0;//指定CRC校验的多项式，本实验不校验数据，多项式随便写
//	
//	SPI_Init(SPI_NUM, &SPI_InitStruct);//将配置的SPI参数写入寄存器
//	

//	
//	#if SINGLE_LINE //单线模式
//		#if SINGLE_LINE_ReadOnly //单线只读
//			SPI_BiDirectionalLineConfig(SPI_NUM, SPI_Direction_Rx);
//		#else //单线只写
//			SPI_BiDirectionalLineConfig(SPI_NUM, SPI_Direction_Tx);
//		#endif /*SINGLE_LINE_ReadOnly*/
//	#endif /*SINGLE_LINE*/
//	
//	#if Master //主机模式
//	SPI_NSS_HIGH();//取消选择从机
//	#endif /*Master*/

//	SPI_Cmd(SPI_NUM, DISABLE);
//	//SPI_Cmd(SPI_NUM, ENABLE);
//	
//	/******************配置中断******************/
////	SPI_NVIC_Config();//中断配置
////	SPI_I2S_ITConfig(SPI_NUM, SPI_I2S_IT_RXNE, DISABLE);//关闭接收中断
////	SPI_I2S_ITConfig(SPI_NUM, SPI_I2S_IT_TXE, DISABLE);//关闭发送中断
//}	




//static void SPI_NVIC_Config(void)
//{
//	NVIC_InitTypeDef NVIC_InitStruct;
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//选择中断优先级分组
//	
//	NVIC_InitStruct.NVIC_IRQChannel = SPI1_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	
//	NVIC_Init(&NVIC_InitStruct);
//}



/**
* @brief   SPI1 DMA 发送
  * @param   
  *		@arg SendBuff ：需要发送的数据地址
  *		@arg len ：需要传输多少字节数据
  *		@arg MemInc ：数据地址需不需要自动增加： 1/0  需要/不需要
  * @retval  
**/
void SPI1_DMA_SendBytes(uint8_t *SendBuff, uint32_t len, _Bool MemInc)
{
	
	DMA1_Channel3->CPAR = (uint32_t)(SPI1_BASE+0x0c); //外设地址
    DMA1_Channel3->CMAR = (uint32_t) SendBuff; //mem地址
    DMA1_Channel3->CNDTR = len ; //传输长度
    DMA1_Channel3->CCR = (0 << 14) | // 非存储器到存储器模式
            (2 << 12) | // 通道优先级高
            (0 << 11) | // 存储器数据宽度8bit
            (0 << 10) | // 存储器数据宽度8bit
            (0 <<  9) | // 外设数据宽度8bit
            (0 <<  8) | // 外设数据宽度8bit
            ((uint16_t)MemInc <<  7) | // 存储器地址增量模式
            (0 <<  6) | // 外设地址增量模式(不增)
            (0 <<  5) | // 非循环模式
            (1 <<  4) | // 从存储器读
            (0 <<  3) | // 不允许传输错误中断
            (0 <<  2) | // 不允许半传输中断
            (1 <<  1) | // 允许传输完成中断
            (1);        // 通道开启
	
	SPI1->CR2 |= SPI_I2S_DMAReq_Tx;//开启SPI1的TX  DMA ，当TXE置位时SPI会立即发出DMA请求

}





#if SPI1_SINGLE_LINE /****************单线模式*****************/

	#if SPI1_Master /*************(单线)主机模式**************/
		/*
		发送数据
		*/	
		void SPI1_WriteByte(uint8_t Byte)
		{
			SPI1_NSS_LOW();//选择从机
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)!=SET);
			SPI_I2S_SendData(SPI1, Byte);
			while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)!=RESET);//等待SPI线空闲
			SPI1_NSS_HIGH();//取消选择从机
		}

		/*
		接收数据
		*/
		void SPI1_ReadByte(uint8_t Byte,uint8_t* RByte)
		{
			uint8_t i;
			//SPI_Cmd(SPI_NUM, DISABLE);
			
			SPI1_NSS_LOW();//选择从机
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)!=SET);
			SPI_I2S_SendData(SPI1, Byte);
			//SPI_Cmd(SPI_NUM, ENABLE);
			while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)!=RESET);//等待SPI线空闲
			SPI_BiDirectionalLineConfig(SPI1, SPI_Direction_Rx);//切换为只读
			for(i=0; i<4; i++)
			{
				while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)!=SET);
				*(RByte+i) = SPI_I2S_ReceiveData(SPI1);
			}
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY)!=RESET);//等待传输结束
			SPI1_NSS_HIGH();//取消选择从机
			SPI_BiDirectionalLineConfig(SPI1, SPI_Direction_Tx);//切换为只写
			
		}
	#else /*********(单线)从机模式*************/
		/*
		发送数据
		*/	
		void SPI1_WriteByte(uint8_t Byte)
		{
			SPI_BiDirectionalLineConfig(SPI1, SPI_Direction_Tx);//切换为只写
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)!=SET);
			SPI_I2S_SendData(SPI1, Byte);
			SPI_BiDirectionalLineConfig(SPI1, SPI_Direction_Rx);//切换为只读
		}

		/*
		接收数据
		*/
		void SPI1_ReadByte(uint8_t Byte,uint8_t* RByte)
		{
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)!=SET);
			*RByte = SPI_I2S_ReceiveData(SPI1);
		}
	#endif /*Master*/




#else /**************双线模式*************/

	#if SPI1_Master /*************(双线)主机模式**************/
	/*
	发送数据
	*WByte:发出的数据
	*RByte:发出时接收到的数据
	*/
	void SPI1_WriteByte(uint8_t* WByte,uint8_t* RByte)
	{
		SPI1_NSS_LOW();//选择从机
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)!=SET);
		SPI_I2S_SendData(SPI1, *WByte);
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)!=SET);
		SPI_I2S_ReceiveData(SPI1);//同时读取数据
		SPI1_NSS_HIGH();//取消选择从机
	}

	/*
	接收数据
	WByte:接收时发出的数据的地址
	RByte:接收到的数据的地址
	*/
	uint8_t SPI1_ReadByte(uint8_t* WByte,uint8_t* RByte)
	{
		uint8_t i=0;
		SPI1_NSS_LOW();//选择从机
		/*读写数组*/
		for(;i<4;i++)
		{
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)!=SET);
			SPI_I2S_SendData(SPI1, *(WByte+i));
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)!=SET);
			*(RByte+i) = SPI_I2S_ReceiveData(SPI1);//同时读取数据
		}
		/*读写单字节数据*/
	//		while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE)!=SET);
	//		SPI_I2S_SendData(SPI_NUM, *(WByte));
	//		while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE)!=SET);
	//		*(RByte) = SPI_I2S_ReceiveData(SPI_NUM);//同时读取数据
			
		SPI1_NSS_HIGH();//取消选择从机
		return *RByte;//返回读取的数据（数组的第一个数据）
	}
	
	#else /*********(双线)从机模式*************/
	/*
	发送数据
	*WByte:发出的数据
	*RByte:发出时接收到的数据
	*/
	void SPI1_WriteByte(uint8_t* WByte,uint8_t* RByte)
	{
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)!=SET);
		SPI_I2S_SendData(SPI1, *WByte);
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)!=SET);
		SPI_I2S_ReceiveData(SPI1);//同时读取数据
	}

	/*
	接收数据
	WByte:接收时发出的数据的地址
	RByte:接收到的数据的地址
	*/
	uint8_t SPI1_ReadByte(uint8_t* WByte,uint8_t* RByte)
	{
		uint8_t i=0;
		/*读写数组*/
		for(;i<4;i++)
		{
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)!=SET);
			SPI_I2S_SendData(SPI1, *(WByte+i));
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)!=SET);
			*(RByte+i) = SPI_I2S_ReceiveData(SPI1);//同时读取数据
		}
		/*读写单字节数据*/
	//		while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE)!=SET);
	//		SPI_I2S_SendData(SPI_NUM, *(WByte));
	//		while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE)!=SET);
	//		*(RByte) = SPI_I2S_ReceiveData(SPI_NUM);//同时读取数据
		
		return *RByte;//返回读取的数据（数组的第一个数据）
	}

	#endif /*Master*/
	
#endif /*SPI1_SINGLE_LINE*/






