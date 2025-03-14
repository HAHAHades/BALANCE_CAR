

#ifndef _BSP_PWM_H_
#define _BSP_PWM_H_


#include "stm32f10x.h"

/*******注意，如果使用TIM3的CH1或CH3等的映射引脚，GPIO需要重映射,使用GPIO_PinRemapConfig()********/
//注意，C6T6没有TIM4

/****************定义pwm使用的定时器********************/
//#define PWM1_TIMx TIM3


//#define USE_TIMx_CH1 0
//#define USE_TIMx_CH2 0
//#define USE_TIMx_CH3 0
//#define USE_TIMx_CH4 0

//#if USE_TIMx_CH1
//#define PWM1_Channelx TIM_Channel_1
//#define PWM1_TIM_OCxInitFUN  TIM_OC1Init
//#define PWM1_TIM_OCxPreloadConfig TIM_OC1PreloadConfig

//#endif

//#if USE_TIMx_CH2
//#define PWM1_Channelx TIM_Channel_2
//#define PWM1_TIM_OCxInitFUN  TIM_OC2Init
//#define PWM1_TIM_OCxPreloadConfig TIM_OC2PreloadConfig

//#endif

//#if USE_TIMx_CH3
//#define PWM1_Channelx TIM_Channel_3
//#define PWM1_TIM_OCxInitFUN  TIM_OC3Init
//#define PWM1_TIM_OCxPreloadConfig TIM_OC3PreloadConfig

//#endif

//#if USE_TIMx_CH4
//#define PWM1_Channelx TIM_Channel_4
//#define PWM1_TIM_OCxInitFUN  TIM_OC4Init
//#define PWM1_TIM_OCxPreloadConfig TIM_OC4PreloadConfig

//#endif




/******************** 引脚 *********************/

//#define TIM4_CH3_PORT 				GPIOB			// TIM2_CH1_PORT GPIOA
//#define TIM4_CH3_PIN  				GPIO_Pin_8	// TIM2_CH1_PIN  GPIO_Pin_15 两个引脚任选
//#define TIM4_CH3_PORT_CLK 			RCC_APB2Periph_GPIOB  // 通道时钟
//#define TIM4_CH3_PORT_CMDFUN 		RCC_APB2PeriphClockCmd 
//#define TIM4_CH3_PORT_AF_CLK		RCC_APB2Periph_AFIO  //通道引脚复用
//#define TIM4_CH3_PORT_AF_CMDFUN 	RCC_APB2PeriphClockCmd 

//#define TIM3_CH1_PORT 				GPIOA			// TIM2_CH1_PORT GPIOA
//#define TIM3_CH1_PIN  				GPIO_Pin_6	// TIM2_CH1_PIN  GPIO_Pin_15 两个引脚任选
//#define TIM3_CH1_PORT_CLK 			RCC_APB2Periph_GPIOA  // 通道时钟
//#define TIM3_CH1_PORT_CMDFUN 		RCC_APB2PeriphClockCmd 
//#define TIM3_CH1_PORT_AF_CLK		RCC_APB2Periph_AFIO  //通道引脚复用
//#define TIM3_CH1_PORT_AF_CMDFUN 	RCC_APB2PeriphClockCmd 

//#define TIM3_CH4_PORT 				GPIOB			// TIM2_CH1_PORT GPIOA
//#define TIM3_CH4_PIN  				GPIO_Pin_1	// TIM2_CH1_PIN  GPIO_Pin_15 两个引脚任选
//#define TIM3_CH4_PORT_CLK 			RCC_APB2Periph_GPIOB  // 通道时钟
//#define TIM3_CH4_PORT_CMDFUN 		RCC_APB2PeriphClockCmd 
//#define TIM3_CH4_PORT_AF_CLK		RCC_APB2Periph_AFIO  //通道引脚复用
//#define TIM3_CH4_PORT_AF_CMDFUN 	RCC_APB2PeriphClockCmd 



///******************* 定时器 *********************/
//#define TIM4_CLK			RCC_APB1Periph_TIM4
//#define TIM4_CLKCMD_FUNC 	RCC_APB1PeriphClockCmd

//#define TIM3_CLK			RCC_APB1Periph_TIM3
//#define TIM3_CLKCMD_FUNC 	RCC_APB1PeriphClockCmd


/********************************/



/***********************************************************/
void TIMx_CHx_PWM1_Init(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x, uint8_t GPIO_Remapping ,u16 reload, u16 psc);
void TIMx_PWM1_RloadPsc_Set(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x ,u16 reload, u16 psc);
//void 		TIM3_CH1_PWM_Init		(u16 reload, u16 psc);
//void 		TIM3_CH4_PWM_Init		(u16 reload, u16 psc);
//void 		TIM4_CH3_PWM_Init		(u16 reload, u16 psc);
void PWM1_SetCompare(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x, u16 cmp);
void PWM1_SetSta(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x, _Bool sta, u16 compare);
#endif //_BSP_PWM_H_
