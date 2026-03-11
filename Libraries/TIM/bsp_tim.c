
#include "bsp_tim.h"
#include "bsp_time.h"
#include "bsp_pwm.h"



void BSP_TIMx_UsageDemo(void)
{
	BSP_TIMx_TypeDef BSP_TIM3_Struct;


	BSP_TIM3_Struct.TIMx = TIM3;
	BSP_TIM3_Struct.WorkMode = BSP_TIMx_TIME;
	BSP_TIM3_Struct.TIMx_Prescaler = TIMx72M_1ms_Prescaler;
	BSP_TIM3_Struct.TIMx_Period = TIMx72M_1ms_Period;
	BSP_TIMx_Init(&BSP_TIM3_Struct);

	TIMx_Delay_xCount(&BSP_TIM3_Struct, 10);


	

	// BSP_TIM3_Struct.TIM_TimeBaseInitStruct->TIM_ClockDivision = TIM_CKD_DIV1;
	// BSP_TIM3_Struct.TIM_TimeBaseInitStruct->TIM_CounterMode = TIM_CounterMode_Up;
	// BSP_TIM3_Struct.TIM_TimeBaseInitStruct->TIM_Period = 1000-1;// 自动重装载值
	// BSP_TIM3_Struct.TIM_TimeBaseInitStruct->TIM_Prescaler =   (72-1);// 分频器值
	// // BSP_TIM3_Struct.TIM_TimeBaseInitStruct->TIM_Period = BSP_TIM3_Struct.TIMx_Period;// 自动重装载值
	// // BSP_TIM3_Struct.TIM_TimeBaseInitStruct->TIM_Prescaler =   BSP_TIM3_Struct.TIMx_Prescaler;// 分频器值
	// BSP_TIM3_Struct.TIM_TimeBaseInitStruct->TIM_RepetitionCounter =0;// 未用

	// BSP_TIM3_Struct.TIM_Channel_x = TIM_Channel_1;
	
	// BSP_TIM3_Struct.TIM_OCInitStruct-> = ;

	// BSP_TIM3_Struct.TIM_OCInitStruct->TIM_OCMode = TIM_OCMode_PWM1;
	// BSP_TIM3_Struct.TIM_OCInitStruct->TIM_OCPolarity = TIM_OCPolarity_High;//输出极性,当定时器计数值小于CCR1_Val时为高电平
	// BSP_TIM3_Struct.TIM_OCInitStruct->TIM_OutputState = TIM_OutputState_Enable;//使能
	// BSP_TIM3_Struct.TIM_OCInitStruct->TIM_Pulse = 0; //设置初始PWM比较脉冲宽度为0	



}



uint8_t BSP_TIMx_Init(BSP_TIMx_TypeDef* BSP_TIMx_struct)
{

	uint8_t rVal = TIMx_RET_Flag_OK;

	if (BSP_TIMx_struct->WorkMode==BSP_TIMx_TIME)
	{
		
		BSP_TIMx_TIME_Init(BSP_TIMx_struct->TIMx, BSP_TIMx_struct->TIMx_Prescaler, BSP_TIMx_struct->TIMx_Period);

	}
	else if (BSP_TIMx_struct->WorkMode==BSP_TIMx_PWM)
	{
		 

		// TIMx_CHx_PWM_Init(BSP_TIMx_struct->TIMx,  BSP_TIMx_struct->TIM_Channel_x,  BSP_TIMx_struct->TIM_GPIO_Remap ,
		// 	BSP_TIMx_struct->TIMx_Period, BSP_TIMx_struct->TIMx_Prescaler);

	}
	else if (BSP_TIMx_struct->WorkMode==BSP_TIMx_ENCODER)
	{
		
	}
	else
	{
		rVal = TIMx_RET_Flag_Init_Error;
	}
	



	return rVal;
}


#if 0
uint8_t BSP_TIMx_Init(BSP_TIMx_TypeDef* BSP_TIMx_struct)
{
	uint8_t rVal = TIMx_RET_Flag_OK;

	TIM_Cmd(BSP_TIMx_struct->TIMx,DISABLE); //关闭定时器

	void (*TIMx_RCC_APBxPeriphClockCmd)(uint32_t RCC_APBxPeriph, FunctionalState NewState);
	uint32_t RCC_APBxPeriphTIMx;



	if (BSP_TIMx_struct->TIMx==TIM1)//TIM2 的 CH1234 分别在 A0 A1 A2 A3 
	{
		RCC_APBxPeriphTIMx = RCC_APB2Periph_TIM1;
		TIMx_RCC_APBxPeriphClockCmd = RCC_APB2PeriphClockCmd;
		
		NVIC_IRQChannel = TIM1_UP_IRQn;
	}
	else if (BSP_TIMx_struct->TIMx==TIM2)//TIM2 的 CH1234 分别在 A0 A1 A2 A3 
	{	
		RCC_APBxPeriphTIMx = RCC_APB1Periph_TIM2;
		TIMx_RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		
		NVIC_IRQChannel = TIM2_IRQn;
	}
	else if (BSP_TIMx_struct->TIMx==TIM3)//TIM3 的 CH1234 分别在 A6 A7 B0 B1
	{	
		RCC_APBxPeriphTIMx = RCC_APB1Periph_TIM3;
		TIMx_RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		
		NVIC_IRQChannel = TIM3_IRQn;
	}	
	else if (BSP_TIMx_struct->TIMx==TIM4)//TIM4 的 CH1234 分别在 B6 B7 B8 B9
	{
		RCC_APBxPeriphTIMx = RCC_APB1Periph_TIM4;
		TIMx_RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		
		NVIC_IRQChannel = TIM4_IRQn;
	}
	else
	{
		rVal = TIMx_RET_Flag_Init_Error;
	}
	
	TIMx_RCC_APBxPeriphClockCmd(RCC_APBxPeriphTIMx, ENABLE);//开启TIMx时钟

	return rVal;
}

#endif //if 0

