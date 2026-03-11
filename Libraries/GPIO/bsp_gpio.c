#include "bsp_gpio.h"//板级支持包board support package\
                      仅支持某一个或一批同样的开发板
					  
					  
#ifdef BSP_USE_F103
/**
  * @brief   初始化GPIO
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
			RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,  ENABLE);
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
		}
	
	}
	else if(GPIO_POTRx==GPIOA)//A13 A14 A15
	{
		if(GPIO_PINx==GPIO_Pin_13 || GPIO_PINx==GPIO_Pin_14 || GPIO_PINx==GPIO_Pin_15  )
		{
			RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,  ENABLE);
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
		}
	}
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_TMP;
	GPIO_InitStruct.GPIO_Pin = GPIO_PINx;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init( GPIO_POTRx, &GPIO_InitStruct);

}
#endif //BSP_USE_F103



#ifdef BSP_USE_F401
/**
  * @brief   初始化GPIO
  * @param   GPIO_POTRx ：需要配置的端口 
  *   @arg   GPIOx
  * @param   GPIO_PINx ：需要配置的引脚 
  *   @arg   GPIO_Pin_x
  * @param   GPIO_Mode ：需要配置的输入输出模式 
  *   @arg   GPIO_Mode_IN / GPIO_Mode_OUT / GPIO_Mode_AF / GPIO_Mode_AN
  * @param   GPIO_OType ：需要配置的输出模类型
  *   @arg   GPIO_OType_PP / GPIO_OType_OD
  * @param   GPIO_PuPd ：需要配置的上下拉类型
  *   @arg   GPIO_PuPd_NOPULL / GPIO_PuPd_UP / GPIO_PuPd_DOWN
  * @param   GPIO_Speed ：需要配置的IO速度
  *   @arg   GPIO_Low_Speed / GPIO_Medium_Speed / GPIO_Fast_Speed / GPIO_High_Speed
  *   @arg   GPIO_Speed_2MHz / GPIO_Speed_25MHz / GPIO_Speed_50MHz / GPIO_Speed_100MHz
  * @retval  
  */
void BSP_GPIO_Config(GPIO_TypeDef* GPIO_POTRx, uint16_t GPIO_PINx, GPIOMode_TypeDef GPIO_Mode, 
	GPIOOType_TypeDef GPIO_OType, GPIOPuPd_TypeDef GPIO_PuPd, GPIOSpeed_TypeDef GPIO_Speed)
{
	uint32_t RCC_AxBxPeriph_GPIOx  = RCC_AHB1Periph_GPIOA << \
										(((uint32_t)GPIO_POTRx - (uint32_t)GPIOA) / 0x0400 );
	RCC_AHB1PeriphClockCmd( RCC_AxBxPeriph_GPIOx,  ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
	GPIO_InitStruct.GPIO_Pin = GPIO_PINx;
	GPIO_InitStruct.GPIO_OType = GPIO_OType;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed;

	GPIO_Init( GPIO_POTRx, &GPIO_InitStruct);

}
#endif //BSP_USE_F401

