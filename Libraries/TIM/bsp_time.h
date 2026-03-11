

#ifndef _BSP_TIME_H_
#define _BSP_TIME_H_
#include "bsp_tim.h"

#include "stm32f10x.h"

/*******注意，如果使用TIM3的CH1或CH3等的映射引脚，GPIO需要重映射,使用GPIO_PinRemapConfig()********/
//注意，C6T6没有TIM4
//库函数SYSCLK为72M,AHB分频为1,APB1为36M分频为2,TIM2-TIM7为36*2=72M
#define TIMx_IT_ON 1 //是否使用TimX定时器中断

// #define TIMx_Delay TIM3 //需要用作任务定时的TIM；任务定时时长和任务数在.c文件里修改

/** @defgroup TIMx_DelayT 计数器中断时间对应的psc和reload
  * @{
  */

#define TIMx72M_1us_Prescaler 72-1
#define TIMx72M_1us_Period 1-1 // TIMx_DelayT = 1 / ( 72000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx72M_10us_Prescaler 72-1
#define TIMx72M_10us_Period 10-1 // TIMx_DelayT = 1 / ( 72000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx72M_100us_Prescaler 72-1
#define TIMx72M_100us_Period 100-1 // TIMx_DelayT = 1 / ( 72000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx72M_1ms_Prescaler 72-1
#define TIMx72M_1ms_Period 1000-1 // TIMx_DelayT = 1 / ( 72000000 / ( (Psc+1)*(Period+1) ) )


#define TIMx36M_1us_Prescaler 36-1
#define TIMx36M_1us_Period 1-1 // TIMx_DelayT = 1 / ( 36000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx36M_10us_Prescaler 36-1
#define TIMx36M_10us_Period 10-1 // TIMx_DelayT = 1 / ( 36000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx36M_100us_Prescaler 36-1
#define TIMx36M_100us_Period 100-1 // TIMx_DelayT = 1 / ( 36000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx36M_1ms_Prescaler 36-1
#define TIMx36M_1ms_Period 1000-1 // TIMx_DelayT = 1 / ( 36000000 / ( (Psc+1)*(Period+1) ) )

/**
  * @}
  */


#define ARRAY_LENGTH(arr) sizeof(arr)/sizeof(arr[0]) //获取数据长度


/***********************************************************/
void BSP_TIMx_TIME_Init(TIM_TypeDef * TIMx, u16 reload, u16 psc);
void TIMx_Delay_xCount(BSP_TIMx_TypeDef* BSP_TIMx_Struct, uint32_t count);
void TIMx_Get_TIMx_DelayTCount(BSP_TIMx_TypeDef* BSP_TIMx_Struct, uint32_t *re);
void TIMx_DelayT_IRQHandler(BSP_TIMx_TypeDef* BSP_TIMx_Struct);


#endif //_BSP_TIME_H_
