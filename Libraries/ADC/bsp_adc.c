#include "bsp_adc.h"//板级支持包board support package\
                      仅支持某一个或一批同样的开发板
					  
					  
uint16_t ADCx_Independent_DMA_MemoryBaseAddr[ADC_NumOfChannel];//独立模式

uint32_t ADCx_RegSimult_DMA_MemoryBaseAddr[ADC_NumOfChannel];//双ADC模式


/**
  * @brief   ADC测试
  * @param   
  * @retval  
  * 注：C8T6只有ADC1和ADC2 的CH0-CH9；ADC2没有DMA功能
  */
void ADC_Test()
{


	uint32_t ADCx_Mode = ADC_Mode_Independent;
	uint8_t ADC_Channel_xList[ADC_NumOfChannel] = {ADC_Channel_0, 
													ADC_Channel_1, 
													ADC_Channel_2, 
													ADC_Channel_3};

	
	ADCx_CHx_IndependentCongfig(ADC1, ADC_Channel_xList,4);
	ADC_START(ADC1,  ADCx_Mode);

	if (ADCx_Mode==ADC_Mode_RegSimult)
	{
		float tmpV;
		while(1)
		{
			for(uint8_t i =0; i<ADC_NumOfChannel; i++)
			{
				tmpV = (float)(ADCx_RegSimult_DMA_MemoryBaseAddr[i]&0x0000ffff)/4096.0*3.3;//低16位 ADC1
				UsartPrint("The voltage of ADC1_CH%d is %.6f\n",ADC_Channel_xList[i],tmpV);
				
				
				tmpV = (float)(ADCx_RegSimult_DMA_MemoryBaseAddr[i]>>16)/4096.0*3.3;//高16位 ADC2
				UsartPrint("The voltage of ADC2_CH%d is %.6f\n",ADC_Channel_9 - ADC_Channel_xList[i],tmpV);
				
			}
			UsartPrint("\n\n");
			SisTic_Delay_ms(500);
			
		}
	}
	else
	{
		float tmpV;
		while(1)
		{
			for(uint8_t i =0; i<ADC_NumOfChannel; i++)
			{
				tmpV = (float)ADCx_Independent_DMA_MemoryBaseAddr[i]/4096.0*3.3;
			
				UsartPrint("The voltage of ADC_CH%d is %.6f\n",ADC_Channel_xList[i],tmpV);
				
			}
			UsartPrint("\n\n");
			SisTic_Delay_ms(500);
			
		}
	}






}


/**
  * @brief   获取ADC通道转换结果
  * @param   data[OUT]:转换结果
  * @param   i[IN]:通道转换顺序索引
  * @retval  
  * 注：C8T6只有ADC1和ADC2 的CH0-CH9；ADC2没有DMA功能
  */
void ADC_Get_CHx_Data(uint16_t* data, uint8_t i)
{
	*data = ADCx_Independent_DMA_MemoryBaseAddr[i];

}



/**
  * @brief   初始化ADC，独立模式，单/多通道
  * @param   ADCx ： where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param   ADC_Channel_xList[ADC_NbrOfChannel] ：需要初始化的ADC通道列表, ADC_Channel_x  where x can be 0,1,...,9 .
  * @param   ChannelCount:通道数
  * @retval  
  * 注：C8T6只有ADC1和ADC2 的CH0-CH9；ADC2没有DMA功能
  */
void ADCx_CHx_IndependentCongfig( ADC_TypeDef* ADCx, uint8_t* ADC_Channel_xList, uint8_t ChannelCount )	
{

	/*****************GPIO Config*********************/
	
	GPIO_TypeDef * ADCx_CHx_PORT;
	uint32_t ADCx_CHx_RCC_APBxPeriph;
	uint16_t ADCx_CHx_PIN ;
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	for(uint8_t i=0; i<ChannelCount; i++ )
	{
		if(ADC_Channel_xList[i] <= ADC_Channel_7)
		{
			ADCx_CHx_PORT = GPIOA;
			ADCx_CHx_RCC_APBxPeriph = RCC_APB2Periph_GPIOA;
			ADCx_CHx_PIN = (uint16_t) 0x0001 << ADC_Channel_xList[i];
		}
		else if(ADC_Channel_xList[i] <= ADC_Channel_9)
		{
			ADCx_CHx_PORT = GPIOB;
			ADCx_CHx_RCC_APBxPeriph = RCC_APB2Periph_GPIOB;
			ADCx_CHx_PIN = ((uint16_t) 0x01) << (ADC_Channel_xList[i] - ADC_Channel_8);
		}
	
		RCC_APB2PeriphClockCmd(ADCx_CHx_RCC_APBxPeriph, ENABLE);//打开时钟
		GPIO_InitStruct.GPIO_Pin = ADCx_CHx_PIN;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(ADCx_CHx_PORT, &GPIO_InitStruct);
		
	}
	/*************************************************/
	
	
	
	/**********************ADC Config***************************/
	uint32_t ADCx_RCC_APBxPeriph;
	
	ADCx_RCC_APBxPeriph = (ADCx == ADC1 ? RCC_APB2Periph_ADC1 : RCC_APB2Periph_ADC2);
	
	RCC_APB2PeriphClockCmd(ADCx_RCC_APBxPeriph, ENABLE);//打开ADC时钟
	
	
	ADC_InitTypeDef ADC_InitStruct;
	
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//ADC模式
	ADC_InitStruct.ADC_ScanConvMode =  ChannelCount>1 ?  ENABLE : DISABLE;//扫描模式（是否多通道）
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//是否连续转换
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//外部触转换方式
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//转换结果对齐方式
	ADC_InitStruct.ADC_NbrOfChannel = ChannelCount;//转换通道个数
	
	ADC_Init(ADCx,  &ADC_InitStruct);
	
	
	//ADC时钟频率，通道转换顺序和采样时间
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//ADC最大支持频率14M，配置频率应小于14M
	
	
	for(uint8_t i=0; i<ChannelCount; i++)
	{
		//通道转换顺序和采样时间
		ADC_RegularChannelConfig(ADCx, ADC_Channel_xList[i], i+1,  ADC_SampleTime_28Cycles5);
	}

	/***********************************************************/
	
	/**************************DMA Config***********************/
	if(ADCx==ADC1)
	{
		ADC1_DMA_Config(DMA_PeripheralDataSize_HalfWord, DMA_MemoryDataSize_HalfWord, (uint32_t)ADCx_Independent_DMA_MemoryBaseAddr, ChannelCount);
	}
	

	/***********************************************************/

	
}


/**
  * @brief   初始化ADC，双ADC同步模式，单/多通道
  * @param   ADC_Channel_xList[ADC_NbrOfChannel] ：需要使用的ADC1通道列表, ADC_Channel_x  where x can be 0,1,...,9 . 
													（ADC1_CH0~9对应ADC2_CH9~0）
  * @param   ChannelCount: 通道数												
  * @retval  
  * 注：C8T6只有ADC1和ADC2 的CH0-CH9；ADC2没有DMA功能
  */
void ADCx_CHx_RegSimultCongfig( uint8_t* ADC_Channel_xList, uint8_t ChannelCount )	
{
	
	/*****************GPIO Config*********************/
	
	GPIO_TypeDef * ADCx_CHx_PORT;
	uint32_t ADCx_CHx_RCC_APBxPeriph;
	uint16_t ADCx_CHx_PIN ;
	


	GPIO_InitTypeDef GPIO_InitStruct;
	//ADC1 GPIO
	for(uint8_t i=0; i<ADC_NumOfChannel; i++ )
	{
		if(ADC_Channel_xList[i] <= ADC_Channel_7)
		{
			ADCx_CHx_PORT = GPIOA;
			ADCx_CHx_RCC_APBxPeriph = RCC_APB2Periph_GPIOA;
			ADCx_CHx_PIN = (uint16_t) 0x0001 << ADC_Channel_xList[i];
		}
		else if(ADC_Channel_xList[i] <= ADC_Channel_9)
		{
			ADCx_CHx_PORT = GPIOB;
			ADCx_CHx_RCC_APBxPeriph = RCC_APB2Periph_GPIOB;
			ADCx_CHx_PIN = ((uint16_t) 0x01) << (ADC_Channel_xList[i] - ADC_Channel_8);
		}
	
		RCC_APB2PeriphClockCmd(ADCx_CHx_RCC_APBxPeriph, ENABLE);//打开时钟
		GPIO_InitStruct.GPIO_Pin = ADCx_CHx_PIN;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(ADCx_CHx_PORT, &GPIO_InitStruct);
		
	}
	
	//ADC2 GPIO
	uint8_t ADC2_Channel_x;
	for(uint8_t i=0; i<ADC_NumOfChannel; i++ )
	{
		ADC2_Channel_x = ADC_Channel_9 - ADC_Channel_xList[i];
		if(ADC2_Channel_x <= ADC_Channel_7)
		{
			ADCx_CHx_PORT = GPIOA;
			ADCx_CHx_RCC_APBxPeriph = RCC_APB2Periph_GPIOA;
			ADCx_CHx_PIN = (uint16_t) 0x0001 << ADC2_Channel_x;
		}
		else if(ADC2_Channel_x <= ADC_Channel_9)
		{
			ADCx_CHx_PORT = GPIOB;
			ADCx_CHx_RCC_APBxPeriph = RCC_APB2Periph_GPIOB;
			ADCx_CHx_PIN = ((uint16_t) 0x01) << (ADC2_Channel_x - ADC_Channel_8);
		}
	
		RCC_APB2PeriphClockCmd(ADCx_CHx_RCC_APBxPeriph, ENABLE);//打开时钟
		GPIO_InitStruct.GPIO_Pin = ADCx_CHx_PIN;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(ADCx_CHx_PORT, &GPIO_InitStruct);
		
	}
	/*************************************************/
	
	
	
	/**********************ADC Config***************************/

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);//打开ADC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);//打开ADC时钟
	
	ADC_InitTypeDef ADC_InitStruct;
	
	ADC_InitStruct.ADC_Mode = ADC_Mode_RegSimult;//ADC模式
	ADC_InitStruct.ADC_ScanConvMode =  ADC_NumOfChannel>1 ?  ENABLE : DISABLE;//扫描模式（是否多通道）
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//是否连续转换
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//外部触转换方式
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//转换结果对齐方式
	ADC_InitStruct.ADC_NbrOfChannel = ADC_NumOfChannel;//转换通道个数
	
	ADC_Init(ADC1,  &ADC_InitStruct);
	ADC_Init(ADC2,  &ADC_InitStruct);
	
	//ADC时钟频率，通道转换顺序和采样时间
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADC最大支持频率14M，配置频率应小于14M
	
	
	for(uint8_t i=0; i<ADC_NumOfChannel; i++)
	{
		//通道转换顺序和采样时间
		ADC_RegularChannelConfig(ADC1, ADC_Channel_xList[i], i+1,  ADC_SampleTime_28Cycles5);
		ADC_RegularChannelConfig(ADC2, ADC_Channel_9 - ADC_Channel_xList[i], i+1,  ADC_SampleTime_28Cycles5);
	}

	/***********************************************************/
	
	/**************************DMA Config***********************/
	
	ADC1_DMA_Config(DMA_PeripheralDataSize_Word, DMA_MemoryDataSize_Word,   (uint32_t)ADCx_RegSimult_DMA_MemoryBaseAddr,ChannelCount);


	/***********************************************************/

}


/**
  * @brief   配置ADC1的DMA
  * @param   DMA_PeripheralDataSize ： 外设数据宽度
  * @param   DMA_MemoryDataSize ： 储存器数据宽度
  * @param   DMA_MemoryBaseAddr ： 储存器地址
  * @param   ChannelCount ： 传输数目
  * @retval  
  * 注：C8T6只有ADC1和ADC2 的CH0-CH9；ADC2没有DMA功能
  */
void ADC1_DMA_Config(uint32_t DMA_PeripheralDataSize, uint32_t DMA_MemoryDataSize ,uint32_t DMA_MemoryBaseAddr, uint8_t ChannelCount)	
{
	DMA_DeInit(DMA1_Channel1);
	DMA_InitTypeDef DMA_InitStruct;
	
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);/*打开DMA的时钟(DMA挂在AHB线上)，使用DMAx*/
	
	DMA_InitStruct.DMA_PeripheralBaseAddr 	= (uint32_t)ADC1 + 0x4c;//配置外设数据地址
	DMA_InitStruct.DMA_MemoryBaseAddr 		= (uint32_t)DMA_MemoryBaseAddr; //配置接收数据地址
	DMA_InitStruct.DMA_DIR 					= DMA_DIR_PeripheralSRC;//设置传输方向(从储存器读或从外设读)
	DMA_InitStruct.DMA_BufferSize 			= ChannelCount;//设置传输数目
	DMA_InitStruct.DMA_MemoryInc 			= DMA_MemoryInc_Enable;//设置寄存器地址增量模式，
	DMA_InitStruct.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;//设置外设地址增量模式，
	DMA_InitStruct.DMA_MemoryDataSize 		= DMA_MemoryDataSize;//设置寄存器数据宽度，
	DMA_InitStruct.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize;//设置外设数据宽度，
	DMA_InitStruct.DMA_Mode 				= DMA_Mode_Circular;//设置发送方式，循环发送，有数据就传输
	DMA_InitStruct.DMA_Priority 			= DMA_Priority_High;//设置通道优先级
	DMA_InitStruct.DMA_M2M 					= DMA_M2M_Disable;//MtoM模式
	
	DMA_Init(DMA1_Channel1,  &DMA_InitStruct);
	DMA_Cmd(DMA1_Channel1, ENABLE);//使能DMA通道
	DMA_ClearFlag(DMA1_FLAG_TC1);//清除传输完成标志
	
	ADC_DMACmd( ADC1, ENABLE);//使能ADC的DMA请求
	
}



/**
  * @brief   开启ADC转换
  * @param   ADCx ： where x can be 1, 2 or 3 to select the ADC peripheral.
  *	@param   ADC_Mode ：ADC模式 : 若为 ADC_Mode_RegSimult  ADCx可任意
  * @retval  
  * 注：C8T6只有ADC1和ADC2 的CH0-CH9；ADC2没有DMA功能
  */
void ADC_START(ADC_TypeDef* ADCx, uint32_t ADC_Mode)
{
	
	if(ADC_Mode==ADC_Mode_RegSimult)
	{
		ADC_Cmd(ADC1, ENABLE);//打开ADC
		ADC_ResetCalibration(ADC1);//初始化ADC校准寄存器
		while(ADC_GetResetCalibrationStatus( ADC1)); //等待校准寄存器初始化完成
		ADC_StartCalibration(ADC1);//开启校准
		while(ADC_GetCalibrationStatus(ADC1));//等待校准完成
		
		ADC_Cmd(ADC2, ENABLE);//打开ADC
		ADC_ResetCalibration(ADC2);//初始化ADC校准寄存器
		while(ADC_GetResetCalibrationStatus( ADC2)); //等待校准寄存器初始化完成
		ADC_StartCalibration(ADC2);//开启校准
		while(ADC_GetCalibrationStatus(ADC2));//等待校准完成
		
		ADC_ExternalTrigConvCmd( ADC2, ENABLE );
		if(ADC_SOFTWARE_START)
		{
			ADC_SoftwareStartConvCmd(ADC1, ENABLE );//软件触发ADC转换
		
		}
		
		
		
		
		return;
	}
	
	ADC_Cmd(ADCx, ENABLE);//打开ADC

	ADC_ResetCalibration(ADCx);//初始化ADC校准寄存器
	
	while(ADC_GetResetCalibrationStatus( ADCx)); //等待校准寄存器初始化完成
	
	ADC_StartCalibration(ADCx);//开启校准
	
	while(ADC_GetCalibrationStatus(ADCx));//等待校准完成


	
	
	if(ADC_SOFTWARE_START)
	{
		ADC_SoftwareStartConvCmd(ADCx, ENABLE );//软件触发ADC转换
	}
	
}





