#ifndef _CONTROLLER_DECODE_H
#define _CONTROLLER_DECODE_H

#include "stm32f10x.h"
#include "bsp_key.h"

/* 小车遥控器解码 */

#define NRF_CTRL_ON 1 //开启小车控制

/*******************************************************/

#define CTRL_DECODE_POT_TargetDataRatio  4.5 //电位计按键控制比，  电位计输出数值 : 实际要控制数值
#define CTRL_DECODE_POT_TurnDataRatio  0.5 //电位计按键控制比，  电位计输出数值 : 实际要控制数值


/*******************************************************/


/*函数声明*/

uint8_t CTRL_M_DECODE_CopeKeyFIFO(KEY_FIFO_t* Keys, uint8_t keys_num, uint8_t* Msg);
uint8_t CTRL_S_DECODE_CopeKeyFIFO(uint8_t* Msg);
void CTRL_S_DECODE_ADJParam(uint8_t *Msg);
void CTRL_DECODE_PackMSG(uint8_t index, uint8_t* Msg, uint8_t* Package);
#endif // _CONTROLLER_DECODE_H


