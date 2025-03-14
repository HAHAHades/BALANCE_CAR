#ifndef _BSP_CONTROL_H
#define _BSP_CONTROL_H

#include "stm32f10x.h"







					




/***************º¯ÊýÉêÃ÷***************/


int Vertical_Loop(float tatget_angle, float real_angle, float gyro_Y);
float Velocity_Loop(int tatget_speed, int real_speed);
int Turn_Loop(float gyro_Z);
uint8_t pwm_limit_abs(int* data);
void Control_PWM(float tatget_angle, float real_angle, float gyro_Y, int tatget_speed, int encoder_L, int encoder_R, float gyro_Z);





/*************************************/


#endif //_BSP_CONTROL_H



