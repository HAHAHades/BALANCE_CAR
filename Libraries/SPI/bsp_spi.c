
#include "bsp_spi.h"


#if 0
void BSP_SPI_Init(void)
{
	SPI_InitTypeDef SPI_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	
	/******************************GPIO配置***********************************/
	//模式配置见参考手册（通用复用功能I/O）
	/*NSS引脚*/
	SPI_NSS_CLKCMD_FUNC(SPI_NSS_CLK,ENABLE);//打开NSS引脚端口时钟
	
	#if Master //主机
		#if NSS_Soft //选择NSS控制方式
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//选择NSS为软件控制
		#else
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//选择NSS为硬件控制
		#endif /*NSS_Soft*/
	#else //从机
		#if NSS_Soft //选择NSS控制方式
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//选择NSS为软件控制（暂无）
		#else
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//选择NSS为硬件控制
		#endif /*NSS_Soft*/
	#endif /*Master*/
	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = SPI_NSS_PIN;
	GPIO_Init(SPI_NSS_PORT, &GPIO_InitStruct);//将配置的NSS引脚参数写入寄存器
	
	/*CK引脚*/
	SPI_CK_CLKCMD_FUNC(SPI_CK_CLK,ENABLE);//打开CK引脚端口时钟
	#if Master //主机
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	#else //从机
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	#endif /*Master*/
	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = SPI_CK_PIN;
	GPIO_Init(SPI_CK_PORT, &GPIO_InitStruct);//将配置的NSS引脚参数写入寄存器
	
	/*MISO引脚*/
	SPI_MISO_CLKCMD_FUNC(SPI_MISO_CLK,ENABLE);//打开MISO引脚端口时钟
	#if Master //主机
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	#else //从机
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	#endif /*Master*/
	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = SPI_MISO_PIN;
	GPIO_Init(SPI_MISO_PORT, &GPIO_InitStruct);//将配置的MISO引脚参数写入寄存器
	
	/*MOSI引脚*/
	SPI_MOSI_CLKCMD_FUNC(SPI_MOSI_CLK,ENABLE);//打开MOSI引脚端口时钟
	#if Master //主机
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	#else //从机
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	#endif /*Master*/
	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = SPI_MOSI_PIN;
	GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStruct);//将配置的MISO引脚参数写入寄存器
	
	
	/******************************SPI配置************************************/
	SPI_CLKCMD_FUNC(SPI_CLK_PORT,ENABLE);//打开SPI外设时钟
	
	#if SINGLE_LINE //单线模式
		SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx ;//定义SPI数据收发方向| SPI_Direction_1Line_Rx
	#else
		SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//定义SPI数据收发方向
	#endif /*SINGLE_LINE*/
	
	#if Master //主机模式
		SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//选择SPI主
	#else
		SPI_InitStruct.SPI_Mode = SPI_Mode_Slave;//选择SPI从
	#endif /*Master*/
	
	#if NSS_Soft //选择NSS控制方式
		SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//选择NSS为软件控制
	#else
		SPI_InitStruct.SPI_NSS = SPI_NSS_Hard;//选择NSS为硬件控制
	#endif /*NSS_Soft*/
	
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//选择单次收发数据大小
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;//选择时钟极性(空闲时钟电平)
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;//选择时钟相位(数据奇边沿采样还是偶边沿采样)
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//设置波特率分频因子
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//设置先发低字节还是高字节
	SPI_InitStruct.SPI_CRCPolynomial = 0;//指定CRC校验的多项式，本实验不校验数据，多项式随便写
	
	SPI_Init(SPI_NUM, &SPI_InitStruct);//将配置的SPI参数写入寄存器
	

	
	#if SINGLE_LINE //单线模式
		#if SINGLE_LINE_ReadOnly //单线只读
			SPI_BiDirectionalLineConfig(SPI_NUM, SPI_Direction_Rx);
		#else //单线只写
			SPI_BiDirectionalLineConfig(SPI_NUM, SPI_Direction_Tx);
		#endif /*SINGLE_LINE_ReadOnly*/
	#endif /*SINGLE_LINE*/
	
	#if Master //主机模式
	SPI_NSS_HIGH();//取消选择从机
	#endif /*Master*/

	SPI_Cmd(SPI_NUM, DISABLE);
	//SPI_Cmd(SPI_NUM, ENABLE);
	
	/******************配置中断******************/
//	SPI_NVIC_Config();//中断配置
//	SPI_I2S_ITConfig(SPI_NUM, SPI_I2S_IT_RXNE, DISABLE);//关闭接收中断
//	SPI_I2S_ITConfig(SPI_NUM, SPI_I2S_IT_TXE, DISABLE);//关闭发送中断
}	

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

#endif //if 0


BSP_SPIx_TypeDef BSP_SPI1_Struct;
BSP_SPIx_TypeDef BSP_SPI2_Struct;

void SPI_UsageDemo()
{
	BSP_SPI1_Struct.SPIx = SPI1;
	BSP_SPI1_Struct.SPI_GPIO_Remap = SPI1_IO_Reamp0;
	BSP_SPI1_Struct.SPI_InitStruct = BSP_SPIxStructInit( SPI_Mode_Master,  SPI_Direction_2Lines_FullDuplex);
	BSP_SPIx_Init( &BSP_SPI1_Struct);

}



void BSP_SPIx_NSS_STA_LOW(BSP_SPIx_TypeDef* BSP_SPIx_Struct)
{
	GPIO_ResetBits(BSP_SPIx_Struct->SPI_NSS_PORTx, BSP_SPIx_Struct->SPI_NSS_PINx);

}
void BSP_SPIx_NSS_STA_HIGH(BSP_SPIx_TypeDef* BSP_SPIx_Struct)
{
	GPIO_SetBits(BSP_SPIx_Struct->SPI_NSS_PORTx, BSP_SPIx_Struct->SPI_NSS_PINx);

}


/**
  * @brief   SPI 初始化
  * @param   SPIx:  要使用的SPIx
  * @param   SPI_InitStruct:  SPIx的配置
  * @param   SPI_GPIO_Remap:  IO重映射 @ref SPIx_IO_Reamp
  * @retval  
  *		@arg SPIx_RET_Flag_OK:初始化成功
  *		@arg other:初始化失败
  */
uint8_t BSP_SPIx_Init(BSP_SPIx_TypeDef* BSP_SPIx_Struct)
{
	uint8_t retVal = SPIx_RET_Flag_OK;
	void (*SPIx_RCC_APBxPeriphClockCmd)(uint32_t RCC_APBxPeriph, FunctionalState NewState);//spi时钟使能函数
	
	uint32_t SPIx_RCC_APBxPeriph_CLK;//SPI外设时钟，spi2/3 APB1  spi1/4/5/6 APB2

	BSP_SPIx_Struct->SPI_NSS_HIGH = BSP_SPIx_NSS_STA_HIGH;
	BSP_SPIx_Struct->SPI_NSS_LOW = BSP_SPIx_NSS_STA_LOW;

	GPIO_TypeDef* SPI_NSS_PORTx;
	uint16_t SPI_NSS_PINx;

	GPIO_TypeDef* SPI_CK_PORTx;
	uint16_t SPI_CK_PINx;

	GPIO_TypeDef* SPI_MISO_PORTx;
	uint16_t SPI_MISO_PINx;

	GPIO_TypeDef* SPI_MOSI_PORTx;
	uint16_t SPI_MOSI_PINx;

	if (BSP_SPIx_Struct->SPIx == SPI1)
	{
		/* code */
		SPIx_RCC_APBxPeriphClockCmd = RCC_APB2PeriphClockCmd;
		SPIx_RCC_APBxPeriph_CLK = RCC_APB2Periph_SPI1;

		BSP_SPIx_Struct->SPI_NSS_PORTx = GPIOA;
		BSP_SPIx_Struct->SPI_NSS_PINx = GPIO_Pin_4;

		SPI_NSS_PORTx = GPIOA;
		SPI_NSS_PINx = GPIO_Pin_4;

		SPI_CK_PORTx = GPIOA;
		SPI_CK_PINx = GPIO_Pin_5;

		SPI_MISO_PORTx = GPIOA;
		SPI_MISO_PINx = GPIO_Pin_6;

		SPI_MOSI_PORTx = GPIOA;
		SPI_MOSI_PINx = GPIO_Pin_7;

		if (BSP_SPIx_Struct->SPI_GPIO_Remap == SPI1_IO_Reamp1)
		{
			BSP_SPIx_Struct->SPI_NSS_PORTx = GPIOA;
			BSP_SPIx_Struct->SPI_NSS_PINx = GPIO_Pin_15;

			SPI_NSS_PORTx = GPIOA;
			SPI_NSS_PINx = GPIO_Pin_15;

			SPI_CK_PORTx = GPIOB;
			SPI_CK_PINx = GPIO_Pin_3;

			SPI_MISO_PORTx = GPIOB;
			SPI_MISO_PINx = GPIO_Pin_4;

			SPI_MOSI_PORTx = GPIOB;
			SPI_MOSI_PINx = GPIO_Pin_5;

			RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,  ENABLE);
			GPIO_PinRemapConfig( GPIO_Remap_SPI1,  ENABLE);
		}
	}
	else if (BSP_SPIx_Struct->SPIx == SPI2)
	{
		SPIx_RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		SPIx_RCC_APBxPeriph_CLK = RCC_APB1Periph_SPI2;

		BSP_SPIx_Struct->SPI_NSS_PORTx = GPIOB;
		BSP_SPIx_Struct->SPI_NSS_PINx = GPIO_Pin_12;

		SPI_NSS_PORTx = GPIOB;
		SPI_NSS_PINx = GPIO_Pin_12;

		SPI_CK_PORTx = GPIOB;
		SPI_CK_PINx = GPIO_Pin_13;

		SPI_MISO_PORTx = GPIOB;
		SPI_MISO_PINx = GPIO_Pin_14;

		SPI_MOSI_PORTx = GPIOB;
		SPI_MOSI_PINx = GPIO_Pin_15;
	}
	else
	{
		retVal = SPIx_RET_Flag_Init_Error;
	}

	SPIx_RCC_APBxPeriphClockCmd(SPIx_RCC_APBxPeriph_CLK, ENABLE);
	SPI_Cmd(BSP_SPIx_Struct->SPIx, DISABLE);

	/******************************GPIO配置***********************************/
	if (BSP_SPIx_Struct->SPI_InitStruct.SPI_Mode == SPI_Mode_Master)//选择SPI主
	{
		if (BSP_SPIx_Struct->SPI_InitStruct.SPI_NSS == SPI_NSS_Hard)//选择NSS为硬件控制
		{
			BSP_GPIO_Config(SPI_NSS_PORTx, SPI_NSS_PINx,  GPIO_Mode_AF_PP);
		}
		else //选择NSS为软件控制
		{
			BSP_GPIO_Config(SPI_NSS_PORTx, SPI_NSS_PINx,  GPIO_Mode_Out_PP);
		}

		BSP_GPIO_Config(SPI_CK_PORTx, SPI_CK_PINx,  GPIO_Mode_AF_PP);
		BSP_GPIO_Config(SPI_MISO_PORTx, SPI_MISO_PINx,  GPIO_Mode_IN_FLOATING);
		BSP_GPIO_Config(SPI_MOSI_PORTx, SPI_MOSI_PINx,  GPIO_Mode_AF_PP);
	}
	else if (BSP_SPIx_Struct->SPI_InitStruct.SPI_Mode == SPI_Mode_Slave)//选择SPI从
	{
		BSP_GPIO_Config(SPI_NSS_PORTx, SPI_NSS_PINx,  GPIO_Mode_IN_FLOATING);
		BSP_GPIO_Config(SPI_CK_PORTx, SPI_CK_PINx,  GPIO_Mode_AF_PP);
		BSP_GPIO_Config(SPI_MISO_PORTx, SPI_MISO_PINx,  GPIO_Mode_AF_PP);
		BSP_GPIO_Config(SPI_MOSI_PORTx, SPI_MOSI_PINx,  GPIO_Mode_IN_FLOATING);
	}
	else
	{
		retVal = SPIx_RET_Flag_Init_Error;
	}

	if (BSP_SPIx_Struct->SPI_InitStruct.SPI_Direction == SPI_Direction_1Line_Tx)
	{
		//单线只写
		SPI_BiDirectionalLineConfig(BSP_SPIx_Struct->SPIx, SPI_Direction_Tx);
	}
	if (BSP_SPIx_Struct->SPI_InitStruct.SPI_Direction == SPI_Direction_1Line_Rx)
	{
		//单线只读
		SPI_BiDirectionalLineConfig(BSP_SPIx_Struct->SPIx, SPI_Direction_Rx);
	}
	// if (BSP_SPIx_Struct->SPI_InitStruct.SPI_Mode == SPI_Mode_Master)
	// {
	// 	/* 主机模式 */
	// 	SPI_NSS_HIGH();//取消选择从机
	// }
	
	SPI_Init(BSP_SPIx_Struct->SPIx, &(BSP_SPIx_Struct->SPI_InitStruct));//将配置的SPI参数写入寄存器
	SPI_Cmd(BSP_SPIx_Struct->SPIx, ENABLE);

	return retVal;
}

// /**
//   * @brief   按默认方式配置SPI结构体
//   * @param   SPI_Mode： SPI模式 ： SPI_Mode_Master/SPI_Mode_Slave 
//   * @param   SPI_Direction：数据传输的方向
//   *		@arg SPI_Direction_2Lines_FullDuplex 双线全双工
//   *		@arg SPI_Direction_2Lines_RxOnly     双线只读 通常用于多个从设备情况下，从设备接收时禁止发送可防止影响总线
//   *		@arg SPI_Direction_1Line_Rx			 单线读取  
//   *		@arg SPI_Direction_1Line_Tx			 单线发送
//   *		注：单线模式下，主设备使用MOSI 从设备使用MISO
//   * @param   SPI_NSS：NSS控制方式 : SPI_NSS_Soft(SSM=1) / SPI_NSS_Hard(SSM=0)  
//   *		注：软件NSS与硬件NSS所使用引脚默认相同。推荐主机配置为软件NSS，从机配置为硬件NSS。
// 			选择软件NSS时，SPI的NSS电平由SPIx_CR1的SSI决定，在NSS引脚上的输入IO操作无效
// 			若把NSS脚配置为OUT_PP则可直接在NSS脚上进行输出IO操作，从设备会读取到NSS脚的状态。
// 			选择硬件NSS时，SPIx_CR2的SSOE决定设备的主从模式，SSOE为0，此设备为从设备，NSS脚被总线上的主设备控制；
// 			SSOE为1时，此设备为主设备，NSS被持续拉低直至SPI失能，若想SPI失能后NSS输出高电平则引脚需上拉。
// 		例：若设备工作在硬件主模式，即SSM=0,MSTR=1;SSOE=0且NSS脚接高电平设备才能正常工作，若与从设备通信则需另外的IO来片选从设备，共占用5个IO。
// 												SSOE=1，NSS脚持续输出低电平直至SPI失能，共占用4个IO。
// 			若设备工作在软件主模式，即SSM=1,MSTR=1;此时NSS脚可作为普通IO，SSI=1，设备才能正常工作，若与从设备通信则需另外的IO来片选从设备，共占用4个IO。（常用）
// 			若设备工作在硬件从模式，即SSM=0,MSTR=0;此时NSS脚从主设备的IO接收片选信号，被片选后可通信。（常用）
// 			若设备工作在软件从模式，即SSM=1,MSTR=0;此时NSS脚可作为普通IO，SSI=0时才被片选，被片选后可通信。
//   * @param   SPI_SSOE 主模式下NSS输出是否使能  SPIx_SSOE_ENABLE：NSS输出使能，NSS脚持续输出低电平，直至SPI失能。 SPIx_SSOE_DISABLE：禁止NSS输出。
//   * @param   SPI_CPOL 时钟极性(空闲时钟电平)  SPI_CPOL_Low / SPI_CPOL_High
//   * @param   SPI_CPHA 时钟相位(数据奇边沿采样还是偶边沿采样)   SPI_CPHA_1Edge / SPI_CPHA_2Edge
//   * @param   SPI_FirstBit 设置先发低字节还是高字节  SPI_FirstBit_MSB (高字节)  / SPI_FirstBit_LSB (低字节)
//   * @param   SPI_BaudRatePrescaler 波特率分频因子  SPI_BaudRatePrescaler_2/4/8/16/32/64/128/256
//   * @param   SPI_DataSize 数据位 SPI_DataSize_8b / SPI_DataSize_16b
//   * @retval 
//   * 注： GPIO_Remap_SPI1 引脚B5与IIC1冲突
// **/
// SPI_InitTypeDef BSP_SPIxStructInit(uint16_t SPI_Mode, uint16_t SPI_Direction, uint16_t SPI_NSS, uint8_t SPI_SSOE, uint16_t SPI_CPOL, 
// 								   uint16_t SPI_CPHA, uint16_t SPI_FirstBit, uint16_t SPI_BaudRatePrescaler, uint16_t SPI_DataSize)
// {

// 	SPI_InitTypeDef SPI_InitStruct;

// 	SPI_InitStruct.SPI_Mode = SPI_Mode;
// 	SPI_InitStruct.SPI_Direction = SPI_Direction;
// 	SPI_InitStruct.SPI_NSS = SPI_NSS;
// 	SPI_InitStruct.SPI_DataSize = SPI_DataSize;//选择单次收发数据大小
// 	SPI_InitStruct.SPI_CPOL = SPI_CPOL;//选择时钟极性(空闲时钟电平)
// 	SPI_InitStruct.SPI_CPHA = SPI_CPHA;//选择时钟相位(数据奇边沿采样还是偶边沿采样)
// 	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler;//设置波特率分频因子
// 	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit;//设置先发低字节还是高字节
// 	SPI_InitStruct.SPI_CRCPolynomial = 0;//指定CRC校验的多项式，本实验不校验数据，多项式随便写
	
// 	return SPI_InitStruct;

// }

SPI_InitTypeDef BSP_SPIxStructInit(uint16_t SPI_Mode, uint16_t SPI_Direction)
{
	SPI_InitTypeDef SPI_InitStruct;

	SPI_InitStruct.SPI_Mode = SPI_Mode;
	SPI_InitStruct.SPI_Direction = SPI_Direction;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//选择NSS为软件控制
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//选择单次收发数据大小
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;//选择时钟极性(空闲时钟电平)
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;//选择时钟相位(数据奇边沿采样还是偶边沿采样)
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//设置波特率分频因子
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//设置先发低字节还是高字节
	SPI_InitStruct.SPI_CRCPolynomial = 0;//指定CRC校验的多项式，本实验不校验数据，多项式随便写
	
	return SPI_InitStruct;
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
				return (ERRCODE_GET_TX_STA_TIMEOUT);
			}
		}
		SPI_I2S_SendData(SPIx, *(WByte+i));//发送数据
		
		SPIxTimeout = SPIx_LONG_TIMEOUT;
		while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE)==RESET)
		{
			if((SPIxTimeout--)==0)  
			{
				return (ERRCODE_GET_RX_STA_TIMEOUT);
			}
		}
		*(RByte+i) = SPI_I2S_ReceiveData(SPIx);//同时读取数据
	}
	
	return 0;
}


#if 0
#if SINGLE_LINE /****************单线模式*****************/

	#if Master /*************(单线)主机模式**************/
		/*
		发送数据
		*/	
		void Spi_WriteByte(uint8_t Byte)
		{
			SPI_NSS_LOW();//选择从机
			while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE)!=SET);
			SPI_I2S_SendData(SPI_NUM, Byte);
			while(SPI_I2S_GetFlagStatus(SPI_NUM,SPI_I2S_FLAG_BSY)!=RESET);//等待SPI线空闲
			SPI_NSS_HIGH();//取消选择从机
		}

		/*
		接收数据
		*/
		void Spi_ReadByte(uint8_t Byte,uint8_t* RByte)
		{
			uint8_t i;
			//SPI_Cmd(SPI_NUM, DISABLE);
			
			SPI_NSS_LOW();//选择从机
			while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE)!=SET);
			SPI_I2S_SendData(SPI_NUM, Byte);
			//SPI_Cmd(SPI_NUM, ENABLE);
			while(SPI_I2S_GetFlagStatus(SPI_NUM,SPI_I2S_FLAG_BSY)!=RESET);//等待SPI线空闲
			SPI_BiDirectionalLineConfig(SPI_NUM, SPI_Direction_Rx);//切换为只读
			for(i=0; i<4; i++)
			{
				while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE)!=SET);
				*(RByte+i) = SPI_I2S_ReceiveData(SPI_NUM);
			}
			while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_BSY)!=RESET);//等待传输结束
			SPI_NSS_HIGH();//取消选择从机
			SPI_BiDirectionalLineConfig(SPI_NUM, SPI_Direction_Tx);//切换为只写
			
		}
	#else /*********(单线)从机模式*************/
		/*
		发送数据
		*/	
		void Spi_WriteByte(uint8_t Byte)
		{
			SPI_BiDirectionalLineConfig(SPI_NUM, SPI_Direction_Tx);//切换为只写
			while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE)!=SET);
			SPI_I2S_SendData(SPI_NUM, Byte);
			SPI_BiDirectionalLineConfig(SPI_NUM, SPI_Direction_Rx);//切换为只读
		}

		/*
		接收数据
		*/
		void Spi_ReadByte(uint8_t Byte,uint8_t* RByte)
		{
			while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE)!=SET);
			*RByte = SPI_I2S_ReceiveData(SPI_NUM);
		}
	#endif /*Master*/




#else /**************双线模式*************/

	#if Master /*************(双线)主机模式**************/
	/*
	发送数据
	*WByte:发出的数据
	*RByte:发出时接收到的数据
	*/
	void Spi_WriteByte(uint8_t* WByte,uint8_t* RByte)
	{
		SPI_NSS_LOW();//选择从机
		while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE)!=SET);
		SPI_I2S_SendData(SPI_NUM, *WByte);
		while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE)!=SET);
		SPI_I2S_ReceiveData(SPI_NUM);//同时读取数据
		SPI_NSS_HIGH();//取消选择从机
	}

	/*
	接收数据
	WByte:接收时发出的数据的地址
	RByte:接收到的数据的地址
	*/
	uint8_t Spi_ReadByte(uint8_t* WByte,uint8_t* RByte)
	{
		uint8_t i=0;
		SPI_NSS_LOW();//选择从机
		/*读写数组*/
		for(;i<4;i++)
		{
			while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE)!=SET);
			SPI_I2S_SendData(SPI_NUM, *(WByte+i));
			while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE)!=SET);
			*(RByte+i) = SPI_I2S_ReceiveData(SPI_NUM);//同时读取数据
		}
		/*读写单字节数据*/
	//		while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE)!=SET);
	//		SPI_I2S_SendData(SPI_NUM, *(WByte));
	//		while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE)!=SET);
	//		*(RByte) = SPI_I2S_ReceiveData(SPI_NUM);//同时读取数据
			
		SPI_NSS_HIGH();//取消选择从机
		return *RByte;//返回读取的数据（数组的第一个数据）
	}
	
	#else /*********(双线)从机模式*************/
	/*
	发送数据
	*WByte:发出的数据
	*RByte:发出时接收到的数据
	*/
	void Spi_WriteByte(uint8_t* WByte,uint8_t* RByte)
	{
		while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE)!=SET);
		SPI_I2S_SendData(SPI_NUM, *WByte);
		while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE)!=SET);
		SPI_I2S_ReceiveData(SPI_NUM);//同时读取数据
	}

	/*
	接收数据
	WByte:接收时发出的数据的地址
	RByte:接收到的数据的地址
	*/
	uint8_t Spi_ReadByte(uint8_t* WByte,uint8_t* RByte)
	{
		uint8_t i=0;
		/*读写数组*/
		for(;i<4;i++)
		{
			while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE)!=SET);
			SPI_I2S_SendData(SPI_NUM, *(WByte+i));
			while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE)!=SET);
			*(RByte+i) = SPI_I2S_ReceiveData(SPI_NUM);//同时读取数据
		}
		/*读写单字节数据*/
	//		while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE)!=SET);
	//		SPI_I2S_SendData(SPI_NUM, *(WByte));
	//		while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE)!=SET);
	//		*(RByte) = SPI_I2S_ReceiveData(SPI_NUM);//同时读取数据
		
		return *RByte;//返回读取的数据（数组的第一个数据）
	}

	#endif /*Master*/
	
#endif /*SINGLE_LINE*/
#endif //if 0
