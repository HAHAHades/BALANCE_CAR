#include "nrf_controller.h"
#include "bsp_exti.h"
#include "string.h"


#if NRF_CTRL_OLED_ON                                          
#include "oled.h"
#endif



#if NRF_CTRL_MODIFYSPEED // 修改小车速度
// extern int32_t G_BSPCTRL_TargetSpeed ;//前后速度
// extern int32_t G_BSPCTRL_TurnSpeed ;//转向速度，正为左转，负为右转
#endif // NRF_CTRL_MODIFYSPEED

#if 1
static uint32_t SG_NRF_CTRL_CD_Tick; //长连接心跳
// static uint8_t SG_NRF_CTRL_CD_Status; //连接检测状态，0：未连接，else：已连接


static NRF24L01P_Hard_Typedef SG_NRF_CTRL_NRF_HardStruct;//遥控器使用的硬件对象

void NRF_Controller_UsageDemo(void)
{
    #if NRF_CTRL_MASTER
    //主控端

	uint8_t initSta; 
	uint8_t Button_Count = 4;
	uint8_t Comb_Count = 2;
	uint8_t Pot_Count = 4;

	initSta = BSP_KEY_InitParam( Button_Count,  Comb_Count, Pot_Count);
	if (initSta)
	{
		//return init failed code
		return;
	}
	
	//先注册配置实体按钮，索引从0开始，小于Button_Count
	KEY_Button_TypeDef ButtonStruct;
	//设置按钮0参数为 PA0引脚 低电平有效，开启单击按下、长按、双击回调
	BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_8, KEY_PL_RESET, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Button_Register( 0,  ButtonStruct);
	BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_9, KEY_PL_RESET, (KEY_Flag_P|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Button_Register( 1,  ButtonStruct);
	BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_10, KEY_PL_RESET, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Button_Register( 2,  ButtonStruct);
	BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_11, KEY_PL_RESET, (KEY_Flag_P|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Button_Register( 3,  ButtonStruct);


	//再注册配置组合键，按键数多的索引应越小，否则会出现歧义(比如A+B+C会触发A+B)，索引从0开始，小于Comb_Count
	KEY_COMBINATION_t CombStruct;
	//组合键0 包含3个按键 索引分别为 0，2，4，开启单击释放、长按、双击回调
	uint8_t indexList1[] = {0,2};
	BSP_KEY_COMB_StructInit( &CombStruct,  2, indexList1, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Comb_Register(0 , CombStruct);
	uint8_t indexList2[] = {0,3};
	BSP_KEY_COMB_StructInit( &CombStruct,  2, indexList2, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Comb_Register(1 , CombStruct);

	//再注册电位计按键
	KEY_Pot_TypeDef PotStruct;
	KEY_Pot_StructInit(&PotStruct, ADC1,  ADC_Channel_0);
	BSP_KEY_Pot_Register(0,  PotStruct);
	KEY_Pot_StructInit(&PotStruct, ADC1,  ADC_Channel_1);
	BSP_KEY_Pot_Register(1,  PotStruct);
	KEY_Pot_StructInit(&PotStruct, ADC1,  ADC_Channel_2);
	BSP_KEY_Pot_Register(2,  PotStruct);
	KEY_Pot_StructInit(&PotStruct, ADC1,  ADC_Channel_3);
	BSP_KEY_Pot_Register(3,  PotStruct);

	BSP_KEY_GpioConfig();

	//重复执行 Key_TickTask 更新io状态
	//根据需求重复执行void KeyDetectButton(void)更新按键FIFO


    uint8_t tmpR;

    //全局指针方便调用中断函数
    G_NRF_HardStruct_ForEXTI0 = &SG_NRF_CTRL_NRF_HardStruct;
    NRF_HardStruct_Init(&SG_NRF_CTRL_NRF_HardStruct, NRF_Mode_Tx, NRF_ACK_PAY_EN ,SPI1,  SPI1_IO_Reamp0, GPIOB,  GPIO_Pin_1,  GPIOB,  GPIO_Pin_0);
    tmpR = NRF24L01P_Init( &SG_NRF_CTRL_NRF_HardStruct);//测试硬件是否异常
    if (tmpR)
    {
      NRF_CTRL_DEBUG("NRF24L01P_Init Failed!");
      return;
    }
    tmpR = NRF_TXRX_TEST( &SG_NRF_CTRL_NRF_HardStruct, 10000 );

    if (tmpR)
    {
      NRF_CTRL_DEBUG("NRF_TXRX_TEST successful!");
    }
    else
    {
      NRF_CTRL_DEBUG("NRF_TXRX_TEST failed!");
      return;
    }

    uint8_t FlagCount=0;
    KEY_FIFO_t keyFifo[KEY_FIFO_CAPACITY];
    
    uint32_t nowTime = 0;
    uint32_t timeDiff = 0;
    uint32_t NRFCTRL_LastGetFIFOTime = 0;
    uint32_t NRFCTRL_LastTRMSGTime = 0;
    uint32_t Key_TickTask_LastRunTime = 0;
    uint32_t Key_TickTaskTime = 10;

    uint8_t msg[WRX_PAYLOAD_WIDTH];   
    uint8_t copedFifoCount;   

    while (1)
    {
        nowTime = NRFCTR_GetTime();

        timeDiff = nowTime - Key_TickTask_LastRunTime;
        if (timeDiff > Key_TickTaskTime)
        {
          Key_TickTask();
          KeyDetectButton();
          Key_TickTask_LastRunTime = NRFCTR_GetTime();;
        }

        timeDiff = nowTime - NRFCTRL_LastGetFIFOTime;

        if (timeDiff > NRF_CTRL_GetFIFO_Period)
        {
          FlagCount = KeyGetFIFOFlag(keyFifo,  KEY_FIFO_CAPACITY);
          for (uint8_t i = 0; i < FlagCount; i++)
          {
            if (keyFifo[i] & KEY_POT_FIFO_FLAG)
            {
              //电位计数据

              KEY_POT_PCT_t potData = (KEY_POT_PCT_t)(keyFifo[i]&KEY_POT_FIFO_MASK);
              uint8_t potIndex = (keyFifo[i]&KEY_POT_FIFO_I_MASK)>>KEY_POT_FIFO_I_BIT_S;
              NRF_CTRL_DEBUG("pot%d data%d", potIndex, potData);
              
            }
          }
          NRFCTRL_LastGetFIFOTime = nowTime;
        }
        timeDiff = nowTime - NRFCTRL_LastTRMSGTime;
        if (timeDiff > NRT_CTRL_TRMsg_Period)
        {
            #if NRF_CTRL_USE_CTRLDECODE
            copedFifoCount = 0;
            while (FlagCount)
            {
                //一次最多处理发送6组数据
                copedFifoCount = CTRL_M_DECODE_CopeKeyFIFO( &keyFifo[copedFifoCount],  FlagCount, msg);
                NRF_CTRL_SendMsg( NRF_CTRL_CMD_KEY,  msg,  copedFifoCount);
                //NRF_CTRL_PrintMsg(msg);
                FlagCount -= copedFifoCount;
            }

            #else
            do
            {
              copedFifoCount = NRF_CTRL_FIFO2MSG(keyFifo, FlagCount, msg);
              NRF_CTRL_SendMsg( NRF_CTRL_CMD_KEY,  msg,  copedFifoCount);
            } while (FlagCount);

            #endif //NRF_CTRL_USE_CTRLDECODE
            NRFCTRL_LastTRMSGTime = nowTime;
        }
    }

    #else 
    //被控端

    uint8_t tmpR;

    //全局指针方便调用中断函数
    G_NRF_HardStruct_ForEXTI0 = &SG_NRF_CTRL_NRF_HardStruct;

    NRF_HardStruct_Init(&SG_NRF_CTRL_NRF_HardStruct, NRF_Mode_Rx, NRF_ACK_PAY_EN, SPI1,  SPI1_IO_Reamp0, GPIOB,  GPIO_Pin_1,  GPIOB,  GPIO_Pin_0);
    tmpR = NRF24L01P_Init( &SG_NRF_CTRL_NRF_HardStruct);//测试硬件是否异常
    if (tmpR)
    {
      NRF_CTRL_DEBUG("NRF24L01P_Init Failed!");
      return;
    }

    tmpR = NRF_TXRX_TEST( &SG_NRF_CTRL_NRF_HardStruct, 10000);//测试主控与被控端通信是否异常
    if (tmpR)
    {
      NRF_CTRL_DEBUG("NRF_TXRX_TEST successful!");
    }
    else
    {
      NRF_CTRL_DEBUG("NRF_TXRX_TEST failed!");
      return;
    }

    uint32_t nowTime = 0;
    uint32_t timeDiff = 0;
    uint32_t NRFCTRL_LastTRMSGTime = 0;

    uint8_t msg[WRX_PAYLOAD_WIDTH];      

    while (1)
    {
        nowTime = NRFCTR_GetTime();
        timeDiff = nowTime - NRFCTRL_LastTRMSGTime;
        if (timeDiff > NRT_CTRL_TRMsg_Period)
        {
          if (!NRF_CTRL_RecvMsg( msg))
          {
            //接收到数据 处理数据
            NRF_CTRL_PrintMsg(msg);
            CTRL_S_DECODE_CopeKeyFIFO( msg);
          }
          NRFCTRL_LastTRMSGTime = NRFCTR_GetTime();
        }
    }
    #endif //NRF_CTRL_MASTER

}


/** 
  * @brief   配置控制器
  * @param   
  * @retval  0表示成功
  */
uint8_t NRF_Controller_Config(void)
{
    #if NRF_CTRL_MASTER
    //主控端

	uint8_t initSta; 
	uint8_t Button_Count = 4;
	uint8_t Comb_Count = 2;
	uint8_t Pot_Count = 4;

	initSta = BSP_KEY_InitParam( Button_Count,  Comb_Count, Pot_Count);
	if (initSta)
	{
		//return init failed code
		return 1;
	}
	
	//先注册配置实体按钮，索引从0开始，小于Button_Count
	KEY_Button_TypeDef ButtonStruct;
	//设置按钮0参数为 PA0引脚 低电平有效，开启单击按下、长按、双击回调
	BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_8, KEY_PL_RESET, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Button_Register( 0,  ButtonStruct);
	BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_9, KEY_PL_RESET, (KEY_Flag_P|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Button_Register( 1,  ButtonStruct);
	BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_10, KEY_PL_RESET, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Button_Register( 2,  ButtonStruct);
	BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_11, KEY_PL_RESET, (KEY_Flag_P|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Button_Register( 3,  ButtonStruct);


	//再注册配置组合键，按键数多的索引应越小，否则会出现歧义(比如A+B+C会触发A+B)，索引从0开始，小于Comb_Count
	KEY_COMBINATION_t CombStruct;
	//组合键0 包含3个按键 索引分别为 0，2，4，开启单击释放、长按、双击回调
	uint8_t indexList1[] = {0,2};
	BSP_KEY_COMB_StructInit( &CombStruct,  2, indexList1, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Comb_Register(0 , CombStruct);
	uint8_t indexList2[] = {0,3};
	BSP_KEY_COMB_StructInit( &CombStruct,  2, indexList2, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Comb_Register(1 , CombStruct);

	//再注册电位计按键
	KEY_Pot_TypeDef PotStruct;
	KEY_Pot_StructInit(&PotStruct, ADC1,  ADC_Channel_0);
	BSP_KEY_Pot_Register(0,  PotStruct);
	KEY_Pot_StructInit(&PotStruct, ADC1,  ADC_Channel_1);
	BSP_KEY_Pot_Register(1,  PotStruct);
	KEY_Pot_StructInit(&PotStruct, ADC1,  ADC_Channel_2);
	BSP_KEY_Pot_Register(2,  PotStruct);
	KEY_Pot_StructInit(&PotStruct, ADC1,  ADC_Channel_3);
	BSP_KEY_Pot_Register(3,  PotStruct);

	BSP_KEY_GpioConfig();

	//重复执行 Key_TickTask 更新io状态
	//根据需求重复执行void KeyDetectButton(void)更新按键FIFO


    uint8_t tmpR;

    //全局指针方便调用中断函数
    G_NRF_HardStruct_ForEXTI0 = &SG_NRF_CTRL_NRF_HardStruct;
    NRF_HardStruct_Init(&SG_NRF_CTRL_NRF_HardStruct, NRF_Mode_Tx, NRF_ACK_PAY_EN ,SPI1,  SPI1_IO_Reamp0, GPIOB,  GPIO_Pin_1,  GPIOB,  GPIO_Pin_0);
    tmpR = NRF24L01P_Init( &SG_NRF_CTRL_NRF_HardStruct);//测试硬件是否异常
    if (tmpR)
    {
      NRF_CTRL_DEBUG("NRF24L01P_Init Failed!");
      return 1;
    }


    #else 
    //被控端

    uint8_t tmpR;

    //全局指针方便调用中断函数
    G_NRF_HardStruct_ForEXTI0 = &SG_NRF_CTRL_NRF_HardStruct;
    NRF_HardStruct_Init(&SG_NRF_CTRL_NRF_HardStruct, NRF_Mode_Rx, NRF_ACK_PAY_EN, SPI1,  SPI1_IO_Reamp0, GPIOB,  GPIO_Pin_1,  GPIOB,  GPIO_Pin_0);
    tmpR = NRF24L01P_Init( &SG_NRF_CTRL_NRF_HardStruct);//测试硬件是否异常
    if (tmpR)
    {
      NRF_CTRL_DEBUG("NRF24L01P_Init Failed!");
      return 1;
    }






#endif //NRF_CTRL_MASTER

return 0;

}

void NRF_Controller_Run(void)
{
    #if NRF_CTRL_MASTER
    //主控端

    uint8_t FlagCount=0;
    KEY_FIFO_t keyFifo[KEY_FIFO_CAPACITY];
    
    uint32_t nowTime = 0;
    uint32_t timeDiff = 0;
    uint32_t NRFCTRL_LastGetFIFOTime = 0;
    uint32_t NRFCTRL_LastTRMSGTime = 0;
    uint32_t Key_TickTask_LastRunTime = 0;
    uint32_t Key_TickTaskTime = 10;

    uint8_t msg[WRX_PAYLOAD_WIDTH];   
    uint8_t copedFifoCount;   

   
    {
        nowTime = NRFCTR_GetTime();

        timeDiff = nowTime - Key_TickTask_LastRunTime;
        if (timeDiff > Key_TickTaskTime)
        {
          Key_TickTask();
          KeyDetectButton();
          Key_TickTask_LastRunTime = NRFCTR_GetTime();;
        }

        timeDiff = nowTime - NRFCTRL_LastGetFIFOTime;

        if (timeDiff > NRF_CTRL_GetFIFO_Period)
        {
          FlagCount = KeyGetFIFOFlag(keyFifo,  KEY_FIFO_CAPACITY);
          for (uint8_t i = 0; i < FlagCount; i++)
          {
            if (keyFifo[i] & KEY_POT_FIFO_FLAG)
            {
              //电位计数据

              KEY_POT_PCT_t potData = (KEY_POT_PCT_t)(keyFifo[i]&KEY_POT_FIFO_MASK);
              uint8_t potIndex = (keyFifo[i]&KEY_POT_FIFO_I_MASK)>>KEY_POT_FIFO_I_BIT_S;
              NRF_CTRL_DEBUG("pot%d data%d", potIndex, potData);
              
            }
          }
          NRFCTRL_LastGetFIFOTime = nowTime;
        }
        timeDiff = nowTime - NRFCTRL_LastTRMSGTime;
        if (timeDiff > NRT_CTRL_TRMsg_Period)
        {
            #if NRF_CTRL_USE_CTRLDECODE
            copedFifoCount = 0;
            while (FlagCount)
            {
                //一次最多处理发送6组数据
                copedFifoCount = CTRL_M_DECODE_CopeKeyFIFO( &keyFifo[copedFifoCount],  FlagCount, msg);
                NRF_CTRL_SendMsg( NRF_CTRL_CMD_KEY,  msg,  copedFifoCount);
                //NRF_CTRL_PrintMsg(msg);
                FlagCount -= copedFifoCount;
            }

            #else
            do
            {
              copedFifoCount = NRF_CTRL_FIFO2MSG(keyFifo, FlagCount, msg);
              NRF_CTRL_SendMsg( NRF_CTRL_CMD_KEY,  msg,  copedFifoCount);
            } while (FlagCount);

            #endif //NRF_CTRL_USE_CTRLDECODE
            NRFCTRL_LastTRMSGTime = nowTime;
        }
    }

    #else 
    //被控端

    uint32_t nowTime = 0;
    uint32_t timeDiff = 0;
    uint32_t NRFCTRL_LastTRMSGTime = 0;

    uint8_t msg[WRX_PAYLOAD_WIDTH];      


    {
        nowTime = NRFCTR_GetTime();
        timeDiff = nowTime - NRFCTRL_LastTRMSGTime;
        if (timeDiff > NRT_CTRL_TRMsg_Period)
        {
          if (!NRF_CTRL_RecvMsg( msg))
          {
            //接收到数据 处理数据
            NRF_CTRL_PrintMsg(msg);
            CTRL_S_DECODE_CopeKeyFIFO( msg);
          }
          NRFCTRL_LastTRMSGTime = NRFCTR_GetTime();
        }
    }
    #endif //NRF_CTRL_MASTER

}





/**
  * @brief   获取时间戳/ms
  * @param   
  * @retval  
**/
uint32_t NRFCTR_GetTime(void) 
{
	unsigned long count;
	get_tick_count(&count);
	return count;
}


/**
  * @brief   按键信息处理，根据按键标识，做出相应处理，输出处理后的MSG数据。
  * @param   Keys[IN] : 接受到的按键数据
  * @param   keys_num[IN] : 接受到的按键数据个数
  * @param   Msg[IN/OUT] : 输出的数据, 固定32个uint8_t
  * @retval  已处理的Keys个数
**/
uint8_t NRF_CTRL_FIFO2MSG(KEY_FIFO_t* Keys, uint8_t keys_num, uint8_t* Msg)
{

  uint8_t rVal;

  return  rVal;

}

#if 0
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

#endif


/**
  * @brief   发送数据，第一字节为指令，第二字节为有效数据长度，后面每5个字节为一组数据
  * @param   CMD : 指令
  * @param   Msg : 数据
  * @param   MsgLen : 数据个数，最大6组
  * @retval  0表示成功
**/
uint8_t NRF_CTRL_SendMsg(uint8_t CMD, uint8_t* Msg, uint8_t MsgLen)
{
  // uint8_t TxBuf[32];
  Msg[0] = CMD;
  Msg[1] = MsgLen;


  // if (MsgLen>0)
  // {
  //   memcpy((void*)(&(TxBuf[2])), (const void*)Msg, MsgLen*NRF_CTRL_MsgBytes);//此处memcpy赋值不了
  // }
  // NRF_CTRL_DEBUG("Msg[2]:%d", Msg[2]);
  // NRF_CTRL_DEBUG("Msg[5]:%d", Msg[5]);
  // NRF_CTRL_DEBUG("Msg[6]:%d", Msg[6]);
  // NRF_CTRL_DEBUG("TxBuf[2]:%d", TxBuf[2]);
  // NRF_CTRL_DEBUG("TxBuf[5]:%d", TxBuf[5]);
  // NRF_CTRL_DEBUG("TxBuf[6]:%d", TxBuf[6]);
  NRF_CTRL_PrintMsg(Msg);
  return NRF24L01_TxPacket(Msg, NRF_CTRL_MsgTimeOut, &SG_NRF_CTRL_NRF_HardStruct);
}


/**
  * @brief   接收发来的数据并处理，单次检测，无阻塞
  * @param   Msg[OUT]: 接收的数据，固定32Bytes
  * @retval  0表示成功
**/
uint8_t NRF_CTRL_RecvMsg(uint8_t* Msg)
{
  uint8_t tmp=0;
  //接收测试数据
  tmp = NRF24L01_RxPacket(Msg, 1, &SG_NRF_CTRL_NRF_HardStruct);
  if(!tmp)//接收到数据
  {
    SG_NRF_CTRL_CD_Tick = SG_NRF_CTRL_CD_Tick? 0:SG_NRF_CTRL_CD_Tick;
    return 0;
  }

  return 1;
}


/**
  * @brief   输出收到的数据
  * @param   Msg[IN]: 接收的数据，固定32Bytes
  * @retval  
**/
void NRF_CTRL_PrintMsg(uint8_t* Msg)
{
  uint8_t cmd = Msg[0];
  uint8_t msgLen = Msg[1];
  if (msgLen > NRF_CTRL_MAXMsgPerPackage)
  {
    NRF_CTRL_DEBUG("RecvMsg Len:%d, Msg err", msgLen);
    return;
  }
  
  NRF_CTRL_DEBUG("RecvMsg CMD:%d", cmd);
  NRF_CTRL_DEBUG("RecvMsg Len:%d", msgLen);
  
  for (uint8_t i = 0; i < msgLen; i++)
  {
    for (uint8_t j = 0; j < NRF_CTRL_MsgBytes; j++)
    {
      NRF_CTRL_DEBUG("RecvMsg[%d,%d]:%d", i, j, Msg[2+NRF_CTRL_MsgBytes*i+j]);
    }
  }
}


#if 0
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





#endif


#if NRF_CTRL_MASTER //主控/遥控端

#if 0

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

#endif 

#else //NRF_CTRL_MASTER 被控端


#if 0

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


#endif

#endif //NRF_CTRL_MASTER

#endif


