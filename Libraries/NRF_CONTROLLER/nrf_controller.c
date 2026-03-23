#include "nrf_controller.h"
					  
#include "nrf24l01p.h"
#include "bsp_exti.h"
#include "bsp_usart.h"
#include "string.h"
#if NRF_CTRL_OLED_ON                                          
#include "oled.h"
#endif

#if NRF_CTRL_USE_CTRLDECODE //使用控制端解码
#include "controller_decode.h"
#endif // NRF_CTRL_USE_CTRLDECODE

#if NRF_CTRL_MODIFYSPEED // 修改小车速度
extern int32_t G_BSPCTRL_TargetSpeed ;//前后速度
extern int32_t G_BSPCTRL_TurnSpeed ;//转向速度，正为左转，负为右转
#endif // NRF_CTRL_MODIFYSPEED

#if 0
uint8_t NRF_CTRL_Recvbuf[WRX_PAYLOAD_WIDTH] ; //被控端接收的数据
uint32_t G_NRF_CTRL_STA_Flag;//连接器状态标志
static uint32_t SG_NRF_CTRL_CD_Tick; //长连接心跳
static uint8_t SG_NRF_CTRL_CD_Status; //连接检测状态，0：未连接，else：已连接

/**
  * @brief   NRF遥控器/被控器运行函数，单次运行无阻塞
  * @param   
  * @retval  
**/
void NRF_Controller_RunOnce(void)
{
  /* 接收消息，并处理 */

  #if NRF_CTRL_MASTER //主控端
  NRF_CTRL_RecvMsg();
  NRF_CTRL_CheckKeyStatues();//检测按键并发送
  #if NRF_CTRL_USE_CTRLDECODE //使用控制端解码
  NRF_CTRL_UpdataVirKeySendMsg();
  #endif // NRF_CTRL_USE_CTRLDECODE
  #else //被控端
  NRF_CTRL_RecvMsg();
  #endif // NRF_CTRL_MASTER

  
  //读取按键码
  

  //发送数据
  // NRF_CTRL_SendMsg(uint8_t CMD, uint8_t* Msg, uint8_t MsgLen);

  //更新连接状态


  return;
}


/**
  * @brief   基于NRF的遥控器/被控器初始化
  * @param   
  * @retval  1：初始化成功  0：初始化失败
**/
uint8_t NRF_Controller_Config(void)
{
    uint8_t ret;
    SG_NRF_CTRL_CD_Tick = 0;
    SG_NRF_CTRL_CD_Status = 0;
    #if NRF_CTRL_MASTER  //主控/遥控端
      #if NRF_CTRL_USE_KEYIOSCAN 
        KeyInit();
      #endif // NRF_CTRL_USE_KEYIOSCAN
      ret = NRF24L01P_Init(NRF_MODE_TX);//初始化NRF为遥控端
    #else // 被控端
      ret = NRF24L01P_Init(NRF_MODE_RX);//初始化NRF为被控端
    #endif //NRF_CTRL_MASTER

    #if NRF_CTRL_DEBUG_ON
    NRF_CTRL_DEBUG("Configuring NRF Conctroller...");
    #endif //NRF_CTRL_DEBUG_ON

    return ret;
}


/**
  * @brief   发送数据，第一字节为指令，第二字节为有效数据长度，后面30字节为数据
  * @param   CMD : 指令
  * @param   Msg : 数据
  * @param   MsgLen : 数据长度,最大30Bytes
  * @retval  0表示成功
**/
uint8_t NRF_CTRL_SendMsg(uint8_t CMD, uint8_t* Msg, uint8_t MsgLen)
{
  uint8_t TxBuf[32];
  TxBuf[0] = CMD;
  TxBuf[1] = MsgLen;
  if (MsgLen>0)
  {
    memcpy(&(TxBuf[2]), Msg, MsgLen);
  }
  
  return NRF24L01_TxPacket(TxBuf);
}


/**
  * @brief   NRF主控端被控端连接检测
  * @param   
  * @retval 1:连接成功 
**/
uint8_t NRF_CTRL_ConnectionDetect(void)
{
  uint8_t sMsg[5] = {0x10, 0x21, 0x32, 0x43, 0x54};
  uint8_t rMsg[32] = {};
  uint8_t tmpRet;
  uint8_t ret=1;

  NRF_CTRL_SendMsg(NRF_CTRL_CMD_CONNDET, sMsg, 5);
  tmpRet = NRF24L01_RxPacket(rMsg);
  if (!tmpRet)
  {
    /* 接受到消息 */
    if ((rMsg[0]==NRF_CTRL_CMD_CONNDET)&&(rMsg[1]==5))
    {
      /* 消息格式正确 */
      for (uint8_t i = 0; i < 5; i++)
      {
        if (rMsg[i+2]!=sMsg[i])
        {
          /* 返回数据错误 */
          ret = 0;
        #if NRF_CTRL_DEBUG_ON //
          NRF_CTRL_DEBUG("Connection Detect data error!\n");
          NRF_CTRL_DEBUG("CMD: %d, msglen:%d", rMsg[0], rMsg[1]);
          for (uint8_t i = 0; i < rMsg[1]; i++)
          {
            NRF_CTRL_DEBUG("msg[%d]: %d,", i, rMsg[i+2]);
    
          }
        #endif // NRF_CTRL_DEBUG_ON
          break;
        }
      }
    }
    else
    {
      /* 消息格式错误 */
      ret = 0;
    #if NRF_CTRL_DEBUG_ON //
      NRF_CTRL_DEBUG("Connection Detect data error!\n");
      NRF_CTRL_DEBUG("CMD: %d, msglen:%d", rMsg[0], rMsg[1]);
      for (uint8_t i = 0; i < rMsg[1]; i++)
      {
        NRF_CTRL_DEBUG("msg[%d]: %d,", i, rMsg[i+2]);

      }
      
    #endif // NRF_CTRL_DEBUG_ON
    }
    
  }
  else
  {
    /* 未接受到消息 */
    ret = 0;
  #if NRF_CTRL_DEBUG_ON //
    NRF_CTRL_DEBUG("Connection Detect time out!\n");
  #endif // NRF_CTRL_DEBUG_ON
  }
  
  if (ret)
  {
    /* 连接检测成功 */
    SG_NRF_CTRL_CD_Status = 1;
    SG_NRF_CTRL_CD_Tick = 0;
  #if NRF_CTRL_DEBUG_ON //
    NRF_CTRL_DEBUG("Connection Detect OK!\n");
  #endif // NRF_CTRL_DEBUG_ON
  }
  else
  {
    /* 连接失败 */
    SG_NRF_CTRL_CD_Status = 0;
  }
  
  return ret;
}


/**
  * @brief   NRF主控端被控端心跳自增
  * @param   
  * @retval  
注：执行周期 NRF_CTRL_TICK_PERIOD
**/
void NRF_CTRL_TickIncrease(void)
{
  SG_NRF_CTRL_CD_Tick++;
}


/**
  * @brief   NRF主控端被控端长连接心跳检测
  * @param   
  * @retval  
**/
void NRF_CTRL_TickDetect(uint32_t tic)
{
  if (tic*NRF_CTRL_TICK_PERIOD > NRF_CTRL_TICKDET_TIMEOUT)
  {
    /* 超时 */
    SG_NRF_CTRL_CD_Tick = 0;
    if (G_NRF_CTRL_STA_Flag|NRF_CTRL_STA_CONNECTED)
    {
     /* 已连接 */
      if (NRF_CTRL_SendTick())
      {
        /* 发送心跳失败 */
        G_NRF_CTRL_STA_Flag &= ~(NRF_CTRL_STA_CONNECTED);
      }
    }
    else
    {
      /* 未连接 */
      if (NRF_CTRL_ConnectionDetect())
      {
        /* 连接成功 */
        G_NRF_CTRL_STA_Flag |= NRF_CTRL_STA_CONNECTED;
      }
    }
  }
  return;
}


/**
  * @brief   发送心跳
  * @param   
  * @retval 0:成功 
**/
uint8_t NRF_CTRL_SendTick(void)
{
  uint8_t Msg;
  uint8_t ret;
  ret = NRF_CTRL_SendMsg(NRF_CTRL_CMD_CONNDET, &Msg,  0);
  return ret;
}


/**
  * @brief   接收发来的数据并处理，单次检测，无阻塞
  * @param   
  * @retval  
**/
void NRF_CTRL_RecvMsg(void)
{
  uint8_t tmp=0xff ;

  tmp=NRF24L01_FastRxPacket( NRF_CTRL_Recvbuf);//接收测试数据
  
  if(!tmp)//接收到数据
  {
    SG_NRF_CTRL_CD_Tick = 0;
    NRF_CTRL_CopeMsg(NRF_CTRL_Recvbuf);//处理数据
  }

  return;
}


#if NRF_CTRL_MASTER //主控/遥控端

/**
  * @brief  主控/遥控端，检测并发送按键状态
  * @param   
  * @retval  
注：目前只提供了按键IO扫描硬件驱动
**/
void NRF_CTRL_CheckKeyStatues(void)
{
  uint8_t tmp_readFlagNum=0;
  uint8_t key_fifoFlag[30];

#if NRF_CTRL_USE_KEYIOSCAN // 使用按键IO扫描作为硬件按键驱动
  tmp_readFlagNum = KeyGetFIFOFlag(key_fifoFlag,  8);
#endif // NRF_CTRL_USE_KEYIOSCAN

  if (tmp_readFlagNum !=0)
  {
    /* 读取到按键 */
    #if NRF_CTRL_USE_CTRLDECODE //使用控制端解码
    CTRL_DECODE_CopeCmdKeyMsg(key_fifoFlag, tmp_readFlagNum);
    #else // NRF_CTRL_USE_CTRLDECODE
    NRF_CTRL_SendMsg(NRF_CTRL_CMD_KEY, key_fifoFlag, tmp_readFlagNum);
    #endif // NRF_CTRL_USE_CTRLDECODE
  }
  
#if NRF_CTRL_DEBUG_ON //
  NRF_CTRL_DEBUG( "get %d key flag\n", tmp_readFlagNum);
#endif // NRF_CTRL_DEBUG_ON
 
  return ;
}


#if NRF_CTRL_USE_CTRLDECODE //使用控制端解码
/**
  * @brief  主控/遥控端，检测虚拟按键状态并发送数据给被控端
  * @param   
  * @retval  
注：仅供控制端解码使用
**/
void NRF_CTRL_UpdataVirKeySendMsg(void)
{
  uint8_t VKey = 0;
  uint8_t Msg[10];
  CTRL_DECODE_GetVirtualKey(&VKey);
  if (VKey&CTRL_DECODE_Key_CONNDET)
  {
    /* 连接检测 */
    NRF_CTRL_ConnectionDetect();
    #if NRF_CTRL_OLED_ON
    OLED_Display_XxX_ASCII( 0, 0,  8, 16, 1, "ConnectDetect\n");
    OLED_Refresh();
    #endif // NRF_CTRL_OLED_ON
  }
  if (VKey&CTRL_DECODE_Key_DISTANCEDETON)
  {
    /* 开启距离检测 */
    Msg[0] = NRF_CTRL_KEY_DT_ON;
    NRF_CTRL_SendMsg( NRF_CTRL_CMD_KEY, Msg,  1);
    #if NRF_CTRL_OLED_ON
    OLED_Display_XxX_ASCII( 0, 0,  8, 16, 1, "DisDetectOn\n");
    OLED_Refresh();
    #endif // NRF_CTRL_OLED_ON
  }
  if (VKey&CTRL_DECODE_Key_DISTANCEDETOFF)
  {
    /* 关闭距离检测 */
    Msg[0] = NRF_CTRL_KEY_DT_OFF;
    NRF_CTRL_SendMsg( NRF_CTRL_CMD_KEY, Msg,  1);
    #if NRF_CTRL_OLED_ON
    OLED_Display_XxX_ASCII( 0, 0,  8, 16, 1, "DisDetectOff\n");
    OLED_Refresh();
    #endif // NRF_CTRL_OLED_ON
  }
  if (VKey&CTRL_DECODE_Key_UPDATAWHEEL)
  {
    /* 更新车轮速度 */
    int32_t Lw = 0;
    int32_t Rw = 0;
    CTRL_DECODE_GetWheelSpeed(&Lw, &Rw);
    Msg[0] = NRF_CTRL_REQ_WheelSpeed;
    #if NRF_CTRL_WHEEL2MOVETURN //将两轮速度解码成移动和转向
    int32_t moveS = 0;
    int32_t turnS = 0;
    int32_t tmpE = 0;

    /* 速度解码 */
    tmpE = Rw - Lw;
    turnS = tmpE/2;
    moveS = Rw - turnS;

    Msg[1] = (uint8_t)(moveS & 0xff);
    Msg[2] = (uint8_t)((moveS>>8) & 0xff);
    Msg[3] = (uint8_t)((moveS>>16) & 0xff);
    Msg[4] = (uint8_t)((moveS>>24) & 0xff);
    
    Msg[5] = (uint8_t)(turnS & 0xff);
    Msg[6] = (uint8_t)((turnS>>8) & 0xff);
    Msg[7] = (uint8_t)((turnS>>16) & 0xff);
    Msg[8] = (uint8_t)((turnS>>24) & 0xff);

    NRF_CTRL_SendMsg( NRF_CTRL_CMD_REQ_RES, Msg,  9);
    #if NRF_CTRL_OLED_ON
    OLED_Display_XxX_ASCII( 0, 0,  8, 16, 1, "send moveS turnS\n");
    OLED_Display_XxX_ASCII( 0, 16,  8, 16, 1, "M:%3d,T:%3d\n",moveS, turnS);
    OLED_Refresh();
    #endif // NRF_CTRL_OLED_ON
    #else 
    Msg[1] = (uint8_t)(Lw & 0xff);
    Msg[2] = (uint8_t)((Lw>>8) & 0xff);
    Msg[3] = (uint8_t)((Lw>>16) & 0xff);
    Msg[4] = (uint8_t)((Lw>>24) & 0xff);
    
    Msg[5] = (uint8_t)(Rw & 0xff);
    Msg[6] = (uint8_t)((Rw>>8) & 0xff);
    Msg[7] = (uint8_t)((Rw>>16) & 0xff);
    Msg[8] = (uint8_t)((Rw>>24) & 0xff);

    NRF_CTRL_SendMsg( NRF_CTRL_CMD_REQ_RES, Msg,  9);
    #if NRF_CTRL_OLED_ON
    OLED_Display_XxX_ASCII( 0, 0,  8, 16, 1, "send Lw Rw\n");
    OLED_Display_XxX_ASCII( 0, 16,  8, 16, 1, "L:%3d,R:%3d\n",Lw, Rw);
    OLED_Refresh();
    #endif // NRF_CTRL_OLED_ON
    #endif // NRF_CTRL_WHEEL2MOVETURN
  }
  
  return;
}

#endif // NRF_CTRL_USE_CTRLDECODE


/**
  * @brief   主控端 处理接收的32Bytes数据，第一字节为指令，第二字节为有效数据长度，第三字节开始为有效数据
  * @param   Msg : 数据
  * @retval  
**/
void NRF_CTRL_CopeMsg(uint8_t* Msg)
{
  uint8_t CMD,MsgLen;
  CMD = Msg[0];
  MsgLen = Msg[1];
  
  switch (CMD)
  {
  case NRF_CTRL_CMD_DUMMY:
  {
  } break;
  case NRF_CTRL_CMD_KEY:
  {
    /* 按键数据 */
    if ((MsgLen>=1))
    {
      /* 按键处理 */
      Msg++;
      Msg++;

      NRF_CTRL_CopeCmdKeyMsg(Msg, MsgLen);
    }
  } break;
  case NRF_CTRL_CMD_DATA:
  {
    /* 数据处理 */
    Msg+=2;
    NRF_CTRL_CopeData( MsgLen, Msg);
  } break;
  case NRF_CTRL_CMD_REQ:
  {
    /* 请求处理 */
    uint8_t reqObj = Msg[2];
    Msg+=3;
    NRF_CTRL_CopeREQ( CMD,  MsgLen,  reqObj,  Msg);
  } break;
  case NRF_CTRL_CMD_REQ_RES:
  {
    /* 请求处理 */
    uint8_t reqObj = Msg[2];
    Msg+=3;
    NRF_CTRL_CopeREQ( CMD,  MsgLen,  reqObj,  Msg);
  } break;
  case NRF_CTRL_CMD_REQ_REFUSE:
  {
    /* 请求处理 */
    uint8_t reqObj = Msg[2];
    Msg+=3;
    NRF_CTRL_CopeREQ( CMD,  MsgLen,  reqObj,  Msg);
  } break;
  case NRF_CTRL_CMD_CONNDET:
  {
    /* 连接检测 */
    if (MsgLen==0)
    {
      /* 心跳检测 */
      SG_NRF_CTRL_CD_Tick = 0;
    }
    else if (MsgLen==5)
    {
      /* 返回检测数据 */
      NRF_CTRL_SendMsg(NRF_CTRL_CMD_CONNDET, &Msg[2], 5);
    }
  } break;
  default:
    break;
  }

  return ;
}


/**
  * @brief   主控端 按键信息处理处理。
  * @param   Keys : 接受到的按键数据
  * @param   keys_num : 接受到的按键数据个数
  * @retval  
**/
void NRF_CTRL_CopeCmdKeyMsg(uint8_t* Keys, uint8_t keys_num)
{

  return ;
}


/**
  * @brief   主控端 数据处理
  * @param   dataByte : 数据长度(第二字节)
  * @param   data : 有效数据开始地址
  * @retval  
**/
void NRF_CTRL_CopeData(uint8_t dataByte,  uint8_t* data)
{

  return ;
}


/**
  * @brief   主控端 处理请求，第一字节为请求类型，第二字节为有效数据长度，第三字节开始为有效数据，第三字节为请求的目标/对象，后面为数据
  * @param   reqMode : 请求类型(第一字节)
  * @param   dataBytes : 数据长度(第二字节)
  * @param   reqObj : 请求目标/对象(第三字节)
  * @param   data : 数据开始地址(第四字节)
  * @retval  
**/
void NRF_CTRL_CopeREQ(uint8_t reqMode, uint8_t dataBytes, uint8_t reqObj,  uint8_t* data)
{

  switch (reqMode)
  {
  case NRF_CTRL_CMD_REQ_RES:
  {
    /* 请求回复 */
    if ((dataBytes==9)&&(reqObj==NRF_CTRL_REQ_WheelSpeed))
    {
      /* 收到车轮速度 */
      int32_t Lw = 0;
      int32_t Rw = 0;
      Lw |= data[0];
      Lw |= (uint32_t)(data[1]) << 8;
      Lw |= (uint32_t)(data[2]) << 16;
      Lw |= (uint32_t)(data[3]) << 24;

      Rw |= data[4];
      Rw |= (uint32_t)(data[5]) << 8;
      Rw |= (uint32_t)(data[6]) << 16;
      Rw |= (uint32_t)(data[7]) << 24;

      //获取到车速
      #if NRF_CTRL_DEBUG_ON
      NRF_CTRL_DEBUG("Wheel Speed Lw:%d, Rw:%d \n", Lw, Rw);
      #endif //NRF_CTRL_DEBUG_ON
    }
    else if ((dataBytes==17)&&(reqObj==NRF_CTRL_REQ_DeviceInfo))
    {
      /* 设备信息 */

    }
    else if ((dataBytes==2)&&(reqObj==NRF_CTRL_REQ_BatteryLevel))
    {
      /* 电池电压 */

    }
    else if ((dataBytes==5)&&(reqObj==NRF_CTRL_REQ_RunTime))
    {
      /* 运行时间 */
      int32_t runTime = 0;
      runTime |= data[0];
      runTime |= (uint32_t)(data[1]) << 8;
      runTime |= (uint32_t)(data[2]) << 16;
      runTime |= (uint32_t)(data[3]) << 24;

      #if NRF_CTRL_DEBUG_ON
      NRF_CTRL_DEBUG("Run time:%dms \n", runTime);
      #endif //NRF_CTRL_DEBUG_ON
    }
    

  }break;
  case NRF_CTRL_CMD_REQ_REFUSE:
  {
    /* 请求拒绝 */
  }break;
  default:
    break;
  }

  return ;
}

#else //NRF_CTRL_MASTER 被控端


/**
  * @brief   被控端 处理接收的32Bytes数据，第一字节为指令，第二字节为有效数据长度，后面30字节为有效数据数据
  * @param   Msg : 数据
  * @retval  
**/
void NRF_CTRL_CopeMsg(uint8_t* Msg)
{
  uint8_t CMD,MsgLen;
  CMD = Msg[0];
  MsgLen = Msg[1];
  
  switch (CMD)
  {
  case NRF_CTRL_CMD_DUMMY:
  {
  } break;
  case NRF_CTRL_CMD_KEY:
  {
    /* 按键数据 */
    if ((MsgLen>=1))
    {
      /* 按键处理 */
      Msg++;
      Msg++;
      NRF_CTRL_CopeCmdKeyMsg(Msg, MsgLen);
    }
  } break;
  case NRF_CTRL_CMD_DATA:
  {
    /* 数据处理 */
    Msg+=2;
    NRF_CTRL_CopeData( MsgLen, Msg);
  } break;
  case NRF_CTRL_CMD_REQ:
  {
    /* 请求处理 */
    uint8_t reqObj = Msg[2];
    Msg+=3;
    NRF_CTRL_CopeREQ( CMD,  MsgLen,  reqObj,  Msg);
  } break;
  case NRF_CTRL_CMD_REQ_RES:
  {
    /* 请求处理 */
    uint8_t reqObj = Msg[2];
    Msg+=3;
    NRF_CTRL_CopeREQ( CMD,  MsgLen,  reqObj,  Msg);
  } break;
  case NRF_CTRL_CMD_REQ_REFUSE:
  {
    /* 请求处理 */
    uint8_t reqObj = Msg[2];
    Msg+=3;
    NRF_CTRL_CopeREQ( CMD,  MsgLen,  reqObj,  Msg);
  } break;
  case NRF_CTRL_CMD_CONNDET:
  {
    /* 连接检测 */
    if (MsgLen==0)
    {
      /* 心跳检测 */
      SG_NRF_CTRL_CD_Tick = 0;
    }
    else if (MsgLen==5)
    {
      /* 返回检测数据 */
      NRF_CTRL_SendMsg(NRF_CTRL_CMD_CONNDET, Msg, 5);
    }
  } break;
  default:
    break;
  }

  return ;
}



/**
  * @brief   被控端 按键信息处理
  * @param   Keys : 接受到的按键数据
  * @param   keys_num : 接受到的按键数据个数
  * @retval  
**/
void NRF_CTRL_CopeCmdKeyMsg(uint8_t* Keys, uint8_t keys_num)
{
  for (uint8_t i = 0; i < keys_num; i++)
  {
    /* code */
    switch (Keys[i])
    {
    case NRF_CTRL_KEY_DT_ON:
    {
      /* 开启距离检测 */
    #if NRF_CTRL_DEBUG_ON //
      NRF_CTRL_DEBUG( "get key NRF_CTRL_KEY_DT_ON\n");
    #endif // NRF_CTRL_DEBUG_ON
    }break;
    case NRF_CTRL_KEY_DT_OFF:
    {
      /* 关闭距离检测 */
    #if NRF_CTRL_DEBUG_ON //
      NRF_CTRL_DEBUG( "get key NRF_CTRL_KEY_DT_OFF\n");
    #endif // NRF_CTRL_DEBUG_ON
    }break;
    
    default:
      break;
    }

  }
  
  return ;
}


/**
  * @brief   数据处理，接收的32Bytes数据，第一字节为数据指令，第二字节为数据长度，后面30字节为数据
  * @param   dataByte : 数据长度
  * @param   data : 有效数据开始地址
  * @retval  
**/
void NRF_CTRL_CopeData(uint8_t dataByte,  uint8_t* data)
{

  return ;
}


/**
  * @brief   处理请求，第一字节为请求类型，第二字节为有效数据长度，第三字节为请求的目标/对象
  * @param   reqMode : 请求类型(第一字节)
  * @param   dataBytes : 数据长度(第二字节)
  * @param   reqObj : 请求目标/对象(第三字节)
  * @param   data : 数据开始地址(第四字节)
  * @retval  
**/
void NRF_CTRL_CopeREQ(uint8_t reqMode, uint8_t dataBytes, uint8_t reqObj,  uint8_t* data)
{

  if ((reqMode == NRF_CTRL_CMD_REQ_RES)&&(dataBytes==9)&&(reqObj==NRF_CTRL_REQ_WheelSpeed))
  {

    #if NRF_CTRL_WHEEL2MOVETURN // 将两轮速度解码成移动和转向
    /* 收到车轮控制数据 */
    uint32_t turnS = 0;
    uint32_t moveS = 0;
    moveS |= data[0];
    moveS |= ((uint32_t)(data[1])) << 8;
    moveS |= ((uint32_t)(data[2])) << 16;
    moveS |= ((uint32_t)(data[3])) << 24;
    
    turnS |= data[4];
    turnS |= ((uint32_t)(data[5])) << 8;
    turnS |= ((uint32_t)(data[6])) << 16;
    turnS |= ((uint32_t)(data[7])) << 24;
    
    #if NRF_CTRL_MODIFYSPEED //
    G_BSPCTRL_TargetSpeed = (int32_t)moveS ;//前后速度
    G_BSPCTRL_TurnSpeed = (int32_t)turnS ;//转向速度，正为左转，负为右转
    #endif // NRF_CTRL_MODIFYSPEED


    //获取到车速
    #if NRF_CTRL_DEBUG_ON //
      NRF_CTRL_DEBUG( "get wheel speed moveS:%d%%, turnS:%d%%\n", (int32_t)moveS, (int32_t)turnS);
    #endif // NRF_CTRL_DEBUG_ON

    #else 
    /* 收到车轮控制数据 */
    uint32_t Lw = 0;
    uint32_t Rw = 0;
    Lw |= data[0];
    Lw |= ((uint32_t)(data[1])) << 8;
    Lw |= ((uint32_t)(data[2])) << 16;
    Lw |= ((uint32_t)(data[3])) << 24;
    
    Rw |= data[4];
    Rw |= ((uint32_t)(data[5])) << 8;
    Rw |= ((uint32_t)(data[6])) << 16;
    Rw |= ((uint32_t)(data[7])) << 24;
    
    //获取到车速
    #if NRF_CTRL_DEBUG_ON //
      NRF_CTRL_DEBUG( "get wheel speed Lw:%d%%, Rw:%d%%\n", (int32_t)Lw, (int32_t)Rw);
    #endif // NRF_CTRL_DEBUG_ON
    #endif // NRF_CTRL_WHEEL2MOVETURN 
  }
  
  return ;
}


#endif //NRF_CTRL_MASTER

#endif


