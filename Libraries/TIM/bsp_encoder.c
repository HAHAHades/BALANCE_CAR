
#include "bsp_encoder.h"





void BSP_ENCODER_UsageDemo(void)
{

	TIMx_CHx_ENCODER_Init(TIM2,  TIM_Channel_1, TIM2_IO_Reamp0 ,  BSP_Encoder_DirF);

	int16_t FirstCount = Get_Encoder_Count(TIM2);


	//after 10ms
	int16_t LastCount = Get_Encoder_Count(TIM2);


	//若一圈11个脉冲，减速比30,编码器直连电机，则输出速度为
	float MSpeed = (LastCount-FirstCount)*100/(30*4*11);// rps,正交编码器模式下一个脉冲计数4次


}





/**
  * @brief   定时器编码器接口初始化
  * @param   ENCODER_TIMx ：要使用的定时器
  * @param   TIM_Channel_x1 ：使用定时器的通道(相对通道要成对使用), TIM_Channel_x1=TIM_Channel_1/3时， TIM_Channel_x2=TIM_Channel_2/4
  * @param   GPIO_Remapping： @ref TIMx_IO_Reamp
  * @param   Dir ：编码器A B相顺序， 
  *		@arg BSP_Encoder_DirF(0):AB，正向
  *		@arg BSP_Encoder_DirB(1):BA，反向
  * @retval  
  */
void TIMx_CHx_ENCODER_Init(TIM_TypeDef * ENCODER_TIMx, uint16_t TIM_Channel_x1,
						uint32_t GPIO_Remapping , _Bool Dir)
{
	
	if(!IS_TIM_ALL_PERIPH(ENCODER_TIMx))return;
	if(!IS_TIM_CHANNEL(TIM_Channel_x1))return;
	
	TIM_Cmd(ENCODER_TIMx,DISABLE); //关闭定时器
	
	void (*TIMx_RCC_APBxPeriphClockCmd)(uint32_t RCC_APBxPeriph, FunctionalState NewState);
	void (*TIMx_CHx_PORT_CMDFUN)(uint32_t RCC_APBxPeriph, FunctionalState NewState);
	uint32_t TIMx_CLK;
	uint32_t ENCODER_GPIOx1_CLK;
	uint32_t ENCODER_GPIOx2_CLK;
	GPIO_TypeDef * ENCODER_GPIOx1;
	GPIO_TypeDef * ENCODER_GPIOx2;
	uint16_t ENCODER_GPIO_Pin_x1;
	uint16_t ENCODER_GPIO_Pin_x2;
	
	uint16_t TIM_Channel_x2 = ((TIM_Channel_x1==0x0008) || (TIM_Channel_x1== 0x0000)) ? TIM_Channel_x1+4 : TIM_Channel_x1-4 ;
	
	uint32_t GPIO_RemapX;
	uint8_t IO_RemapFlag = 0;

	if(ENCODER_TIMx==TIM1)//TIM1 的 CH1234 分别在 A8 A9 A10 A11 
	{
		TIMx_CLK = RCC_APB2Periph_TIM1;
		TIMx_RCC_APBxPeriphClockCmd = RCC_APB2PeriphClockCmd;
		TIMx_CHx_PORT_CMDFUN = RCC_APB2PeriphClockCmd;
		//默认使用CH1 CH2
		ENCODER_GPIOx1_CLK = RCC_APB2Periph_GPIOA;
		ENCODER_GPIOx2_CLK = RCC_APB2Periph_GPIOA;
		ENCODER_GPIOx1 = GPIOA;
		ENCODER_GPIOx2 = GPIOA;
		
		ENCODER_GPIO_Pin_x1 =  GPIO_Pin_8;

		ENCODER_GPIO_Pin_x2 =  GPIO_Pin_9;

	    if(TIM_Channel_x1==TIM_Channel_3 || TIM_Channel_x1==TIM_Channel_4)//使用CH3 CH4
		{
			ENCODER_GPIO_Pin_x1 =  GPIO_Pin_10;
	
			ENCODER_GPIO_Pin_x2 =  GPIO_Pin_11;
		}
	}
	else if (ENCODER_TIMx==TIM2)//TIM2 的 CH1234 分别在 A0 A1 A2 A3 
	{	
		TIMx_CLK = RCC_APB1Periph_TIM2;
		TIMx_RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		TIMx_CHx_PORT_CMDFUN = RCC_APB2PeriphClockCmd;
		
		//默认使用CH1 CH2
		ENCODER_GPIOx1_CLK = RCC_APB2Periph_GPIOA;
		ENCODER_GPIOx2_CLK = RCC_APB2Periph_GPIOA;
		ENCODER_GPIOx1 = GPIOA;
		ENCODER_GPIOx2 = GPIOA;
		
		ENCODER_GPIO_Pin_x1 =  GPIO_Pin_0;
		
		ENCODER_GPIO_Pin_x2 =  GPIO_Pin_1;
		

	    if(TIM_Channel_x1==TIM_Channel_3 || TIM_Channel_x1==TIM_Channel_4)//使用CH3 CH4
		{
			ENCODER_GPIO_Pin_x1 =  GPIO_Pin_2;
			
			ENCODER_GPIO_Pin_x2 =  GPIO_Pin_3;
		}

		if(GPIO_Remapping==TIM2_IO_Reamp1)// CH1234 -> A15 B3 A2 A3
		{
			IO_RemapFlag = 1;
			GPIO_RemapX = GPIO_PartialRemap1_TIM2;
			if(TIM_Channel_x1==TIM_Channel_1 || TIM_Channel_x1==TIM_Channel_2)
			{
				ENCODER_GPIO_Pin_x1 =  GPIO_Pin_15;

				ENCODER_GPIOx2_CLK = RCC_APB2Periph_GPIOB;
				ENCODER_GPIOx2 = GPIOB;
				ENCODER_GPIO_Pin_x2 =  GPIO_Pin_3;
			}
		}
		else if(GPIO_Remapping==TIM2_IO_Reamp2)// CH1234 -> A0 A1 B10 B11
		{
			IO_RemapFlag = 1;
			GPIO_RemapX = GPIO_PartialRemap2_TIM2;

			if(TIM_Channel_x1==TIM_Channel_3 || TIM_Channel_x1==TIM_Channel_4)
			{
				ENCODER_GPIOx1_CLK = RCC_APB2Periph_GPIOB;
				ENCODER_GPIOx1 = GPIOB;
				ENCODER_GPIO_Pin_x1 =  GPIO_Pin_10;
				
				ENCODER_GPIOx2_CLK = RCC_APB2Periph_GPIOB;
				ENCODER_GPIOx2 = GPIOB;
				ENCODER_GPIO_Pin_x2 =  GPIO_Pin_11;
			}
		}
		else if(GPIO_Remapping==TIM2_IO_Reamp3)// CH1234 -> A15 B3 B10 B11
		{
			IO_RemapFlag = 1;
			GPIO_RemapX = GPIO_FullRemap_TIM2;
			
			if(TIM_Channel_x1==TIM_Channel_1 || TIM_Channel_x1==TIM_Channel_2)
			{
				ENCODER_GPIO_Pin_x1 =  GPIO_Pin_15;

				ENCODER_GPIOx2_CLK = RCC_APB2Periph_GPIOB;
				ENCODER_GPIOx2 = GPIOB;
				ENCODER_GPIO_Pin_x2 =  GPIO_Pin_3;
			}
			else if(TIM_Channel_x1==TIM_Channel_3 || TIM_Channel_x1==TIM_Channel_4)
			{
				ENCODER_GPIOx1_CLK = RCC_APB2Periph_GPIOB;
				ENCODER_GPIOx1 = GPIOB;
				ENCODER_GPIO_Pin_x1 =  GPIO_Pin_10;
				
				ENCODER_GPIOx2_CLK = RCC_APB2Periph_GPIOB;
				ENCODER_GPIOx2 = GPIOB;
				ENCODER_GPIO_Pin_x2 =  GPIO_Pin_11;
			}
			
		}
		
	}	
	else if (ENCODER_TIMx==TIM3)//TIM3 的 CH1234 分别在 A6 A7 B0 B1
	{	
		TIMx_CLK = RCC_APB1Periph_TIM3;
		TIMx_RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		TIMx_CHx_PORT_CMDFUN = RCC_APB2PeriphClockCmd;
		
		//默认使用CH1 CH2
		ENCODER_GPIOx1_CLK = RCC_APB2Periph_GPIOA;
		ENCODER_GPIOx2_CLK = RCC_APB2Periph_GPIOA;
		ENCODER_GPIOx1 = GPIOA;
		ENCODER_GPIOx2 = GPIOA;
		ENCODER_GPIO_Pin_x1 =  GPIO_Pin_6;
		ENCODER_GPIO_Pin_x2 =  GPIO_Pin_7;
	    if(TIM_Channel_x1==TIM_Channel_3 || TIM_Channel_x1==TIM_Channel_4)//使用CH3 CH4
		{
			ENCODER_GPIOx1_CLK = RCC_APB2Periph_GPIOB;
			ENCODER_GPIOx1 = GPIOB;
			ENCODER_GPIO_Pin_x1 =  GPIO_Pin_0;
		
			ENCODER_GPIOx2_CLK = RCC_APB2Periph_GPIOB;
			ENCODER_GPIOx2 = GPIOB;
			ENCODER_GPIO_Pin_x2 =  GPIO_Pin_1;
		}

		if(GPIO_Remapping==TIM3_IO_Reamp1)// CH1234 -> B4 B5 B0 B1
		{
			IO_RemapFlag = 1;
			GPIO_RemapX = GPIO_PartialRemap_TIM3;
			if(TIM_Channel_x1==TIM_Channel_1 || TIM_Channel_x1==TIM_Channel_2)
			{
				ENCODER_GPIOx1_CLK = RCC_APB2Periph_GPIOB;
				ENCODER_GPIOx1 = GPIOB;
				ENCODER_GPIO_Pin_x1 =  GPIO_Pin_4;

				ENCODER_GPIOx2_CLK = RCC_APB2Periph_GPIOB;
				ENCODER_GPIOx2 = GPIOB;
				ENCODER_GPIO_Pin_x2 =  GPIO_Pin_5;
			}
		}
	}	
	else if (ENCODER_TIMx==TIM4)//TIM4 的 CH1234 分别在 B6 B7 B8 B9
	{
		TIMx_CLK = RCC_APB1Periph_TIM4;
		TIMx_RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		TIMx_CHx_PORT_CMDFUN = RCC_APB2PeriphClockCmd;
		
		//默认使用CH1 CH2
		ENCODER_GPIOx1_CLK = RCC_APB2Periph_GPIOB;
		ENCODER_GPIOx1 = GPIOB;
		ENCODER_GPIOx2_CLK = RCC_APB2Periph_GPIOB;
		ENCODER_GPIOx2 = GPIOB;
		ENCODER_GPIO_Pin_x1 =  GPIO_Pin_6;
		ENCODER_GPIO_Pin_x2 =  GPIO_Pin_7;
		if(TIM_Channel_x1==TIM_Channel_3 || TIM_Channel_x1==TIM_Channel_4)
		{
			ENCODER_GPIO_Pin_x1 =  GPIO_Pin_8;
			ENCODER_GPIO_Pin_x2 =  GPIO_Pin_9;
		}
	}
	
	if(IO_RemapFlag!=0)//某些引脚重映射需要禁用JTAG和SWD
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//开启IO复用时钟
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//完全禁用SWD及JTAG
	
		GPIO_PinRemapConfig(GPIO_RemapX, ENABLE);//开启外设IO映射
	}
	
	
	TIMx_RCC_APBxPeriphClockCmd(TIMx_CLK, ENABLE);//开启TIMx时钟
	TIMx_CHx_PORT_CMDFUN(ENCODER_GPIOx1_CLK | ENCODER_GPIOx2_CLK, ENABLE);// 开启GPIOx时钟

	/******************GPIO配置****************/
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;// 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = ENCODER_GPIO_Pin_x1;
	GPIO_Init(ENCODER_GPIOx1, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = ENCODER_GPIO_Pin_x2;
	GPIO_Init(ENCODER_GPIOx2, &GPIO_InitStruct);
	
	/******************  TIM配置 ****************/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseStructInit( &TIM_TimeBaseInitStruct);
	TIM_ICInitTypeDef TIM_ICInitStruct;
	TIM_ICStructInit( &TIM_ICInitStruct);

	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 0xffff;// 自动重装载值，计数器在0-TIM_Period之间计数，给定0xffff，则从0向下计数到0xffff为-1，0xfffe为-2...
	TIM_TimeBaseInitStruct.TIM_Prescaler = 1-1;// 分频器值,编码器模式下一般不分频，计数器按照编码器每一个脉冲进行计数
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter =0;// 未用
	TIM_TimeBaseInit(ENCODER_TIMx, &TIM_TimeBaseInitStruct);//时基结构体配置
	
	
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_x1; //使用的通道
	TIM_ICInitStruct.TIM_ICFilter = 0; //滤波器
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;//触发计数边沿
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;//计数器分频
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;//相对通道是否互换
	TIM_ICInit(ENCODER_TIMx, &TIM_ICInitStruct);
	
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_x2;
	TIM_ICInit(ENCODER_TIMx, &TIM_ICInitStruct);
	
	uint16_t TIM_ICPolarity = Dir ? TIM_ICPolarity_Falling : TIM_ICPolarity_Rising;//通道是否反向
	
	TIM_EncoderInterfaceConfig(ENCODER_TIMx, TIM_EncoderMode_TI12,
                                TIM_ICPolarity,  TIM_ICPolarity);//编码器接口配置，使用双通道(正交编码器)，通道是否反向
													
	TIM_Cmd(ENCODER_TIMx,ENABLE); //开启定时器
											
}



/**
  * @brief   获取编码器位置并清零计数器
  * @param   TIMx ：编码器所使用的定时器
  *		@arg 
  * @retval  编码器当前位置(定时器计数器值)
  */
int16_t Get_Encoder_Count(TIM_TypeDef* TIMx)
{

	uint16_t count = TIM_GetCounter(TIMx);
	
	TIM_SetCounter( TIMx,  0);//计数器清零
	
	return ((int16_t)count);
}



