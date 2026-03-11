

#ifndef _BSP_PWM_H_
#define _BSP_PWM_H_

#include "bsp_tim.h"
#include "stm32f10x.h"


/*******注意，如果使用TIM3的CH1或CH3等的映射引脚，GPIO需要重映射,使用GPIO_PinRemapConfig()********/
//注意，C6T6没有TIM4

/****************定义pwm使用的定时器********************/




/***********************************************************/
void TIMx_CHx_PWM1_Init(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x, uint8_t GPIO_Remapping ,u16 reload, u16 psc);
void TIMx_PWM1_RloadPsc_Set(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x ,u16 reload, u16 psc);

void TIMx_CHx_PWM_Init(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x, uint32_t GPIO_Remapping ,u16 reload, u16 psc);
void TIMx_CHx_PWM_IO_Config(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x, uint32_t GPIO_Remapping);
void TIMx_CHx_PWM_TIMEBASE_Config(TIM_TypeDef * PWM_TIMx, u16 reload, u16 psc);
void TIMx_CHx_PWM_OCx_Config(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x);
void PWM_SetCompare(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x, u16 cmp);
void PWM_SetSta(TIM_TypeDef * PWM_TIMx, uint16_t TIM_Channel_x, _Bool sta);
#endif //_BSP_PWM_H_
