#ifndef _BSP_HC_SR04_H
#define _BSP_HC_SR04_H
#include "main.h"
#include "stm32f10x.h"

#include "bsp_exti.h"

#include "bsp_SysTick.h"
#include "bsp_time.h"


#define HC_SR04_TIME_BSPTIMxStruct  BSP_TIM3_Struct  //传感器使用的计时定时器(需设置好全局变量)
#define HC_SR04_TIME_CountUnitDiv10us  ((uint32_t)100)  //传感器使用的计时定时器计数器单位(/10us)


/*  输出端口高电平时长分辨率   /10us */ 
#define HC_SR04_Echo_HLevel_TimeRes 1 //1*10us


/*  IO端口 */
#define HC_SR04_Trip_PORTx GPIOA
#define HC_SR04_Trip_PINx GPIO_Pin_3

#define HC_SR04_Echo_PORTx GPIOA
#define HC_SR04_Echo_PINx GPIO_Pin_2

/* 输出中断 */
#define HC_SR04_Echo_EXTI_IRQHandler EXTI2_IRQHandler

#define HC_SR04_Echo_H_T_Div10us ((float)10000.0) //10us   / 1.7mm
#define HC_SR04_Echo_H_T_Div100us ((float)1000.0) //100us  /17mm
#define HC_SR04_Echo_H_T_Div1ms ((float)100.0) //1ms	   /170mm



void HC_SR04_Init(GPIO_TypeDef* HC_Trig_GPIO_POTRx, uint16_t HC_Trig_GPIO_PINx,
						 GPIO_TypeDef* HC_Echo_GPIO_POTRx, uint16_t HC_Echo_GPIO_PINx);
void HC_SR04_StartMeasure(void);
_Bool HC_SR04_get_distance(float *dis);
void HC_SR04_IRQHandler(void);

#endif /* _BSP_HC_SR04_H */
