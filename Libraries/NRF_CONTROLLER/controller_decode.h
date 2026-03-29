#ifndef _CONTROLLER_DECODE_H
#define _CONTROLLER_DECODE_H

#include "stm32f10x.h"
#include "bsp_key.h"

/* 小车遥控器解码 */

#define NRF_CTRL_ON 1

/*******************************************************/
#define CTRL_DECODE_SpeedIncrement 10 //小车速度单次增量(百分比)
#define CTRL_DECODE_DIRSpeedLimit  10 //转弯限速倍数(速度=原速/CTRL_DECODE_DIRSpeedLimit)
#define CTRL_DECODE_SpeedLimit     3 //直行限速倍数(速度=原速/CTRL_DECODE_SpeedLimit)

#define CTRL_DECODE_POTDataRatio  16 //电位计按键控制比，  电位计输出数值 : 实际要控制数值


/* 按键重定义 */
//单击/长按
#if NRF_CTRL_USE_KEYIOSCAN
#define CTRL_DECODE_Key_UP_PLP         (uint8_t)Key1_P/KEY_FIFOFLAG_NUM  // 上
#define CTRL_DECODE_Key_DOWN_PLP       (uint8_t)Key2_P/KEY_FIFOFLAG_NUM  // 下
#define CTRL_DECODE_Key_LEFT_PLP       (uint8_t)Key3_P/KEY_FIFOFLAG_NUM  // 左
#define CTRL_DECODE_Key_RIGHT_PLP      (uint8_t)Key4_P/KEY_FIFOFLAG_NUM  // 右
#define CTRL_DECODE_Key_RU_PLP         (uint8_t)Key10_P/KEY_FIFOFLAG_NUM // 右上
#define CTRL_DECODE_Key_RD_PLP         (uint8_t)Key11_P/KEY_FIFOFLAG_NUM // 右下
#define CTRL_DECODE_Key_LU_PLP         (uint8_t)Key12_P/KEY_FIFOFLAG_NUM // 左上
#define CTRL_DECODE_Key_LD_PLP         (uint8_t)Key13_P/KEY_FIFOFLAG_NUM // 左下
#endif //NRF_CTRL_USE_KEYIOSCAN

#if NRF_CTRL_USE_ADC
#define CTRL_DECODE_KeyPot1_LP ((uint8_t)Key4_LP/KEY_FIFOFLAG_NUM) //左侧左右
#define CTRL_DECODE_KeyPot2_LP ((uint8_t)Key5_LP/KEY_FIFOFLAG_NUM) //左侧上下
#define CTRL_DECODE_KeyPot3_LP ((uint8_t)Key6_LP/KEY_FIFOFLAG_NUM) //右侧左右
#define CTRL_DECODE_KeyPot4_LP ((uint8_t)Key7_LP/KEY_FIFOFLAG_NUM) //右侧上下

#endif //NRF_CTRL_USE_ADC

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



uint8_t CTRL_M_DECODE_CopeKeyFIFO(KEY_FIFO_t* Keys, uint8_t keys_num, uint8_t* Msg);
uint8_t CTRL_S_DECODE_CopeKeyFIFO(uint8_t* Msg);
void CTRL_DECODE_PackMSG(uint8_t index, uint8_t* Msg, uint8_t* Package);
void CTRL_DECODE_CopeCmdKeyMsg(uint8_t* Keys, uint8_t keys_num);
void CTRL_DECODE_ModifyWheel(int32_t Lw, int32_t Rw);
void CTRL_DECODE_ResetWheelSpeed(void);
void CTRL_DECODE_GetWheelSpeed(int32_t *Lw, int32_t *Rw);
void CTRL_DECODE_ResetVirtualKey(void);
void CTRL_DECODE_GetVirtualKey(uint8_t *VKey);
#endif // _CONTROLLER_DECODE_H


