#ifndef _BSP_CAN_H
#define _BSP_CAN_H

#include "stm32f10x.h"



#define CAN1_GPIO_Remapping 0x10 //CAN1 引脚重映射， 0x00:Rx->A11,Tx->A12    0x10:Rx->B8,Tx->B9

#define CAN_PASS_ID 0x1314 //定义需要的消息ID


					
/***************函数申明***************/

void CAN_Config(CAN_TypeDef* CANx, uint8_t CAN_Mode);



/*************************************/


#endif //_BSP_CAN_H



