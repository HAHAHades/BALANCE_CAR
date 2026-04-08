#ifndef _BSP_HC_SR04_H
#define _BSP_HC_SR04_H

#include "stm32f10x.h"
#include "bsp_tim.h"



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


typedef struct 
{
	GPIO_TypeDef* Trig_PORT;
	uint16_t Trig_PIN;
	GPIO_TypeDef* Echo_PORT;
	uint16_t Echo_PIN;
	uint32_t Echo_EXTI_Line;
	BSP_TIMx_TypeDef Time_TIM_Struct;//使用的定时器对象
	uint8_t Echo_EXTI_Falg;
	uint32_t usResult;//转换的结果/us
}HC_SR04_Typedef;


#define HC_SR04_Typedef_DefaultVal {\
									.Trig_PORT = GPIOA,\
									.Trig_PIN = GPIO_Pin_3,\
									.Echo_PORT = GPIOA,\
									.Echo_PIN = GPIO_Pin_2,\
									.Echo_EXTI_Line = GPIO_Pin_2,\
									.Time_TIM_Struct.TIMx = TIM3}




void HC_SR04_Init(HC_SR04_Typedef* HC_SR04_Struct);
void HC_SR04_StartMeasure(HC_SR04_Typedef* HC_SR04_Struct);
uint8_t HC_SR04_get_distance(HC_SR04_Typedef* HC_SR04_Struct, float *dis);
void HC_SR04_IRQHandler(HC_SR04_Typedef* HC_SR04_Struct);

#endif /* _BSP_HC_SR04_H */
