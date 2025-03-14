#ifndef _BSP_LED_H
#define _BSP_LED_H

#include "stm32f10x.h"



# define MAX_LED_NUM 5  //最多几个led  LED从1开始编号

# define LED_Trigger_Level_Low 0  //定义触发电平
# define LED_Trigger_Level_High 1  //定义触发电平


					
/***************函数申明***************/
void LED_Register(uint8_t LED_num, GPIO_TypeDef* LED_GPIO_POTRx, uint16_t LED_GPIO_PINx, _Bool rigger_Level);
void LED_NEW_STA(uint8_t LED_num, _Bool newsta);
void LED_ON(uint8_t LED_num);
void LED_OFF(uint8_t LED_num);
/********************************/


#endif //_BSP_LED_H



