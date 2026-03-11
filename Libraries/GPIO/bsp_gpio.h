#ifndef _BSP_GPIO_H
#define _BSP_GPIO_H

#include "main.h"


#ifdef BSP_USE_F103
#include "stm32f10x.h"
#endif //BSP_USE_F103

#ifdef BSP_USE_F401
#include "stm32f4xx.h"
#endif //BSP_USE_F401



					
/***************º¯ÊýÉêÃ÷***************/
#ifdef BSP_USE_F103
void BSP_GPIO_Config(GPIO_TypeDef* GPIO_POTRx, uint16_t GPIO_PINx, GPIOMode_TypeDef GPIO_Mode);
#endif //BSP_USE_F103



#ifdef BSP_USE_F401
void BSP_GPIO_Config(GPIO_TypeDef* GPIO_POTRx, uint16_t GPIO_PINx, GPIOMode_TypeDef GPIO_Mode, 
	GPIOOType_TypeDef GPIO_OType, GPIOPuPd_TypeDef GPIO_PuPd, GPIOSpeed_TypeDef GPIO_Speed);
#endif //BSP_USE_F401

/*************************************/


#endif //_BSP_GPIO_H



