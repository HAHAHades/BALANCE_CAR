#include "bsp_DMA.h"


/*定义数组Const_Buffer作为DMA传输数据源
const 关键字将改签数组定义为常量型
表示储存在内部FLASH中*/
//const uint32_t Const_Buffer[]={0x00212230,0x00512830,0x00212287,0x00021230,
//										  0x00212290,0x00218230,0x00211230,0x00212640,
//										  0x00212230,0x00512830,0x00212287,0x00021230,
//										  0x00212290,0x00218230,0x00211230,0x00212640,
//										  0x00212230,0x00512830,0x00212987,0x00021340,
//										  0x00212290,0x00218230,0x00411230,0x00212640,
//										  0x09212230,0x00512830,0x00212287,0x00021258,
//										  0x00212290,0x00218230,0x00211231,0x00245640,
//										  0x00212230,0x00512830,0x00212287,0x00021230,
//										  0x00212290,0x00218230,0x00211230,0x00212640,
//										  0x00212230,0x00512830,0x00212287,0x00021230,
//										  0x00212290,0x00218230,0x00211230,0x00212640,
//										  0x00212230,0x00512830,0x00212987,0x00021340,
//										  0x00212290,0x00218230,0x00411230,0x00212640,
//										  0x09212230,0x00512830,0x00212287,0x00021258,
//										  0x00212290,0x00218230,0x00211231,0x00245640
//};

										  



/**
  * @brief   配置MtoM
  * @param   
  *		@arg DMAy_Channelx：需要使用的DMA通道
  *		@arg DMA_PeripheralBaseAddr： 数据传输的储存器地址1
  *		@arg DMA_MemoryBaseAddr：数据传输的储存器地址2
  *		@arg DMA_DIR：传输方向 : M2P(2->1) / P2M(1->2)
  *		@arg DMA_BufferSize：数据传输数目
  *		@arg DMA_PeripheralDataSize：数据宽度：DMA_PeripheralDataSize_Byte/DMA_PeripheralDataSize_HalfWord/DMA_PeripheralDataSize_Word 
  *		@arg DMA_PeripheralInc：外设地址增量模式：DMA_PeripheralInc_Enable/DMA_PeripheralInc_Disable
  *		@arg DMA_MemoryInc：寄存器地址增量模式：DMA_MemoryInc_Enable/DMA_MemoryInc_Disable
  * @retval  
**/

void DMA_M2M_Init(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t DMA_PeripheralBaseAddr, 
				  uint32_t DMA_MemoryBaseAddr, uint32_t DMA_DIR, uint32_t DMA_BufferSize, 
				  uint32_t DMA_PeripheralDataSize, uint32_t DMA_PeripheralInc, uint32_t DMA_MemoryInc)
{


	DMA_InitTypeDef DMA_InitStruct;
	
	DMA_InitStruct.DMA_PeripheralBaseAddr 	= (uint32_t)DMA_PeripheralBaseAddr;//配置外设数据地址
	DMA_InitStruct.DMA_MemoryBaseAddr 		= (uint32_t)DMA_MemoryBaseAddr; //配置接收数据地址
	DMA_InitStruct.DMA_DIR 					= DMA_DIR;//设置传输方向(从储存器读或从外设读)
	DMA_InitStruct.DMA_BufferSize 			= DMA_BufferSize;//设置传输数目
	DMA_InitStruct.DMA_MemoryInc 			= DMA_MemoryInc_Enable;//设置寄存器地址增量模式，
	DMA_InitStruct.DMA_PeripheralInc 		= DMA_PeripheralInc_Enable;//设置外设地址增量模式，
	//DMA_InitStruct.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;//设置寄存器数据宽度，
	DMA_InitStruct.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize;//设置外设数据宽度，
	DMA_InitStruct.DMA_Mode 				= DMA_Mode_Normal;//设置发送方式，单次发送
	DMA_InitStruct.DMA_Priority 			= DMA_Priority_High;//设置通道优先级
	DMA_InitStruct.DMA_M2M 					= DMA_M2M_Enable;//MtoM模式
	
	
	
	if(DMA_PeripheralDataSize==DMA_PeripheralDataSize_HalfWord)
	{
		DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	}
	else if(DMA_PeripheralDataSize==DMA_PeripheralDataSize_Word)
	{
		DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	}
	else
	{
		DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	}

	
	
	DMAy_Channelx_Config(DMAy_Channelx,DMA_InitStruct);//配置DMA对应的通道
	
	DMA_Cmd(DMAy_Channelx, ENABLE);//使能DMA
	

}



/**
  * @brief   配置串口的DMA
  * @param   
  *		@arg USARTx：需要配置的串口
  *		@arg USART_DMAReq： 需要配置的DMA请求： USART_DMAReq_Tx/USART_DMAReq_Rx
  *		@arg DMA_MemoryBaseAddr：数据传输的储存器地址
  *		@arg DMA_DIR：传输方向 :  M2P / P2M  (DMA_DIR_PeripheralDST/DMA_DIR_PeripheralSRC)
  *		@arg DMA_BufferSize：数据传输数目
  *		@arg DMA_MemoryInc ：寄存器地址是否自动增加 ： DMA_MemoryInc_Enable/DMA_MemoryInc_Disable
  * @retval  
**/

void DMA_USART_Init(USART_TypeDef *USARTx, uint16_t USART_DMAReq, uint32_t DMA_MemoryBaseAddr, 
	                uint32_t DMA_DIR, uint32_t DMA_BufferSize, uint32_t DMA_MemoryInc)
{
	
	
	DMA_Channel_TypeDef* DMAy_Channelx;
	DMA_InitTypeDef DMA_InitStruct;
	
	//DMA_InitStruct.DMA_PeripheralBaseAddr 	= (uint32_t)DMA_PeripheralBaseAddr;//配置外设数据地址
	DMA_InitStruct.DMA_MemoryBaseAddr 		= (uint32_t)DMA_MemoryBaseAddr; //配置接收数据地址
	DMA_InitStruct.DMA_DIR 					= DMA_DIR;//设置传输方向(从储存器读或从外设读)
	DMA_InitStruct.DMA_BufferSize 			= DMA_BufferSize;//设置传输数目
	DMA_InitStruct.DMA_MemoryInc 			= DMA_MemoryInc;//设置寄存器地址增量模式，
	DMA_InitStruct.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;//设置外设地址增量模式，
	DMA_InitStruct.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;//设置寄存器数据宽度，
	DMA_InitStruct.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;//设置外设数据宽度，
	DMA_InitStruct.DMA_Mode 				= DMA_Mode_Normal;//设置发送方式，单次发送
	DMA_InitStruct.DMA_Priority 			= DMA_Priority_High;//设置通道优先级
	DMA_InitStruct.DMA_M2M 					= DMA_M2M_Disable;//MtoM模式
	
	

	
	if(USARTx==USART1)
	{
		DMA_InitStruct.DMA_PeripheralBaseAddr = USART1_BASE + 0x04;//串口的DR数据寄存器
		DMAy_Channelx = USART_DMAReq==USART_DMAReq_Tx ? DMA1_Channel4 :  DMA1_Channel5;
	}
	else if(USARTx==USART2)
	{
		DMA_InitStruct.DMA_PeripheralBaseAddr = USART2_BASE + 0x04;
		DMAy_Channelx = USART_DMAReq==USART_DMAReq_Tx ? DMA1_Channel7 :  DMA1_Channel6;
	}
	else if(USARTx==USART3)
	{
		DMA_InitStruct.DMA_PeripheralBaseAddr = USART3_BASE + 0x04;
		DMAy_Channelx = USART_DMAReq==USART_DMAReq_Tx ? DMA1_Channel2 :  DMA1_Channel3;
	}
	else
	{return;}
	
	
	DMAy_Channelx_Config(DMAy_Channelx, DMA_InitStruct);//配置DMA对应的通道
	
	DMA_Cmd(DMAy_Channelx, ENABLE);//使能DMA
	USART_DMACmd(USARTx, USART_DMAReq, ENABLE);//使能USARTx的DMA
	
}












/**
  * @brief   配置DMA通道
  * @param   
  *		@arg DMAy_Channelx：需要配置的通道
  *		@arg DMA_PeripheralBaseAddr：需要传输数据的外设数据地址
  *		@arg DMA_MemoryBaseAddr：数据传输的储存器地址
  *		@arg DMA_DIR：传输方向 : M2P / P2M  (DMA_DIR_PeripheralDST/DMA_DIR_PeripheralSRC)
  *		@arg DMA_BufferSize：数据传输数目
  *		@arg DMA_PeripheralDataSize：外设数据宽度：DMA_PeripheralDataSize_Byte/DMA_PeripheralDataSize_HalfWord/DMA_PeripheralDataSize_Word
  *		@arg DMA_MemoryDataSize：寄存器数据宽度：DMA_MemoryDataSize_Byte/DMA_MemoryDataSize_HalfWord/DMA_MemoryDataSize_Word
  *		@arg DMA_M2M：是否允许 M2M 传输
  * @retval  
**/

void DMAy_Channelx_Config(DMA_Channel_TypeDef* DMAy_Channelx, DMA_InitTypeDef DMA_InitStruct)
{
	
	
	uint32_t RCC_AHBPeriph_DMAx;
	uint32_t DMAy_FLAG_TCx;
	
	if(DMAy_Channelx == DMA1_Channel1)
	{
		RCC_AHBPeriph_DMAx = RCC_AHBPeriph_DMA1;
		DMAy_FLAG_TCx = DMA1_FLAG_TC1;
	}
	else if(DMAy_Channelx == DMA1_Channel2)
	{
		RCC_AHBPeriph_DMAx = RCC_AHBPeriph_DMA1;
		DMAy_FLAG_TCx = DMA1_FLAG_TC2;
	}
	else if(DMAy_Channelx == DMA1_Channel3)
	{
		RCC_AHBPeriph_DMAx = RCC_AHBPeriph_DMA1;
		DMAy_FLAG_TCx = DMA1_FLAG_TC3;
	}
	else if(DMAy_Channelx == DMA1_Channel4)
	{
		RCC_AHBPeriph_DMAx = RCC_AHBPeriph_DMA1;
		DMAy_FLAG_TCx = DMA1_FLAG_TC4;
	}
	else if(DMAy_Channelx == DMA1_Channel5)
	{
		RCC_AHBPeriph_DMAx = RCC_AHBPeriph_DMA1;
		DMAy_FLAG_TCx = DMA1_FLAG_TC5;
	}
	else if(DMAy_Channelx == DMA1_Channel6)
	{
		RCC_AHBPeriph_DMAx = RCC_AHBPeriph_DMA1;
		DMAy_FLAG_TCx = DMA1_FLAG_TC6;
	}
	else if(DMAy_Channelx == DMA1_Channel7)
	{
		RCC_AHBPeriph_DMAx = RCC_AHBPeriph_DMA1;
		DMAy_FLAG_TCx = DMA1_FLAG_TC7;
	}
	else if(DMAy_Channelx == DMA2_Channel1)
	{
		RCC_AHBPeriph_DMAx = RCC_AHBPeriph_DMA2;
		DMAy_FLAG_TCx = DMA2_FLAG_TC1;
	}
	else if(DMAy_Channelx == DMA2_Channel2)
	{
		RCC_AHBPeriph_DMAx = RCC_AHBPeriph_DMA2;
		DMAy_FLAG_TCx = DMA2_FLAG_TC2;
	}
	else if(DMAy_Channelx == DMA2_Channel3)
	{
		RCC_AHBPeriph_DMAx = RCC_AHBPeriph_DMA2;
		DMAy_FLAG_TCx = DMA2_FLAG_TC3;
	}
	else if(DMAy_Channelx == DMA2_Channel4)
	{
		RCC_AHBPeriph_DMAx = RCC_AHBPeriph_DMA2;
		DMAy_FLAG_TCx = DMA2_FLAG_TC4;
	}	
	else if(DMAy_Channelx == DMA2_Channel5)
	{
		RCC_AHBPeriph_DMAx = RCC_AHBPeriph_DMA2;
		DMAy_FLAG_TCx = DMA2_FLAG_TC5;
	}
	else 
	{
		return;
	}
	

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMAx,ENABLE);/*打开DMA的时钟(DMA挂在AHB线上)，使用DMAx*/

//	DMA_InitStruct.DMA_PeripheralBaseAddr 	= (uint32_t)DMA_PeripheralBaseAddr;//配置外设数据地址，由于是寄存器传给寄存器，所以把数据源的寄存器看作外设
//	DMA_InitStruct.DMA_MemoryBaseAddr 		= (uint32_t)DMA_MemoryBaseAddr; //配置接收数据地址
//	DMA_InitStruct.DMA_DIR 					= DMA_DIR;//设置传输方向(从储存器读或从外设读)，从储存器读
//	DMA_InitStruct.DMA_BufferSize 			= DMA_BufferSize;//设置传输数目
//	DMA_InitStruct.DMA_MemoryInc 			= DMA_MemoryInc_Enable;//设置寄存器地址增量模式，执行增量操作
//	DMA_InitStruct.DMA_PeripheralInc 		= DMA_PeripheralInc_Enable;//设置外设地址增量模式，执行增量操作
//	DMA_InitStruct.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Word;//设置寄存器数据宽度，32位
//	DMA_InitStruct.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Word;//设置外设数据宽度，32位
//	DMA_InitStruct.DMA_Mode 				= DMA_Mode_Normal;//设置发送方式，单次发送
//	DMA_InitStruct.DMA_Priority 			= DMA_Priority_High;//设置通道优先级
//	DMA_InitStruct.DMA_M2M 					= DMA_M2M;//允许MtoM模式
	
	DMA_Init(DMAy_Channelx, &DMA_InitStruct);//将DMA结构体配置写入寄存器(选择DMA1的通道5)
	
	DMA_ClearFlag(DMAy_FLAG_TCx);//清除传输完成标志
	
//	DMA_Cmd(DMAy_Channelx, ENABLE);//使能DMA
}









/*
判断两个数组是否相同
相同返回1
否则返回0
*/
uint8_t BufferCMP(const uint32_t *SentBuffer, uint32_t *ReBuffer,uint16_t BSize)
{
	while(BSize--)
	{
		if(*SentBuffer != *ReBuffer)
			return 0;
		SentBuffer++;
		ReBuffer++;
	}
	return 1;
}

