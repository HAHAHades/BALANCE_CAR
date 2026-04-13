#ifndef _NRF_CONTROLLER_H
#define _NRF_CONTROLLER_H

#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "nrf24l01p.h"
#include "controller_decode.h"

/*基于NRF的遥控器*/

/*******************************User Modified*******************************/
#define NRF_CTRL_DEBUG_ON       0 //是否通过串口输出调试信息
#define NRF_CTRL_TICK_PERIOD  1  //定义心跳周期/ms
#define NRF_CTRL_TICKDET_TIMEOUT 2000 //定义心跳检测超时时间/ms
/****************************End Of User Modified****************************/

/********************模式定义************************/ 
#define NRF_CTRL_MASTER         1 //主控端
#define NRF_CTRL_SLAVER         0 //被控端
#define NRF_CTRL_MODE_TX        ((uint8_t)0x01) //发送方，遥控端
#define NRF_CTRL_MODE_RX        ((uint8_t)0x00) //接收方，被控端
/**********************************************/

/********************指令定义************************/ 
#define NRF_CTRL_CMD_KEY          ((uint8_t)0x01)// 收到/发出了按键值
#define NRF_CTRL_CMD_DATA         ((uint8_t)0x02)// 收到/发出了数据
#define NRF_CTRL_CMD_REQ          ((uint8_t)0x04)// 向对方请求数据
#define NRF_CTRL_CMD_REQ_RES      ((uint8_t)0x08)// 回复对方的请求
#define NRF_CTRL_CMD_REQ_REFUSE   ((uint8_t)0x10)// 拒绝对方的请求
#define NRF_CTRL_CMD_CAHR         ((uint8_t)0x11)// 收到/发出了字符串
#define NRF_CTRL_CMD_ADJP         ((uint8_t)0xAD)// 调参，调参时会忽略其他指令
#define NRF_CTRL_CMD_CONNDET      ((uint8_t)0xCD)// 连接检测
#define NRF_CTRL_CMD_DUMMY        ((uint8_t)0x00)// 空指令
/**********************************************/

/**************************按键定义**********************/
#define NRF_CTRL_KEY_DT_ON      ((uint8_t)0x01) // 开启距离检测
#define NRF_CTRL_KEY_DT_OFF     ((uint8_t)0x02) // 关闭距离检测
/***************************************************/

/********************************请求定义***********************************/
#define NRF_CTRL_REQ_DeviceInfo         ((uint8_t)0x01)// 设备信息
#define NRF_CTRL_REQ_BatteryLevel       ((uint8_t)0x02)// 电池电量/电压
#define NRF_CTRL_REQ_RunTime            ((uint8_t)0x04)// 运行时间
#define NRF_CTRL_REQ_WheelSpeed         ((uint8_t)0x08)// 车轮速度（主控方：回复请求控制车速，发送请求获取车速；被控方：回复请求发出车速）
#define NRF_CTRL_REQ_DUMMY              ((uint8_t)0x00)// 空请求
/**************************************************************************/

#define NRF_CTRL_MsgBytes 5 //每组数据大小
#define NRF_CTRL_MAXMsgPerPackage 6 //每包数据最多有几组
#define NRF_CTRL_MsgTimeOut 10 //数据读写超时时间

#define NRF_CTRL_Key_TickTask_Period  10 //扫描按键状态周期
#define NRF_CTRL_GetFIFO_Period  200 //获取按键FIFO周期
#define NRT_CTRL_TRMsg_Period  200 //发送/接收MSG周期


//遥控器执行周期参数结构体
typedef struct 
{
    uint32_t NRFCTRL_LastGetFIFOTime ;
    uint32_t NRFCTRL_LastTRMSGTime ;
    uint32_t Key_TickTask_LastRunTime ;
}NRF_Controller_Run_Typedef;

#define NRF_Controller_Run_TypedefDefaultVal {\
                                                .NRFCTRL_LastGetFIFOTime = 0,\
                                                .NRFCTRL_LastTRMSGTime = 0,\
                                                .Key_TickTask_LastRunTime = 0}




/** @defgroup NRF_CTRL_STA_FLAG 连接器状态标志位
  * @{
  */
#define NRF_CTRL_STA_CONNECTED          ((uint32_t)0x00000001)//设备已连接

/**
  * @}
  */

/*信息输出*/
#define NRF_CTRL_INFO(fmt,arg...)           printf("<<-NRF_CTRL-INFO->> "fmt"\n",##arg)
#define NRF_CTRL_ERROR(fmt,arg...)          printf("<<-NRF_CTRL-ERROR->> "fmt"\n",##arg)


#define NRF_CTRL_DEBUG(fmt,arg...)          do{\
                                          if(NRF_CTRL_DEBUG_ON)\
                                          printf("<<-NRF_CTRL-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)                                       

/************************函数声明********************/
void NRF_Controller_UsageDemo(void);
uint8_t NRF_Controller_Config(uint8_t mode);
void NRF_Controller_Run(NRF_Controller_Run_Typedef* RunStruct);
void NRF_Controller_WaitForConnect(void);
uint32_t NRFCTR_GetTime(void);
uint8_t NRF_CTRL_FIFO2MSG(KEY_FIFO_t* Keys, uint8_t keys_num, uint8_t* Msg);
void NRF_Controller_RunOnce(void);

uint8_t NRF_CTRL_ConnectionDetect(void);
void NRF_CTRL_TickIncrease(void);
void NRF_CTRL_TickDetect(uint32_t tic);
uint8_t NRF_CTRL_SendTick(void);
uint8_t NRF_CTRL_SendMsg(uint8_t* TXMsg, uint8_t* RXMsg);
uint8_t NRF_CTRL_RecvMsg(uint8_t* TXMsg, uint8_t* RXMsg);
void NRF_CTRL_PrintMsg(uint8_t* Msg);
void NRF_CTRL_M_CopeRecv(uint8_t* recvM);


#if NRF_CTRL_MASTER
void NRF_CTRL_CheckKeyStatues(void);
#if NRF_CTRL_USE_CTRLDECODE //使用控制端解码
void NRF_CTRL_UpdataVirKeySendMsg(void);
#endif // NRF_CTRL_USE_CTRLDECODE
void NRF_CTRL_CopeMsg(uint8_t* Msg);
void NRF_CTRL_CopeCmdKeyMsg(uint8_t* Keys, uint8_t keys_num);
void NRF_CTRL_CopeData(uint8_t dataByte,  uint8_t* data);
void NRF_CTRL_CopeREQ(uint8_t reqMode, uint8_t dataBytes, uint8_t reqObj,  uint8_t* data);
#else
void NRF_CTRL_CopeMsg(uint8_t* Msg);
void NRF_CTRL_CopeCmdKeyMsg(uint8_t* Keys, uint8_t keys_num);
void NRF_CTRL_CopeData(uint8_t dataByte,  uint8_t* data);
void NRF_CTRL_CopeREQ(uint8_t reqMode, uint8_t dataBytes, uint8_t reqObj,  uint8_t* data);


#endif //NRF_CTRL_MASTER


#endif //_NRF_CONTROLLER_H



