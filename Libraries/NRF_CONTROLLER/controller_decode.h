#ifndef _CONTROLLER_DECODE_H
#define _CONTROLLER_DECODE_H

#include "stm32f10x.h"
#include "bsp_key.h"

/* 小车遥控器解码 */

/*******************************************************/
#define CTRL_DECODE_SpeedIncrement 10 //小车速度单次增量(百分比)


/* 按键重定义 */
//单击/长按
#define CTRL_DECODE_Key_UP_PLP         (uint8_t)Key1_P/KEY_FIFOFLAG_NUM  // 上
#define CTRL_DECODE_Key_DOWN_PLP       (uint8_t)Key2_P/KEY_FIFOFLAG_NUM  // 下
#define CTRL_DECODE_Key_LEFT_PLP       (uint8_t)Key3_P/KEY_FIFOFLAG_NUM  // 左
#define CTRL_DECODE_Key_RIGHT_PLP      (uint8_t)Key4_P/KEY_FIFOFLAG_NUM  // 右
#define CTRL_DECODE_Key_RU_PLP         (uint8_t)Key10_P/KEY_FIFOFLAG_NUM // 右上
#define CTRL_DECODE_Key_RD_PLP         (uint8_t)Key11_P/KEY_FIFOFLAG_NUM // 右下
#define CTRL_DECODE_Key_LU_PLP         (uint8_t)Key12_P/KEY_FIFOFLAG_NUM // 左上
#define CTRL_DECODE_Key_LD_PLP         (uint8_t)Key13_P/KEY_FIFOFLAG_NUM // 左下

//单击
#define CTRL_DECODE_Key_M1M2_P       Key9_R  // 中键
//长按
#define CTRL_DECODE_Key_M1M2_LP      Key9_LP  // 中键
//双击
#define CTRL_DECODE_Key_M1M2_DP      Key9_DPR  // 中键

/*虚拟按键定义*/
#define CTRL_DECODE_Key_CONNDET             ((uint8_t)0x01) //连接检测
#define CTRL_DECODE_Key_DISTANCEDETON       ((uint8_t)0x02) //开启距离检测
#define CTRL_DECODE_Key_DISTANCEDETOFF      ((uint8_t)0x04) //关闭距离检测
#define CTRL_DECODE_Key_UPDATAWHEEL         ((uint8_t)0x08) //更新车轮速度

/*******************************************************/


/*函数声明*/
void CTRL_DECODE_CopeCmdKeyMsg(uint8_t* Keys, uint8_t keys_num);
void CTRL_DECODE_ModifyWheel(int32_t Lw, int32_t Rw);
void CTRL_DECODE_ResetWheelSpeed(void);
void CTRL_DECODE_GetWheelSpeed(int32_t *Lw, int32_t *Rw);
void CTRL_DECODE_ResetVirtualKey(void);
void CTRL_DECODE_GetVirtualKey(uint8_t *VKey);
#endif // _CONTROLLER_DECODE_H


