
#include "bsp_pwm.h"


/**
  * @brief   定时器PWM功能初始化
  * @param   
  *		@arg PWM_TIMx：要使用的定时器
  *		@arg TIM_Channel_x：使用定时器的通道
  *		@arg GPIO_Remapping：定时器引脚是否进行重映射，0x00:不重映射;0x01:部分重映射1;0x10:部分重映射2;0x11:完全重映射;
  *		@arg reload：预装载比较值
  *		@arg psc：时钟分频因子；频率=72M/(psc*reload)
  * @retval  
  */


void TIMx_CHx_PWM1_Init(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x, 
						uint8_t GPIO_Remapping ,u16 reload, u16 psc)
{
	
	
	if(!IS_TIM_ALL_PERIPH(PWM_TIMx))return;
	if(!IS_TIM_CHANNEL(TIM_Channel_x))return;
	
	TIM_Cmd(PWM_TIMx,DISABLE); //关闭定时器
	
	void (*RCC_APBxPeriphClockCmd)(uint32_t RCC_APBxPeriph, FunctionalState NewState);
	void (*TIMx_CHx_PORT_CMDFUN)(uint32_t RCC_APBxPeriph, FunctionalState NewState);
	uint32_t TIMx_CLK;
	uint32_t PWM_GPIOx_CLK;
	GPIO_TypeDef * PWM_GPIOx;
	uint16_t PWM_GPIO_Pin_x;
	
	void (*TIMx_OCxInit)(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
	void (*TIMx_OCxPreloadConfig)(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
	
	uint32_t GPIO_RemapX;
	
	
	if(PWM_TIMx==TIM1)//TIM1 的 CH1234 分别在 A8 A9 A10 A11 
	{
		TIMx_CLK = RCC_APB2Periph_TIM1;
		RCC_APBxPeriphClockCmd = RCC_APB2PeriphClockCmd;
		TIMx_CHx_PORT_CMDFUN = RCC_APB2PeriphClockCmd;
		PWM_GPIOx_CLK = RCC_APB2Periph_GPIOA;
		PWM_GPIOx = GPIOA;
		PWM_GPIO_Pin_x = GPIO_Pin_8;
		TIMx_OCxInit = TIM_OC1Init;
		TIMx_OCxPreloadConfig = TIM_OC1PreloadConfig;
		
		if(TIM_Channel_x==TIM_Channel_1)
		{
			PWM_GPIO_Pin_x = GPIO_Pin_8;
			TIMx_OCxInit = TIM_OC1Init;
			TIMx_OCxPreloadConfig = TIM_OC1PreloadConfig;
		}
		else if(TIM_Channel_x==TIM_Channel_2)
		{
			PWM_GPIO_Pin_x = GPIO_Pin_9;
			TIMx_OCxInit = TIM_OC2Init;
			TIMx_OCxPreloadConfig = TIM_OC2PreloadConfig;
		}
		else if(TIM_Channel_x==TIM_Channel_3)
		{
			PWM_GPIO_Pin_x =  GPIO_Pin_10;
			TIMx_OCxInit = TIM_OC3Init;
			TIMx_OCxPreloadConfig = TIM_OC3PreloadConfig;
		}
		else //(TIM_Channel_x==TIM_Channel_4)
		{
			PWM_GPIO_Pin_x = GPIO_Pin_11;
			TIMx_OCxInit = TIM_OC4Init;
			TIMx_OCxPreloadConfig = TIM_OC4PreloadConfig;
		}
		
	}
	else if (PWM_TIMx==TIM2)//TIM2 的 CH1234 分别在 A0 A1 A2 A3 
	{	
		TIMx_CLK = RCC_APB1Periph_TIM2;
		RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		TIMx_CHx_PORT_CMDFUN = RCC_APB2PeriphClockCmd;
		PWM_GPIOx_CLK = RCC_APB2Periph_GPIOA;
		PWM_GPIOx = GPIOA;
		PWM_GPIO_Pin_x = GPIO_Pin_1;
		TIMx_OCxInit = TIM_OC2Init;
		TIMx_OCxPreloadConfig = TIM_OC2PreloadConfig;
		
		if(TIM_Channel_x==TIM_Channel_1||TIM_Channel_x==TIM_Channel_2)
		{
			PWM_GPIO_Pin_x = TIM_Channel_x == TIM_Channel_1 ? GPIO_Pin_0 : GPIO_Pin_1;
			TIMx_OCxInit = TIM_Channel_x == TIM_Channel_1 ? TIM_OC1Init : TIM_OC2Init;
			TIMx_OCxPreloadConfig = TIM_Channel_x == TIM_Channel_1 ? TIM_OC1PreloadConfig : TIM_OC2PreloadConfig;
		}
		else if(TIM_Channel_x==TIM_Channel_3||TIM_Channel_x==TIM_Channel_4)
		{
			PWM_GPIO_Pin_x = TIM_Channel_x == TIM_Channel_3 ? GPIO_Pin_2 : GPIO_Pin_3;
			TIMx_OCxInit = TIM_Channel_x == TIM_Channel_3 ? TIM_OC3Init : TIM_OC4Init;
			TIMx_OCxPreloadConfig = TIM_Channel_x == TIM_Channel_3 ? TIM_OC3PreloadConfig : TIM_OC4PreloadConfig;
		}
		if(GPIO_Remapping==0x01)// CH1234 -> A15 B3 A2 A3
		{
			GPIO_RemapX = GPIO_PartialRemap1_TIM2;
			if(TIM_Channel_x==TIM_Channel_1)
			{
				PWM_GPIO_Pin_x =  GPIO_Pin_15 ;
			}
			else if(TIM_Channel_x==TIM_Channel_2)
			{
				PWM_GPIOx_CLK = RCC_APB2Periph_GPIOB;
				PWM_GPIOx = GPIOB;
				PWM_GPIO_Pin_x =  GPIO_Pin_3;
			}
		}
		else if(GPIO_Remapping==0x10)// CH1234 -> A0 A1 B10 B11
		{
			GPIO_RemapX = GPIO_PartialRemap2_TIM2;

			if(TIM_Channel_x==TIM_Channel_3)
			{
				PWM_GPIOx_CLK = RCC_APB2Periph_GPIOB;
				PWM_GPIOx = GPIOB;
				PWM_GPIO_Pin_x =  GPIO_Pin_10 ;
			}
			else if(TIM_Channel_x==TIM_Channel_4)
			{
				PWM_GPIOx_CLK = RCC_APB2Periph_GPIOB;
				PWM_GPIOx = GPIOB;
				PWM_GPIO_Pin_x =  GPIO_Pin_11;
			}
		}
		else if(GPIO_Remapping==0x11)// CH1234 -> A15 B3 B10 B11
		{
			GPIO_RemapX = GPIO_FullRemap_TIM2;
			if(TIM_Channel_x==TIM_Channel_1)
			{
				PWM_GPIO_Pin_x =  GPIO_Pin_15 ;
			}
			else if(TIM_Channel_x==TIM_Channel_2)
			{
				PWM_GPIOx_CLK = RCC_APB2Periph_GPIOB;
				PWM_GPIOx = GPIOB;
				PWM_GPIO_Pin_x =  GPIO_Pin_3;
			}
			else if(TIM_Channel_x==TIM_Channel_3)
			{
				PWM_GPIOx_CLK = RCC_APB2Periph_GPIOB;
				PWM_GPIOx = GPIOB;
				PWM_GPIO_Pin_x =  GPIO_Pin_10 ;
			}
			else if(TIM_Channel_x==TIM_Channel_4)
			{
				PWM_GPIOx_CLK = RCC_APB2Periph_GPIOB;
				PWM_GPIOx = GPIOB;
				PWM_GPIO_Pin_x =  GPIO_Pin_11;
			}
		}
		
	}	
	else if (PWM_TIMx==TIM3)//TIM3 的 CH1234 分别在 A6 A7 B0 B1
	{	
		TIMx_CLK = RCC_APB1Periph_TIM3;
		RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		TIMx_CHx_PORT_CMDFUN = RCC_APB2PeriphClockCmd;
		PWM_GPIOx_CLK = RCC_APB2Periph_GPIOB;
		PWM_GPIOx = GPIOB;
		if(TIM_Channel_x==TIM_Channel_1||TIM_Channel_x==TIM_Channel_2)
		{
			PWM_GPIOx_CLK = RCC_APB2Periph_GPIOA;
			PWM_GPIOx = GPIOA;
			PWM_GPIO_Pin_x = TIM_Channel_x == TIM_Channel_1 ? GPIO_Pin_6 : GPIO_Pin_7;
			TIMx_OCxInit = TIM_Channel_x == TIM_Channel_1 ? TIM_OC1Init : TIM_OC2Init;
			TIMx_OCxPreloadConfig = TIM_Channel_x == TIM_Channel_1 ? TIM_OC1PreloadConfig : TIM_OC2PreloadConfig;
		}
		else if(TIM_Channel_x==TIM_Channel_3||TIM_Channel_x==TIM_Channel_4)
		{
			PWM_GPIOx_CLK = RCC_APB2Periph_GPIOB;
			PWM_GPIOx = GPIOB;
			PWM_GPIO_Pin_x = TIM_Channel_x == TIM_Channel_3 ? GPIO_Pin_0: GPIO_Pin_1;
			TIMx_OCxInit = TIM_Channel_x == TIM_Channel_3 ? TIM_OC3Init : TIM_OC4Init;
			TIMx_OCxPreloadConfig = TIM_Channel_x == TIM_Channel_3 ? TIM_OC3PreloadConfig : TIM_OC4PreloadConfig;
		}
		
		if(GPIO_Remapping==0x01||GPIO_Remapping==0x01)// CH1234 -> B4 B5 B0 B1
		{
			GPIO_RemapX = GPIO_PartialRemap_TIM3;
			if(TIM_Channel_x==TIM_Channel_1)
			{
				PWM_GPIO_Pin_x =  GPIO_Pin_4 ;
			}
			else if(TIM_Channel_x==TIM_Channel_2)
			{
				PWM_GPIO_Pin_x =  GPIO_Pin_5;
			}
		}
		
	}	
	else if (PWM_TIMx==TIM4)//TIM4 的 CH1234 分别在 B6 B7 B8 B9
	{
		TIMx_CLK = RCC_APB1Periph_TIM4;
		RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		TIMx_CHx_PORT_CMDFUN = RCC_APB2PeriphClockCmd;
		PWM_GPIOx_CLK = RCC_APB2Periph_GPIOB;
		PWM_GPIOx = GPIOB;
		TIM_Channel_x = GPIO_Pin_6;
		TIMx_OCxInit = TIM_OC1Init;
		TIMx_OCxPreloadConfig = TIM_OC1PreloadConfig;
		
		if(TIM_Channel_x==TIM_Channel_2)
		{
			PWM_GPIO_Pin_x = GPIO_Pin_7;
			TIMx_OCxInit = TIM_OC2Init;
			TIMx_OCxPreloadConfig = TIM_OC2PreloadConfig;
		}
		else if(TIM_Channel_x==TIM_Channel_3)
		{
			PWM_GPIO_Pin_x = GPIO_Pin_8;
			TIMx_OCxInit = TIM_OC3Init;
			TIMx_OCxPreloadConfig = TIM_OC3PreloadConfig;
		}
		else if(TIM_Channel_x==TIM_Channel_4)
		{
			PWM_GPIO_Pin_x = GPIO_Pin_9;
			TIMx_OCxInit = TIM_OC4Init;
			TIMx_OCxPreloadConfig = TIM_OC4PreloadConfig;
		}
	
	}
	
	
	if(GPIO_Remapping!=0x00)//某些引脚重映射需要禁用JTAG和SWD
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//开启IO复用时钟
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//完全禁用SWD及JTAG
	
		GPIO_PinRemapConfig(GPIO_RemapX, ENABLE);//开启外设IO映射
		
	}
	
	
	
	RCC_APBxPeriphClockCmd(TIMx_CLK, ENABLE);//开启TIMx时钟
	TIMx_CHx_PORT_CMDFUN(PWM_GPIOx_CLK, ENABLE);// 开启GPIOx时钟

	
	
	/******************GPIO配置****************/
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;// 复用推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = PWM_GPIO_Pin_x;
	
	GPIO_Init(PWM_GPIOx, &GPIO_InitStruct);
	
	/******************  TIM配置 ****************/
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseStructInit( &TIM_TimeBaseInitStruct);


	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = reload;// 自动重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc;// 分频器值
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter =0;// 未用
	TIM_TimeBaseInit(PWM_TIMx, &TIM_TimeBaseInitStruct);//时基结构体配置
	

	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性,当定时器计数值小于CCR1_Val时为高电平
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//使能
	TIM_OCInitStruct.TIM_Pulse = 0; //设置初始PWM比较脉冲宽度为0	
	TIMx_OCxInit(PWM_TIMx, &TIM_OCInitStruct);//注意此函数无法初始化TIM6，TIM7
	TIMx_OCxPreloadConfig(PWM_TIMx,TIM_OCPreload_Enable);//使能CCR3上的TIMx外设预装载寄存器，\
																注意此函数无法使能TIM6，TIM7
	
	//自动重装载
	TIM_ARRPreloadConfig(PWM_TIMx,ENABLE);	//使能ARR上的TIMx外设预加装寄存器											
																
	TIM_Cmd(PWM_TIMx,ENABLE); //开启定时器
	if(PWM_TIMx==TIM1)
	{
		TIM_CtrlPWMOutputs(PWM_TIMx,  ENABLE);//主输出使能，只有高级定时器有
	}
																
}


void TIMx_PWM1_RloadPsc_Set(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x ,u16 reload, u16 psc)
{
	
	TIM_Cmd(PWM_TIMx,DISABLE); //关闭定时器
	
	void (*TIMx_OCxInit)(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
	void (*TIMx_OCxPreloadConfig)(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
	
	
	TIMx_OCxInit = TIM_OC1Init;
	TIMx_OCxPreloadConfig = TIM_OC1PreloadConfig;
	
	if(TIM_Channel_x==TIM_Channel_2)
	{
		TIMx_OCxInit = TIM_OC2Init;
		TIMx_OCxPreloadConfig = TIM_OC2PreloadConfig;
	}
	else if(TIM_Channel_x==TIM_Channel_3)
	{
		TIMx_OCxInit = TIM_OC3Init;
		TIMx_OCxPreloadConfig = TIM_OC3PreloadConfig;
	}
	else if(TIM_Channel_x==TIM_Channel_4)
	{
		TIMx_OCxInit = TIM_OC4Init;
		TIMx_OCxPreloadConfig = TIM_OC4PreloadConfig;
	}
	
	
	
	/******************  TIM配置 ****************/
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;


	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = reload;// 自动重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc;// 分频器值
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter =0;// 未用
	TIM_TimeBaseInit(PWM_TIMx, &TIM_TimeBaseInitStruct);//时基结构体配置
	

	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性,当定时器计数值小于CCR1_Val时为高电平
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//使能
	TIM_OCInitStruct.TIM_Pulse = 0; //设置初始PWM脉冲宽度为0	
	TIMx_OCxInit(PWM_TIMx, &TIM_OCInitStruct);//注意此函数无法初始化TIM6，TIM7
	TIMx_OCxPreloadConfig(PWM_TIMx,TIM_OCPreload_Enable);//使能CCR3上的TIMx外设预装载寄存器，\
																注意此函数无法使能TIM6，TIM7
	
	
	//自动重装载
	TIM_ARRPreloadConfig(PWM_TIMx,ENABLE);	//使能ARR上的TIMx外设预加装寄存器											
																
	TIM_Cmd(PWM_TIMx,ENABLE); //开启定时器
	
	
}

/**************占空比*********************/
void PWM1_SetCompare(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x, u16 cmp)
{

	if(TIM_Channel_x==TIM_Channel_1)
	{
		TIM_SetCompare1(PWM_TIMx,cmp);
	}
	else if(TIM_Channel_x==TIM_Channel_2)
	{
		TIM_SetCompare2(PWM_TIMx,cmp);
	}
	else if(TIM_Channel_x==TIM_Channel_3)
	{
		TIM_SetCompare3(PWM_TIMx,cmp);
	}
	else if(TIM_Channel_x==TIM_Channel_4)
	{
		TIM_SetCompare4(PWM_TIMx,cmp);
	}
	
}	


void PWM1_SetSta(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x, _Bool sta, u16 compare)
{
	if(!IS_TIM_ALL_PERIPH(PWM_TIMx))return;
	if(!IS_TIM_CHANNEL(TIM_Channel_x))return;
	
	void (*TIMx_SetComparey)(TIM_TypeDef* TIMx, uint16_t Compare);
	
	TIMx_SetComparey = TIM_SetCompare1;
	
	if(TIM_Channel_x==TIM_Channel_2)
	{
		TIMx_SetComparey = TIM_SetCompare2;
	}
	else if(TIM_Channel_x==TIM_Channel_3)
	{
		TIMx_SetComparey = TIM_SetCompare3;
	}
	else if(TIM_Channel_x==TIM_Channel_4)
	{
		TIMx_SetComparey = TIM_SetCompare4;
	}


	if (sta)
	{
		TIMx_SetComparey(PWM_TIMx, compare);
		TIM_CCxCmd(PWM_TIMx,  TIM_Channel_x,  TIM_CCx_Enable);
	}
	else
	{	
		TIM_CCxCmd(PWM_TIMx,  TIM_Channel_x,  TIM_CCx_Disable);
	}
}


