#include "bsp_gpio.h"//板级支持包board support package\
                      仅支持某一个或一批同样的开发板
					  
					  



/**
  * @brief   初始化CAN
  * @param   
  *		@arg GPIO_POTRx ：需要配置的端口
  *		@arg GPIO_PINx ：需要配置的引脚 GPIO_Pin_x
  *		@arg GPIO_Mode ：需要配置的输入输出模式
  * @retval  
  */


void BSP_GPIO_Config(GPIO_TypeDef* GPIO_POTRx, uint16_t GPIO_PINx, GPIOMode_TypeDef GPIO_Mode)
{

	GPIOMode_TypeDef GPIO_Mode_TMP = GPIO_Mode;
	
	uint32_t RCC_APB2Periph_GPIOx  = RCC_APB2Periph_GPIOA << \
										(((uint32_t)GPIO_POTRx - (uint32_t)GPIOA) / 0x0400 );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOx,  ENABLE);
	
	
	if(GPIO_POTRx==GPIOB)//B3 B4
	{
		if(GPIO_PINx==GPIO_Pin_3 || GPIO_PINx==GPIO_Pin_4)
		{
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
			RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,  ENABLE);
		}
	
	}
	else if(GPIO_POTRx==GPIOA)//A13 A14 A15
	{
		if(GPIO_PINx==GPIO_Pin_13 || GPIO_PINx==GPIO_Pin_14 || GPIO_PINx==GPIO_Pin_15  )
		{
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
			RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,  ENABLE);
		}
	}
	
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_TMP;
	GPIO_InitStruct.GPIO_Pin = GPIO_PINx;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init( GPIO_POTRx, &GPIO_InitStruct);



}

