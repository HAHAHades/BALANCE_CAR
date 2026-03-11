#ifndef __MAIN_H
#define __MAIN_H



#define BSP_USE_F103  //使用F103系列
//#define BSP_USE_F401  //使用F401系列


#define USART_Print_ON  1
#define OLED_Display_ON  1
#define OLED_SHOW_MPU 0 //OLED 显示mpu的信息

#define OLED_SHOW_HC_SR04 0 //OLED 显示声波传感器的信息
#define OLED_SHOW_Encoder_Speed 0 //OLED 显示编码器信息
#define HC_SR04_ON 0 //是否开启声波测距
#define Motor_DEBUG 0 //调试电机
#define CONTROL_CAR_IN_IT  0 // 是否在中断中控制小车，在主函数里控制小车无法同时做其他事情，如刷新OLED等
#define BANLANCE_CAR_ON 1 //平衡车测试
#define NRF_CTRL_ON 0 //是否使用NRF遥控器
#define TIMx_TIME_ON 0
#define TIMx_ENCODER_ON 0
#define MPU_ON 1
#define MPU_GetEuler_IN_IT  0  //在中断中读取MPU数据
#define USART_DEBUG_MPU 0 //USART 输出mpu数据
#define USART_Print_IN_IT_ON 0 //USART 在中断中输出，(中断频率过高可能导致系统卡死)

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"

#ifdef BSP_USE_F103
#include "stm32f10x.h"
#endif //BSP_USE_F103

#ifdef BSP_USE_F401
#include "stm32f4xx.h"
#endif //BSP_USE_F401

#include "bsp_SysTick.h"
#include "bsp_gpio.h"
#include "bsp_led.h"

/********************* */
#if USART_Print_ON
#include "bsp_usart.h"
#endif //USART_Print_ON

/******************** */
#if OLED_Display_ON
#include "bsp_iic.h"
#include "oled.h"
#endif //OLED_Display_ON

/********************* */
#if HC_SR04_ON
#include "bsp_hc_sr04.h"
#endif //HC_SR04_ON

/******************** */
#if TIMx_TIME_ON
#include "bsp_time.h"
#define BSP_TIM3_TIME_ON 1

#if BSP_TIM3_TIME_ON
extern BSP_TIMx_TypeDef BSP_TIM3_Struct;
#endif //BSP_TIM3_TIME_ON

#endif //TIMx_TIME_ON

/**************** */
#if TIMx_ENCODER_ON
#include "bsp_encoder.h"
#endif //TIMx_ENCODER_ON

/****************** */
#if BANLANCE_CAR_ON
#include "bsp_520Motor.h"
#include "bsp_control.h"
#include "bsp_encoder.h"
#endif //BANLANCE_CAR_ON

/******************* */
#if NRF_CTRL_ON
#include "nrf_controller.h"
#endif //NRF_CTRL_ON

/********************* */
#if MPU_ON
#include "mpu6050.h"
extern float  G_Euler_RPY[3];//欧拉角
extern float  G_GYRO_XYZ[3];//角速度
extern float  G_ACCEL_XYZ[3];//加速度
#endif //MPU_ON

/****************** */
#ifdef   soft_IIC //MPU
#include "bsp_soft_i2c.h"
#else
#include "bsp_hard_i2c.h"
#endif


#endif /* __MAIN_H */
