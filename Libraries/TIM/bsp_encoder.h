

#ifndef _BSP_ENCODER_H_
#define _BSP_ENCODER_H_

#include "bsp_tim.h"
#include "stm32f10x.h"



/*********************定义编码器使用的定时器及通道************************/

#define ENCODER_GET_COUNT_IN_IT 0 //是否在中断中读取编码器脉冲数


#define EncoderA_TIMx TIM2
#define EncoderA_TIMx_ACH_x TIM_Channel_1
#define EncoderA_TIMx_BCH_x TIM_Channel_2
#define EncoderA_TIMx_IO_Reamp TIM2_IO_Reamp0


#define EncoderB_TIMx TIM4
#define EncoderB_TIMx_ACH_x TIM_Channel_1
#define EncoderB_TIMx_BCH_x TIM_Channel_2
#define EncoderB_TIMx_IO_Reamp TIM4_IO_Reamp0

#define Encoder_Count_Period 1000-1 //编码器计数周期 (Encoder_Count_Period+1)*Delay_T(bsp_tim.h)
#define Encoder_Count_GTask_DelayFlag_List_at 1 //编码器计数标识在任务状态标识列表中的位置


#define BSP_Encoder_DirF  0  //AB 正向
#define BSP_Encoder_DirB  1  //BA 反向


/*****************************************************************************/

void TIMx_CHx_ENCODER_Init(TIM_TypeDef * ENCODER_TIMx, uint16_t TIM_Channel_x1,
						uint32_t GPIO_Remapping , _Bool Dir);
int16_t Get_Encoder_Count(TIM_TypeDef* TIMx);


#endif //_BSP_ENCODER_H_
