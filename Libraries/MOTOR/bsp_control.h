#ifndef _BSP_CONTROL_H
#define _BSP_CONTROL_H

#include "stm32f10x.h"

#include "mpu6050.h"


extern int32_t G_BSPCTRL_TargetSpeed ;//????
extern int32_t G_BSPCTRL_TurnSpeed ;//??????????????




					




/***************º¯ÊýÉêÃ÷***************/


int Vertical_Loop(float tatget_angle, float real_angle, float gyro_Y);
float Velocity_Loop(int tatget_speed, int real_speed);
int Turn_Loop(int32_t turn_speed ,float gyro_Z);
uint8_t pwm_limit_abs(int* data);
void Control_PWM(float tatget_angle, float real_angle, float gyro_Y, int32_t tatget_speed, int encoder_L, int encoder_R, int32_t target_turn, float gyro_Z);
void Control_Car_IRQHandler(void);




/*************************************/


#endif //_BSP_CONTROL_H



