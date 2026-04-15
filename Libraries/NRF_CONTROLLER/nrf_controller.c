#include "nrf_controller.h"
#include "bsp_exti.h"
#include "string.h"
#include "bsp_sbv.h"

static uint32_t SG_NRF_CTRL_CD_Tick; //长连接心跳
NRF24L01P_Hard_Typedef G_NRF_CTRL_NRF_HardStruct;//遥控器使用的硬件对象
uint8_t G_NRF_CTRL_S_ACKMsg[WRX_PAYLOAD_WIDTH];//被控端ACK数据



void NRF_Controller_UsageDemo(void)
{
    if (G_NRF_CTRL_NRF_HardStruct.WorkMode==NRF_Mode_Tx)
    {
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
        G_NRF_HardStruct_ForEXTI0 = &G_NRF_CTRL_NRF_HardStruct;
        NRF_HardStruct_Init(&G_NRF_CTRL_NRF_HardStruct, NRF_Mode_Tx, NRF_ACK_PAY_EN ,SPI1,  SPI1_IO_Reamp0, GPIOB,  GPIO_Pin_1,  GPIOB,  GPIO_Pin_0);
        tmpR = NRF24L01P_Init( &G_NRF_CTRL_NRF_HardStruct);//测试硬件是否异常
        if (tmpR)
        {
            NRF_CTRL_DEBUG("NRF24L01P_Init Failed!");
            return;
        }
        tmpR = NRF_TXRX_TEST_WithAckData( &G_NRF_CTRL_NRF_HardStruct, 10000 );

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
                Key_TickTask_LastRunTime = NRFCTR_GetTime();
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
                copedFifoCount = 0;
                while (FlagCount)
                {
                    //一次最多处理发送6组数据
                    copedFifoCount = CTRL_M_DECODE_CopeKeyFIFO( &keyFifo[copedFifoCount],  FlagCount, msg);
                    msg[0] = NRF_CTRL_CMD_KEY;
                    msg[1] = copedFifoCount;
                    uint8_t rMsg[WRX_PAYLOAD_WIDTH];
                    uint8_t rVal = NRF_CTRL_SendMsg( msg,  rMsg);
                    if (rVal==0)//接收到数据
                    {
                      NRF_CTRL_M_CopeRecv(rMsg);
                    }
                    
                    //NRF_CTRL_PrintMsg(msg);
                    FlagCount -= copedFifoCount;
                }
                NRFCTRL_LastTRMSGTime = nowTime;
            }
        }
    }
    else 
    {
        //被控端
        uint8_t tmpR;
        //全局指针方便调用中断函数
        G_NRF_HardStruct_ForEXTI0 = &G_NRF_CTRL_NRF_HardStruct;
        NRF_HardStruct_Init(&G_NRF_CTRL_NRF_HardStruct, NRF_Mode_Rx, NRF_ACK_PAY_EN, SPI1,  SPI1_IO_Reamp0, GPIOB,  GPIO_Pin_1,  GPIOB,  GPIO_Pin_0);
        tmpR = NRF24L01P_Init( &G_NRF_CTRL_NRF_HardStruct);//测试硬件是否异常
        if (tmpR)
        {
            NRF_CTRL_DEBUG("NRF24L01P_Init Failed!");
            return;
        }

        tmpR = NRF_TXRX_TEST_WithAckData( &G_NRF_CTRL_NRF_HardStruct, 10000);//测试主控与被控端通信是否异常
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
        uint8_t TXmsg[WRX_PAYLOAD_WIDTH]={0x01,0x03,0x05};      
        uint8_t msg[WRX_PAYLOAD_WIDTH];      
        while (1)
        {
            nowTime = NRFCTR_GetTime();
            timeDiff = nowTime - NRFCTRL_LastTRMSGTime;
            if (timeDiff > NRT_CTRL_TRMsg_Period)
            {
            if (!NRF_CTRL_RecvMsg( TXmsg,msg))
            {
                //接收到数据 处理数据
                //NRF_CTRL_PrintMsg(msg);
                CTRL_S_DECODE_CopeKeyFIFO( msg);
            }
            NRFCTRL_LastTRMSGTime = NRFCTR_GetTime();
            }
        }
    }

}

/**
  * @brief   等待主控/被控端连接
  * @param   
  * @retval  
**/
void NRF_Controller_WaitForConnect(void)
{
    while (!NRF_TXRX_TEST_WithAckData( &G_NRF_CTRL_NRF_HardStruct, 10000 ))
    {
        
    }
}


/** 
  * @brief   配置控制器
  * @param   
  * @retval  0表示成功
  */
uint8_t NRF_Controller_Config(uint8_t mode)
{
    
    if (mode==NRF_CTRL_MASTER)    //主控端
    {
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
        ButtonStruct.Sta_Struct._LPRPeriod = 5000; //5s触发一次
        BSP_KEY_Button_Register( 0,  ButtonStruct);
        BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_9, KEY_PL_RESET, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
        BSP_KEY_Button_Register( 1,  ButtonStruct);
        BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_10, KEY_PL_RESET, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
        BSP_KEY_Button_Register( 2,  ButtonStruct);
        BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_11, KEY_PL_RESET, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
        ButtonStruct.Sta_Struct._LPRPeriod = 5000; //5s触发一次
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
        G_NRF_HardStruct_ForEXTI0 = &G_NRF_CTRL_NRF_HardStruct;
        NRF_HardStruct_Init(&G_NRF_CTRL_NRF_HardStruct, NRF_Mode_Tx, NRF_ACK_PAY_EN ,SPI1,  SPI1_IO_Reamp0, GPIOB,  GPIO_Pin_1,  GPIOB,  GPIO_Pin_0);
        tmpR = NRF24L01P_Init( &G_NRF_CTRL_NRF_HardStruct);//测试硬件是否异常
        if (tmpR)
        {
            NRF_CTRL_DEBUG("NRF24L01P_Init Failed!");
            return 1;
        }
    }
    else 
    {
        //被控端
        uint8_t tmpR;
        //全局指针方便调用中断函数
        G_NRF_HardStruct_ForEXTI0 = &G_NRF_CTRL_NRF_HardStruct;
        NRF_HardStruct_Init(&G_NRF_CTRL_NRF_HardStruct, NRF_Mode_Rx, NRF_ACK_PAY_EN, SPI1,  SPI1_IO_Reamp0, GPIOB,  GPIO_Pin_1,  GPIOB,  GPIO_Pin_0);
        tmpR = NRF24L01P_Init( &G_NRF_CTRL_NRF_HardStruct);//测试硬件是否异常
        if (tmpR)
        {
        NRF_CTRL_DEBUG("NRF24L01P_Init Failed!");
        return 1;
        }
    }
    return 0;
}


/** 
  * @brief   
  * @param   
  * @retval  
  */
void NRF_Controller_Run(NRF_Controller_Run_Typedef* RunStruct)
{
    if (G_NRF_CTRL_NRF_HardStruct.WorkMode==NRF_Mode_Tx)
    {
        //主控端
        uint8_t FlagCount=0;
        KEY_FIFO_t keyFifo[KEY_FIFO_CAPACITY];
        
        uint32_t nowTime = 0;
        uint32_t timeDiff = 0;

        uint8_t msg[WRX_PAYLOAD_WIDTH];   
        uint8_t rMsg[WRX_PAYLOAD_WIDTH];
        uint8_t copedFifoCount;   
        uint8_t rVal;
        nowTime = NRFCTR_GetTime();
        timeDiff = nowTime - RunStruct->Key_TickTask_LastRunTime;
        if (timeDiff > NRF_CTRL_Key_TickTask_Period)
        {
          Key_TickTask();
          KeyDetectButton();
          RunStruct->Key_TickTask_LastRunTime = NRFCTR_GetTime();
        }

        timeDiff = nowTime - RunStruct->NRFCTRL_LastGetFIFOTime;
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
            else if (keyFifo[i] == Key1_LP+(0*KEY_FLAG_NUM))//按钮0长按进入调参模式
            {
                msg[0]=NRF_CTRL_CMD_ADJP;
                msg[1]=5;
                msg[2]=NRF_CTRL_CMD_ADJP;
                msg[3]=NRF_CTRL_CMD_ADJP;
                msg[4]=NRF_CTRL_CMD_ADJP;
                msg[5]=NRF_CTRL_CMD_ADJP;
                msg[6]=NRF_CTRL_CMD_ADJP;
                
                rVal = NRF_CTRL_SendMsg(msg, rMsg);//发送调参指令
                if (rVal==0)//接收到数据
                {
                  NRF_CTRL_M_CopeRecv(rMsg);
                }
                FlagCount = 0;
            }
            
          }
          RunStruct->NRFCTRL_LastGetFIFOTime = nowTime;
        }

        timeDiff = nowTime - RunStruct->NRFCTRL_LastTRMSGTime;
        if (timeDiff > NRT_CTRL_TRMsg_Period)
        {
            copedFifoCount = 0;
            while (FlagCount)
            {
                //一次最多处理发送6组数据
                copedFifoCount = CTRL_M_DECODE_CopeKeyFIFO( &keyFifo[copedFifoCount],  FlagCount, msg);
                msg[0]=NRF_CTRL_CMD_KEY;
                msg[1]=copedFifoCount;
                
                rVal = NRF_CTRL_SendMsg(msg,  rMsg);
                NRF_CTRL_DEBUG("rVal:%d\n",rVal);
                if (rVal==0)//接收到数据
                {
                  NRF_CTRL_M_CopeRecv(rMsg);
                }
                //NRF_CTRL_PrintMsg(msg);
                FlagCount -= copedFifoCount;
            }
            RunStruct->NRFCTRL_LastTRMSGTime = nowTime;
        }
    }
    else 
    {
        //被控端
        uint8_t rVal;
        uint8_t msg[WRX_PAYLOAD_WIDTH];  
        if (G_NRF_CTRL_NRF_HardStruct.NRF_S_ACKMsg)//有数据待返回
        {
          rVal = NRF_CTRL_RecvMsg(G_NRF_CTRL_S_ACKMsg, msg);
          G_NRF_CTRL_NRF_HardStruct.NRF_S_ACKMsg = 0;
        }
        else
        {
          rVal = NRF_CTRL_RecvMsg(NULL, msg);
        }
        if (!rVal)
        {
            //接收到数据 处理数据
            NRF_CTRL_PrintMsg(msg);
            if (msg[0]==NRF_CTRL_CMD_KEY)//按键数据
            {
            	CTRL_S_DECODE_CopeKeyFIFO( msg);
            }
            else if (msg[0]==NRF_CTRL_CMD_ADJP)//调参
            {
              #if BSP_SBV_PARAMADJ_ON
              CTRL_S_DECODE_ADJParam(msg);
              #endif //#if BSP_SBV_PARAMADJ_ON
            }
        }
    }

}


/**
  * @brief   获取时间戳/ms
  * @param   
  * @retval  
**/
uint32_t NRFCTR_GetTime(void) 
{
	uint32_t count;
	get_tick_count(&count);
	return count;
}



/**
  * @brief   发送数据，第一字节为指令，第二字节为有效数据长度，后面每5个字节为一组数据
  * @param   TXMsg[IN] : 发送的数据
  * @param   RXMsg[OUT] : 接收的数据
  * @retval  0：收发成功，NRF_ERRCODE_RX_Err：发送成功，无接收数据
**/
uint8_t NRF_CTRL_SendMsg(uint8_t* TXMsg, uint8_t* RXMsg)
{

  NRF_CTRL_PrintMsg(TXMsg);
  return NRF24L01_TxPacketWithAckData(TXMsg, RXMsg, NRF_CTRL_MsgTimeOut, &G_NRF_CTRL_NRF_HardStruct);
  //return NRF24L01_TxPacket(TXMsg, NRF_CTRL_MsgTimeOut, &G_NRF_CTRL_NRF_HardStruct);
}


/**
  * @brief   接收发来的数据并处理，单次检测，无阻塞
  * @param   TXMsg[IN] : 下次接收时要发送的数据
  * @param   RXMsg[OUT]: 接收的数据，固定32Bytes
  * @retval  0表示成功
**/
uint8_t NRF_CTRL_RecvMsg(uint8_t* TXMsg, uint8_t* RXMsg)
{
  uint8_t tmp=0;
  //接收测试数据
  tmp = NRF24L01_RxPacketWithAckData(TXMsg, RXMsg, 1, &G_NRF_CTRL_NRF_HardStruct);
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


/**
  * @brief   输出收到的数据
  * @param   recvM[IN]: 接收的数据，固定32Bytes
  * @retval  
**/
void NRF_CTRL_M_CopeRecv(uint8_t* recvM)
{
    uint8_t cmd = recvM[0];
    uint8_t msgLen = recvM[1];
    char* tmpstr = (char*)(&recvM[2]);
    NRF_CTRL_DEBUG("msgLen:%d\n",msgLen);
    if ((msgLen>0) && (cmd==NRF_CTRL_CMD_CAHR))
    {
        //收到字符串
        
        recvM[WRX_PAYLOAD_WIDTH-1] = '\0';
        NRF_CTRL_DEBUG("%s\n",tmpstr);
      
        
    }
    
    


}




