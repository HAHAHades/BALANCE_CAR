
#include "bsp_spi.h"





/**
  * @brief   初始化SPI
  * @param   SPIx：需要配置的SPI：SPI1/2/3
  * @param   SPI_Mode： SPI模式 ： SPI_Mode_Master/SPI_Mode_Slave 
  * @param   SPI_Direction：数据传输的方向
  *		@arg SPI_Direction_2Lines_FullDuplex 双线全双工
  *		@arg SPI_Direction_2Lines_RxOnly     双线只读 通常用于多个从设备情况下，从设备接收时禁止发送可防止影响总线
  *		@arg SPI_Direction_1Line_Rx			 单线读取  
  *		@arg SPI_Direction_1Line_Tx			 单线发送
  *		注：单线模式下，主设备使用MOSI 从设备使用MISO
  * @param   SPI_NSS：NSS控制方式 : SPI_NSS_Soft(SSM=1) / SPI_NSS_Hard(SSM=0)  
  *		注：软件NSS与硬件NSS所使用引脚默认相同。推荐主机配置为软件NSS，从机配置为硬件NSS。
			选择软件NSS时，NSS引脚的电平由SPIx_CR1的SSI决定，在NSS引脚上的IO操作无效
			若把NSS脚配置为OUT_PP则可直接在NSS脚上进行IO操作，从设备会读取到NSS脚的状态。
			选择硬件NSS时，SPIx_CR2的SSOE决定设备的主从模式，SSOE为0，此设备为从设备，NSS脚被总线上的主设备控制；
			SSOE为1时，此设备为主设备，NSS被持续拉低直至SPI失能，若想SPI失能后NSS输出高电平则引脚需上拉。
		例：若设备工作在硬件主模式，即SSM=0,MSTR=1;SSOE=0且NSS脚接高电平设备才能正常工作，若与从设备通信则需另外的IO来片选从设备，共占用5个IO。
												SSOE=1，NSS脚持续输出低电平直至SPI失能，共占用4个IO。
			若设备工作在软件主模式，即SSM=1,MSTR=1;此时NSS脚可作为普通IO，SSI=1，设备才能正常工作，若与从设备通信则需另外的IO来片选从设备，共占用4个IO。（常用）
			若设备工作在硬件从模式，即SSM=0,MSTR=0;此时NSS脚从主设备的IO接收片选信号，被片选后可通信。（常用）
			若设备工作在软件从模式，即SSM=1,MSTR=0;此时NSS脚可作为普通IO，SSI=0时才被片选，被片选后可通信。
  * @param   SPI_SSOE 主模式下NSS输出是否使能  SPIx_SSOE_ENABLE：NSS输出使能，NSS脚持续输出低电平，直至SPI失能。 SPIx_SSOE_DISABLE：禁止NSS输出。
  * @param   SPI_GPIO_Remap 引脚映射
  *		@arg GPIO_Remap_SPI1 将SPI1的NSS SCK MISO MOSI映射到A15 B3 B4 B5
  *		@arg 0x0000 0000 不映射
  * @param   SPI_CPOL 时钟极性(空闲时钟电平)  SPI_CPOL_Low / SPI_CPOL_High
  * @param   SPI_CPHA 时钟相位(数据奇边沿采样还是偶边沿采样)   SPI_CPHA_1Edge / SPI_CPHA_2Edge
  * @param   SPI_FirstBit 设置先发低字节还是高字节  SPI_FirstBit_MSB (高字节)  / SPI_FirstBit_LSB (低字节)
  * @param   SPI_BaudRate_Prescaler 波特率分频因子  SPI_BaudRatePrescaler_2/4/8/16/32/64/128/256
  * @retval 
  * 注： GPIO_Remap_SPI1 引脚B5与IIC1冲突
**/
void BSP_SPIx_Init(SPI_TypeDef* SPIx , uint16_t SPI_Mode, uint16_t SPI_Direction, uint16_t SPI_NSS, uint8_t SPI_SSOE, uint32_t SPI_GPIO_Remap,
					uint16_t SPI_CPOL, uint16_t SPI_CPHA, uint16_t SPI_FirstBit, uint16_t SPI_BaudRate_Prescaler)
{
	
	void (*SPIx_RCC_APBxPeriphClockCmd)(uint32_t RCC_APB2Periph, FunctionalState NewState);
	
	uint32_t SPIx_RCC_APBxPeriph_CLK;//SPI外设时钟
	
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
		SPIx_RCC_APBxPeriph_CLK = RCC_APB2Periph_SPI1;
		
		
		if(SPI_GPIO_Remap==GPIO_Remap_SPI1)
		{
			RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,  ENABLE);
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
			

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
			
			RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,  ENABLE );
			GPIO_PinRemapConfig( SPI_GPIO_Remap, ENABLE);
		}
		else
		{
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
		}

		
		SPIx_GPIO_RCC_APBxPeriphClockCmd = RCC_APB2PeriphClockCmd;
	}
	else if(SPIx == SPI2)
	{
		SPIx_RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		SPIx_RCC_APBxPeriph_CLK = RCC_APB1Periph_SPI2;
		
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
		SPIx_RCC_APBxPeriph_CLK = RCC_APB1Periph_SPI3;
		
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
	
	/*引脚时钟*/
	SPIx_GPIO_RCC_APBxPeriphClockCmd(SPI_NSS_RCC_APBxPeriph_CLK|\
									 SPI_CK_RCC_APBxPeriph_CLK|\
								     SPI_MOSI_RCC_APBxPeriph_CLK|\
									 SPI_MISO_RCC_APBxPeriph_CLK,    ENABLE);//打开引脚端口时钟
	
	if(SPI_Mode == SPI_Mode_Master)//主机
	{
		GPIO_InitStruct_CK.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct_MISO.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_InitStruct_MOSI.GPIO_Mode = GPIO_Mode_AF_PP;
		
		if(SPI_NSS == SPI_NSS_Hard)//选择NSS为硬件控制
		{
			if (SPI_NSS)//SS输出使能
			{
				GPIO_InitStruct_NSS.GPIO_Mode = GPIO_Mode_AF_PP;//NSS向外输出低电平
			}
			else//禁止SS输出
			{
				GPIO_InitStruct_NSS.GPIO_Mode = GPIO_Mode_IPU;//NSS脚被拉低时，设备自动转为从机
			}
		}
	}
	else//从机
	{
		GPIO_InitStruct_CK.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStruct_MISO.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct_MOSI.GPIO_Mode = GPIO_Mode_IPU;
		
		if(SPI_NSS == SPI_NSS_Hard)
		{
			GPIO_InitStruct_NSS.GPIO_Mode = GPIO_Mode_IPU;//NSS为硬件控制
		}
	}

	if(SPI_NSS == SPI_NSS_Hard)//硬件控制才使用NSS，软件控制NSS脚可作为普通IO
	{
		GPIO_InitStruct_NSS.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct_NSS.GPIO_Pin = SPI_NSS_PINx;
		GPIO_Init(SPI_NSS_PORTx, &GPIO_InitStruct_NSS);//将配置的NSS引脚参数写入寄存器
	}

	GPIO_InitStruct_CK.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct_CK.GPIO_Pin = SPI_CK_PINx;
	GPIO_Init(SPI_CK_PORTx, &GPIO_InitStruct_CK);//将配置的SCK引脚参数写入寄存器
	
	
	if((SPI_Direction==SPI_Direction_1Line_Rx) || (SPI_Direction==SPI_Direction_1Line_Tx) )//单线模式
	{
		if(SPI_Mode == SPI_Mode_Master)//单线主模式下MISO未用，MOSI为AFPP
		{
			GPIO_InitStruct_MOSI.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStruct_MOSI.GPIO_Pin = SPI_MOSI_PINx;
			GPIO_InitStruct_MOSI.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(SPI_MOSI_PORTx, &GPIO_InitStruct_MOSI);//将配置的MISO引脚参数写入寄存器
		}
		else //单线从模式下MOSI未用，MISO为AFPP
		{
			GPIO_InitStruct_MISO.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStruct_MISO.GPIO_Pin = SPI_MISO_PINx;
			GPIO_InitStruct_MISO.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(SPI_MISO_PORTx, &GPIO_InitStruct_MISO);//将配置的MISO引脚参数写入寄存器
		}
	}
	else //双线模式 
	{
		GPIO_InitStruct_MOSI.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct_MOSI.GPIO_Pin = SPI_MOSI_PINx;
		GPIO_Init(SPI_MOSI_PORTx, &GPIO_InitStruct_MOSI);//将配置的MISO引脚参数写入寄存器
	
		GPIO_InitStruct_MISO.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct_MISO.GPIO_Pin = SPI_MISO_PINx;
		GPIO_Init(SPI_MISO_PORTx, &GPIO_InitStruct_MISO);//将配置的MISO引脚参数写入寄存器
	}


	

	
	/******************************SPI配置************************************/
	SPI_Cmd(SPIx, DISABLE);//关闭SPI
	SPIx_RCC_APBxPeriphClockCmd(SPIx_RCC_APBxPeriph_CLK, ENABLE);//打开SPI时钟
	SPI_InitTypeDef SPI_InitStruct;

	SPI_InitStruct.SPI_Direction = SPI_Direction ;//定义SPI数据收发方向| 
	SPI_InitStruct.SPI_Mode = SPI_Mode;//主从模式
	SPI_InitStruct.SPI_NSS = SPI_NSS;//选择NSS控制方式

	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//选择单次收发数据大小
	SPI_InitStruct.SPI_CPOL = SPI_CPOL;//选择时钟极性(空闲时钟电平)
	SPI_InitStruct.SPI_CPHA = SPI_CPHA;//选择时钟相位(数据奇边沿采样还是偶边沿采样)
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRate_Prescaler;//设置波特率分频因子
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit;//设置先发低字节还是高字节
	SPI_InitStruct.SPI_CRCPolynomial = 0;//指定CRC校验的多项式，本实验不校验数据，多项式随便写
	
	SPI_Init(SPIx, &SPI_InitStruct);//将配置的SPI参数写入寄存器
	


	
	if((SPI_Mode == SPI_Mode_Master) && ( SPI_NSS == SPI_NSS_Soft))//NSS软件控制
	{
		SPIx->CR1 |= 0x0100; //拉高内部NSS，防止转为从机
	}
	
	if (SPI_SSOE)
	{
		SPIx->CR2 |= (uint16_t)0x0004;
	}
	

	if((SPI_Mode == SPI_Mode_Master)&&(!SPI_SSOE))//主模式SSOE=1时SPI使能就会片选从机
	{
		SPI_Cmd(SPIx, ENABLE);
	}
	
	
}




/**
  * @brief   SPIx单线模式读写切换
  * @param   SPIx：需要配置的SPI：SPI1/2/3
  * @param   Dir ： 数据传输的方向   
  *		@arg 0 ： 读取
  *		@arg 1 ： 发送
  * @retval  
**/
void SPIx_1LineMode_DirSwitch(SPI_TypeDef* SPIx, uint8_t Dir)
{
	SPI_Cmd( SPIx,  DISABLE);

	if(Dir)
	{
		SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Tx);
	}
	else 
	{
		SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Rx);
	}
	SPI_Cmd( SPIx,  ENABLE);
}


#if NSSP_MODE
/**
  * @brief   SPIx软件模拟NSSP取消从机选择
  * @param   SPIx：需要操作的SPI：SPI1/2/3
  * @retval  
**/
void SPIx_NSSP_High(SPI_TypeDef* SPIx)
{
	SPI_Cmd( SPIx,  DISABLE); //  取消选择从机
}

/**
  * @brief   SPIx软件模拟NSSP从机选择
  * @param   SPIx：需要操作的SPI：SPI1/2/3
  * @retval  
**/
void SPIx_NSSP_Low(SPI_TypeDef* SPIx)
{
	SPIx->CR2 |= (uint16_t)0x0004; // SSOE=1  选择从机
	SPI_Cmd( SPIx,  ENABLE);
}

#endif //NSSP_MODE

/**
  * @brief   SPIx超时回调函数
  * @param   errorCode ：错误代码 
  * @retval  default errorCode
**/
uint32_t SPIx_TIMEOUT_UserCallback(uint32_t errorCode)
{
  /* Block communication and all processes */
  SPIx_DEBUG("SPIx 等待超时!errorCode = %d",errorCode);
  
  return errorCode;
}


/**
  * @brief   SPIx双线模式读写数据,无片选
  * @param   SPIx：需要操作的SPI：SPI1/2/3
  * @param   WByte ：需要发送的数据
  * @param[OUT]   RByte ：读取的数据
  * @param   ByteLen ：读写的数据长度
  * @retval  读写状态
  *		@arg 0 ： 成功
  *		@arg else ： 失败
**/
uint32_t SPIx_2Line_RWBytes(SPI_TypeDef* SPIx, uint8_t* WByte, uint8_t* RByte, uint8_t ByteLen)
{
	uint32_t SPIxTimeout; 
	for(uint8_t i=0;i<ByteLen;i++)
	{
		SPIxTimeout = SPIx_LONG_TIMEOUT;
		while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE)==RESET)
		{
			if((SPIxTimeout--)==0)  
			{
				return SPIx_TIMEOUT_UserCallback(ERRCODE_GET_TX_STA_TIMEOUT);
			}
		}
		SPI_I2S_SendData(SPIx, *(WByte+i));//发送数据
		
		SPIxTimeout = SPIx_LONG_TIMEOUT;
		while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE)==RESET)
		{
			if((SPIxTimeout--)==0)  
			{
				return SPIx_TIMEOUT_UserCallback(ERRCODE_GET_RX_STA_TIMEOUT);
			}
		}
		*(RByte+i) = SPI_I2S_ReceiveData(SPIx);//同时读取数据
	}
	
	return 0;
}


/**
  * @brief   SPIx单线模式读数据
  * @param   SPIx ：需要操作的SPI：SPI1/2/3
  * @param[out]   RByte ：读取的数据
  * @param   ByteLen ：读取数据长度
  * @retval  读写状态
  *		@arg 0 ： 成功
  *		@arg else ： 失败
**/
uint32_t SPIx_1Line_RBytes(SPI_TypeDef* SPIx, uint8_t* RByte, uint8_t ByteLen)
{
	uint32_t SPIxTimeout; 
	for(uint8_t i=0;i<ByteLen;i++)
	{
		SPIxTimeout = SPIx_LONG_TIMEOUT;
		while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE)==RESET)
		{
			if((SPIxTimeout--)==0)  
			{
				return SPIx_TIMEOUT_UserCallback(ERRCODE_GET_RX_STA_TIMEOUT);
			}
		}
		*(RByte+i) = SPI_I2S_ReceiveData(SPIx);//同时读取数据
	}
	return 0;
}

/**
  * @brief   SPIx单线模式写数据
  * @param   SPIx ：需要操作的SPI：SPI1/2/3
  * @param   WByte ：需要写入的数据
  * @param   ByteLen ：写入数据长度
  * @retval  读写状态
  *		@arg 0 ： 成功
  *		@arg else ： 失败
**/
uint32_t SPIx_1Line_WBytes(SPI_TypeDef* SPIx, uint8_t* WByte, uint8_t ByteLen)
{
	uint32_t SPIxTimeout; 
	for(uint8_t i=0;i<ByteLen;i++)
	{
		SPIxTimeout = SPIx_LONG_TIMEOUT;
		while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE)==RESET)
		{
			if((SPIxTimeout--)==0)  
			{
				return SPIx_TIMEOUT_UserCallback(ERRCODE_GET_TX_STA_TIMEOUT);
			}
		}
		SPI_I2S_SendData(SPIx, *(WByte+i));//发送数据
	}
	return 0;
}



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








