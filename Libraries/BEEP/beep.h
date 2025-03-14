#ifndef _BEEP_H_
#define _BEEP_H_


#include "stm32f10x.h"

#include "bsp_pwm.h" //无源蜂鸣器使用pwm驱动


/*************定义BEEP使用的TIM和CH 及IO映射***************/
#define BEEP_PWM_TIMx TIM2
#define BEEP_PWM_TIMx_CHx TIM_Channel_2
#define BEEP_PWM_TIMx_CHx_Remap 0x01


/********************************/





/********************************/





/********************************/


#define BEEP_ON(a)		PWM1_SetSta(BEEP_PWM_TIMx, BEEP_PWM_TIMx_CHx, 1,a)
#define BEEP_OFF(a)		PWM1_SetSta(BEEP_PWM_TIMx, BEEP_PWM_TIMx_CHx, 0,a)


/*********************************************/




						
/*******************************************************************************/






#endif //_BEEP_H_

