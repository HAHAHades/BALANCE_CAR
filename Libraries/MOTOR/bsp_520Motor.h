#ifndef _BSP_520MOTOR_H
#define _BSP_520MOTOR_H

#include "stm32f10x.h"






/********************电机正反转输入端口**********************/
/*
IN1		0		0		1
IN2		0		1		0
Motor	停止	正转	反转
*/
//电机A
#define Motor_520_AIN1_PORTx  	GPIOB 
#define Motor_520_AIN1_Pinx 	GPIO_Pin_13
#define Motor_520_AIN2_PORTx  	GPIOB
#define Motor_520_AIN2_Pinx 	GPIO_Pin_12

//电机B
#define Motor_520_BIN1_PORTx  	GPIOB 
#define Motor_520_BIN1_Pinx 	GPIO_Pin_14
#define Motor_520_BIN2_PORTx  	GPIOB
#define Motor_520_BIN2_Pinx 	GPIO_Pin_15


/********************电机PWM控制定时器**********************/
//电机A
#define Motor_520_APWM_TIMx  			TIM1 
#define Motor_520_APWM_TIM_Channel_x 	TIM_Channel_4 

//电机B
#define Motor_520_BPWM_TIMx  			TIM1 
#define Motor_520_BPWM_TIM_Channel_x 	TIM_Channel_1

/************************电机PWM频率*************************/
#define Motor_520_PWM_1KHz_Prescaler  	TIMx72M_1k_Prescaler  // 定时器计数频率f =  72000000 / (Psc+1) 
#define Motor_520_PWM_1KHz_Period		TIMx72M_1k_Period	// pwm 频率f = 72000000 / ((Psc+1) *(Period+1))

#define Motor_520_PWM_10KHz_Prescaler	TIMx72M_10k_Prescaler
#define Motor_520_PWM_10KHz_Period		TIMx72M_10k_Period



					
/*********************电机*************************/				
#define Motor_520_A 0
#define Motor_520_B 1	

#define Motor_520_ForwardRotation 0
#define Motor_520_ReverseRotation 1



/***************函数申明***************/

void BSP_520Motor_Config(TIM_TypeDef * PWM_TIMx, uint16_t PWM_TIM_Channel_x, 
						GPIO_TypeDef* IN1_GPIO_PORTx, uint16_t IN1_GPIO_Pinx, 
						GPIO_TypeDef* IN2_GPIO_PORTx, uint16_t IN2_GPIO_Pinx);
void BSP_520Motor_Rotation(uint8_t Motor_Num, _Bool Dir, float Speed);
void BSP_520Motor_Stop(uint8_t Motor_Num);


/*************************************/


#endif //_BSP_520MOTOR_H



