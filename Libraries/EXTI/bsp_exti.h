#ifndef _BSP_EXTI_H
#define _BSP_EXTI_H

#include "stm32f10x.h"








void KEY_EXTI_Register(GPIO_TypeDef* KEY_GPIO_POTRx, uint16_t KEY_GPIO_PINx, 
						EXTIMode_TypeDef EXTI_Mode, EXTITrigger_TypeDef EXTI_Trigger);



#endif /* _BSP_EXTI_H */
