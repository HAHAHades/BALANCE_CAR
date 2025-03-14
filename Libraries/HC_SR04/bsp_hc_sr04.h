#ifndef _BSP_HC_SR04_H
#define _BSP_HC_SR04_H

#include "stm32f10x.h"

#include "bsp_exti.h"

#include "bsp_SysTick.h"



/*  输出端口高电平时长分辨率   *10us */ 
#define HC_SR04_Echo_HLevel_TimeRes 1 //1*10us


/*  IO端口 */
#define HC_SR04_Trip_PORTx GPIOA
#define HC_SR04_Trip_PINx GPIO_Pin_3

#define HC_SR04_Echo_PORTx GPIOA
#define HC_SR04_Echo_PINx GPIO_Pin_2

/* 输出中断 */
#define HC_SR04_Echo_EXTI_IRQHandler EXTI2_IRQHandler






void HC_SR04_Init(GPIO_TypeDef* HC_Trig_GPIO_POTRx, uint16_t HC_Trig_GPIO_PINx,
						 GPIO_TypeDef* HC_Echo_GPIO_POTRx, uint16_t HC_Echo_GPIO_PINx);
void HC_SR04_StartMeasure(void);
_Bool HC_SR04_get_distance(float *dis);
void HC_SR04_IRQHandler(void);

#endif /* _BSP_HC_SR04_H */
