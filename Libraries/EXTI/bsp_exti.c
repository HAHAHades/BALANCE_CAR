
#include "bsp_exti.h"


/**
  * @brief  按键外部中断配置
  * @param   

  *		@arg KEY_GPIO_POTRx ：产生外部中断的IO端口
  *		@arg KEY_GPIO_PINx ：产生外部中断的IO引脚
  *		@arg EXTI_Mode ：中断/事件   EXTI_Mode_Interrupt/EXTI_Mode_Event
  *		@arg EXTI_Trigger ：触发方式   EXTI_Trigger_Rising/EXTI_Trigger_Falling/EXTI_Trigger_Rising_Falling
  * @retval  
  */
void KEY_EXTI_Register(GPIO_TypeDef* KEY_GPIO_POTRx, uint16_t KEY_GPIO_PINx, 
						EXTIMode_TypeDef EXTI_Mode, EXTITrigger_TypeDef EXTI_Trigger)
{
	
	EXTI_InitTypeDef EXTI_InitStruct;
	
	uint32_t EXTI_Line = KEY_GPIO_PINx;
	
	// 0x00000004 << x
	uint8_t tmpGPIOx_count = ((uint32_t)KEY_GPIO_POTRx - (uint32_t)GPIOA) / 0x0400;
	uint32_t KEY_RCC_APB2Periph_GPIOx  = RCC_APB2Periph_GPIOA << tmpGPIOx_count ;
	
	/*********************GPIO Config************************/
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(KEY_RCC_APB2Periph_GPIOx, ENABLE);//打开外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//打开外设时钟， EXTI在AFIO上
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin =KEY_GPIO_PINx;
	
	GPIO_Init(KEY_GPIO_POTRx, &GPIO_InitStruct);//初始化LED灯对应的端口，将配置\
                                              的模式、速度等写入寄存器中
	uint8_t GPIO_PortSourceGPIOx = tmpGPIOx_count;
	uint8_t GPIO_PinSourcex = 0;
	uint16_t tmp_GPIO_Pinx = KEY_GPIO_PINx;
	while(tmp_GPIO_Pinx)
	{
		tmp_GPIO_Pinx>>=1;
		GPIO_PinSourcex++;
	}
	GPIO_PinSourcex--;
	
	GPIO_EXTILineConfig( GPIO_PortSourceGPIOx,  GPIO_PinSourcex);
	
	/********************************************************/
	
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger;
	
	
	EXTI_Init(&EXTI_InitStruct);
	
	/*********************NVIC Config**************************/

	NVIC_InitTypeDef NVIC_InitStruct;
	uint8_t NVIC_IRQChannel;
	if(GPIO_PinSourcex>9)
	{
		NVIC_IRQChannel = EXTI15_10_IRQn;
	
	}
	else  if(GPIO_PinSourcex>4)
	{
		NVIC_IRQChannel = EXTI9_5_IRQn;
	}
	else
	{
		NVIC_IRQChannel = EXTI0_IRQn + GPIO_PinSourcex;
	}
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//选择中断优先级分组
	
	NVIC_InitStruct.NVIC_IRQChannel = NVIC_IRQChannel;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStruct);
	
	

}













