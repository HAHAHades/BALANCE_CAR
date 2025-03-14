

#ifndef _BSP_TIM_H_
#define _BSP_TIM_H_


#include "stm32f10x.h"

/*******注意，如果使用TIM3的CH1或CH3等的映射引脚，GPIO需要重映射,使用GPIO_PinRemapConfig()********/
//注意，C6T6没有TIM4



#define TIMx_Delay TIM3 //需要用作任务定时的TIM；任务定时时长和任务数在.c文件里修改



#define TIMx_1us_Prescaler 72-1
#define TIMx_1us_Period 1-1 // DelayT = 1 / ( 72000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx_10us_Prescaler 72-1
#define TIMx_10us_Period 10-1 // DelayT = 1 / ( 72000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx_100us_Prescaler 72-1
#define TIMx_100us_Period 100-1 // DelayT = 1 / ( 72000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx_1ms_Prescaler 72-1
#define TIMx_1ms_Period 1000-1 // DelayT = 1 / ( 72000000 / ( (Psc+1)*(Period+1) ) )

#define ARRAY_LENGTH(arr) sizeof(arr)/sizeof(arr[0]) //获取数据长度


/***********************************************************/
void TIMx_Delay_Register(TIM_TypeDef * TIMx, u16 reload, u16 psc);
void TIMx_Delay_xCount(uint32_t count);
void TIMx_10us_IRQHandler(void);
void TIMx_Get_10usTicCount(uint32_t* re);

#endif //_BSP_TIM_H_
