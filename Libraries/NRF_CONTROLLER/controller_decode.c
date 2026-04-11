#include "controller_decode.h"
#include "nrf_controller.h"
#include "bsp_sbv.h"


extern BSP_TWSBV_Typedef G_CTRL_TWSBV_Struct;//小车对象
extern uint8_t G_NRF_CTRL_S_ACKMsg[WRX_PAYLOAD_WIDTH];//被控端ACK数据
extern NRF24L01P_Hard_Typedef G_NRF_CTRL_NRF_HardStruct;//遥控器使用的硬件对象
/**
  * @brief   主控端按键信息处理，根据按键标识，做出相应处理，输出处理后的MSG数据。
  * 每个按键数据打包5个字节，第一字节为按键索引(初始化输入的序号，组合键排按钮后面)，
  * 按钮数据第2、3字节为0，第4、5字节为对应key1的回调标识
  * 电位计数据第2、3字节为前16bitFIFO，第4、5字节为电位计解码后的控制数值(int16_t)
  * @param   Keys[IN] : 接受到的按键数据
  * @param   keys_num[IN] : 接受到的按键数据个数
  * @param   Msg[IN/OUT] : 输出的数据, 固定32个uint8_t
  * @retval  已处理的Keys个数
**/
uint8_t CTRL_M_DECODE_CopeKeyFIFO(KEY_FIFO_t* Keys, uint8_t keys_num, uint8_t* Msg)
{
    uint8_t ret_num = 0;
    
    KEY_FIFO_t tmpFlag;
    uint8_t tmpMsg[5];
    uint8_t potIndex;
    int16_t potData;
    uint8_t tmpCount = (keys_num > NRF_CTRL_MAXMsgPerPackage) ? NRF_CTRL_MAXMsgPerPackage : keys_num;
    for (uint8_t i = 0; i < tmpCount; i++)
    {
        tmpFlag = Keys[i];
        if (tmpFlag&KEY_POT_FIFO_FLAG )
        {
            //电位计按键
            ret_num++;
            potIndex = (tmpFlag&KEY_POT_FIFO_I_MASK)>>KEY_POT_FIFO_I_BIT_S;
            potData = (int16_t)(tmpFlag & KEY_POT_FIFO_MASK);
            tmpMsg[0] = potIndex;
            tmpMsg[1] = (tmpFlag&0xff000000)>>24;
            tmpMsg[2] = (tmpFlag&0xff0000)>>16;
            tmpMsg[3] = (potData&0xff00)>>8;
            tmpMsg[4] = potData&0xff;
            CTRL_DECODE_PackMSG(ret_num,  tmpMsg,  Msg);
        }
        else 
        {
            //按钮
            ret_num++;
            uint16_t flag16 = tmpFlag & KEY_BUTTON_FIFO_MASK;
            
            potIndex = flag16/KEY_FLAG_NUM;
            uint8_t data8 = flag16%KEY_FLAG_NUM;

            tmpMsg[0] = potIndex;
            tmpMsg[1] = 0;
            tmpMsg[2] = 0;
            tmpMsg[3] = 0;
            tmpMsg[4] = data8;
            CTRL_DECODE_PackMSG(ret_num,  tmpMsg,  Msg); 
        }
    }

    return ret_num;
}


/**
  * @brief   被控端按键信息处理，根据按键标识，做出相应处理。
  * @param   Msg[IN] : 输入的数据, 固定32个uint8_t
  * @retval  
**/
uint8_t CTRL_S_DECODE_CopeKeyFIFO(uint8_t* Msg)
{
    uint8_t ret_num = 0;
    uint8_t cmd = Msg[0];
    uint8_t keys_num = Msg[1];
    
    uint8_t potIndex;
    int16_t potData=0;
    int32_t potData32=0;

    NRF_CTRL_DEBUG("cmd:%d, keys_num:%d", cmd, keys_num);
    keys_num = (keys_num > NRF_CTRL_MAXMsgPerPackage) ? NRF_CTRL_MAXMsgPerPackage : keys_num;
    for (uint8_t i = 0; i < keys_num; i++)
    {
        if (Msg[3+NRF_CTRL_MsgBytes*i]|Msg[4+NRF_CTRL_MsgBytes*i])
        {
            //电位计
            potData=0;
            potData32=0;
            potIndex = Msg[2+NRF_CTRL_MsgBytes*i];
            potData |= (uint16_t)(Msg[2+3+NRF_CTRL_MsgBytes*i])<<8;
            potData |= (uint16_t)(Msg[2+4+NRF_CTRL_MsgBytes*i]);
            potData32 = (int32_t)potData;
            
            NRF_CTRL_DEBUG("pot:%d data:%d", potIndex, potData);
            NRF_CTRL_DEBUG("pot:%d potData32:%ld", potIndex, potData32);

            #if NRF_CTRL_ON
            if (G_CTRL_TWSBV_Struct.Ctroller_ON)
            {         
                if (potIndex==0)
                {
                    G_CTRL_TWSBV_Struct.TurnSpeed = potData32/CTRL_DECODE_POT_TurnDataRatio;
                }
                else if (potIndex==3)
                {
                    G_CTRL_TWSBV_Struct.TargetSpeed = potData32/CTRL_DECODE_POT_TargetDataRatio;
                }
            }
            #endif //NRF_CTRL_ON

        }
        else
        {
            //按钮
            uint8_t KeyIndex = Msg[2+NRF_CTRL_MsgBytes*i];
            uint8_t KeyData = Msg[6+NRF_CTRL_MsgBytes*i];

            if (KeyIndex==3)
            {
                if (KeyData==Key1_LP)//按钮3长按 启停小车
                {
                    #if NRF_CTRL_ON
                    if (G_CTRL_TWSBV_Struct.CAR_ON)
                    {
                        G_CTRL_TWSBV_Struct.CAR_ON = 0;
                        //NRF_CTRL_DEBUG("stop ctrl car...");
                        G_NRF_CTRL_S_ACKMsg[0] = NRF_CTRL_CMD_CAHR;
                        G_NRF_CTRL_S_ACKMsg[1] = snprintf((char*)(&G_NRF_CTRL_S_ACKMsg[2]), WRX_PAYLOAD_WIDTH-2, "Car_Off");
                        G_NRF_CTRL_NRF_HardStruct.NRF_S_ACKMsg = 1;
                    }
                    else
                    {
                        G_CTRL_TWSBV_Struct.CAR_ON = 1;
                        //NRF_CTRL_DEBUG("start ctrl car...");
                        G_NRF_CTRL_S_ACKMsg[0] = NRF_CTRL_CMD_CAHR;
                        G_NRF_CTRL_S_ACKMsg[1] = snprintf((char*)(&G_NRF_CTRL_S_ACKMsg[2]), WRX_PAYLOAD_WIDTH-2, "Car_On");
                        G_NRF_CTRL_NRF_HardStruct.NRF_S_ACKMsg = 1;
                    }
                    #endif //NRF_CTRL_ON
                }
                else if (KeyData==Key1_DPR)//按钮3双击 启停遥控
                {
                    if (G_CTRL_TWSBV_Struct.Ctroller_ON)
                    {
                        G_CTRL_TWSBV_Struct.Ctroller_ON = 0;
                        G_NRF_CTRL_S_ACKMsg[0] = NRF_CTRL_CMD_CAHR;
                        G_NRF_CTRL_S_ACKMsg[1] = snprintf((char*)(&G_NRF_CTRL_S_ACKMsg[2]), WRX_PAYLOAD_WIDTH-2, "Ctroller_OFF");
                        G_NRF_CTRL_NRF_HardStruct.NRF_S_ACKMsg = 1;
                    }
                    else
                    {
                        G_CTRL_TWSBV_Struct.Ctroller_ON = 1;
                        G_NRF_CTRL_S_ACKMsg[0] = NRF_CTRL_CMD_CAHR;
                        G_NRF_CTRL_S_ACKMsg[1] = snprintf((char*)(&G_NRF_CTRL_S_ACKMsg[2]), WRX_PAYLOAD_WIDTH-2, "Ctroller_ON");
                        G_NRF_CTRL_NRF_HardStruct.NRF_S_ACKMsg = 1;
                    }
                }
                
            }
            
        }
    }

    return ret_num;
}



#if BSP_SBV_PARAMADJ_ON
/**
  * @brief   配控端参数调节，持续读取按键信息，直至再次读取到参数调节指令退出
  * @param   Msg[IN] : 输入的数据, 固定32个uint8_t
  * @retval  
**/
void CTRL_S_DECODE_ADJParam(uint8_t *Msg)
{
    if ((Msg[0]==NRF_CTRL_CMD_ADJP)&&(Msg[1]==5))
    {
        for (uint8_t i = 0; i < 5; i++)
        {
            if(Msg[2+i]!=NRF_CTRL_CMD_ADJP)
            {
                return;
            }
        }
    }
    else
    {
        return;
    }

    //开始接收指令调节参数
    NRF_CTRL_DEBUG("adjp...",);
    G_NRF_CTRL_NRF_HardStruct.NRF_S_ACKMsg = 1;
    G_NRF_CTRL_S_ACKMsg[0] = NRF_CTRL_CMD_CAHR;
    G_NRF_CTRL_S_ACKMsg[1] = snprintf((char*)(&G_NRF_CTRL_S_ACKMsg[2]), WRX_PAYLOAD_WIDTH-2, "adjp...%.5f",G_CTRL_TWSBV_Struct.KP_vert);
    float step = 1.0;//指示当前步距
    uint8_t ParamFlag = 0;//指示当前所调参数,0-7分别为 直立环KP/KD、速度环KP/KI、转向环KP/KD、中值、转向偏置
    uint8_t ParamFlagMAX = 7;//
    float tmp_Param = 0;//参数

    uint32_t adjPeriod = 1000;//每1000ms只修改一次
    uint32_t lastAdjTime = 0;
    uint32_t nowTime = NRFCTR_GetTime();
    while (1)
    {
        uint8_t recvmsg[WRX_PAYLOAD_WIDTH];
        uint8_t rVal=0;
        if (G_NRF_CTRL_NRF_HardStruct.NRF_S_ACKMsg)//有数据待返回
        {
          rVal = NRF_CTRL_RecvMsg(G_NRF_CTRL_S_ACKMsg, recvmsg);
          G_NRF_CTRL_NRF_HardStruct.NRF_S_ACKMsg = 0;
          NRF_CTRL_DEBUG("%s",(char*)(&G_NRF_CTRL_S_ACKMsg[2]));
        }
        else
        {
          rVal = NRF_CTRL_RecvMsg(NULL, recvmsg);
        } 
        if (!rVal)
        {
            //接收到数据 处理数据
            //NRF_CTRL_PrintMsg(recvmsg);
            if (recvmsg[0]==NRF_CTRL_CMD_KEY)//按键数据
            {
                uint8_t cmd = recvmsg[0];
                uint8_t keys_num = recvmsg[1];
                uint8_t potIndex;
                int16_t potData=0;
                int32_t potData32=0;
                NRF_CTRL_DEBUG("cmd:%d, keys_num:%d", cmd, keys_num);
                keys_num = (keys_num > NRF_CTRL_MAXMsgPerPackage) ? NRF_CTRL_MAXMsgPerPackage : keys_num;
                for (uint8_t i = 0; i < keys_num; i++)
                {
                    tmp_Param = 0;
                    nowTime = NRFCTR_GetTime();
                    if (nowTime - lastAdjTime < adjPeriod)
                    {
                        continue;
                    }
                    
                    
                    if (recvmsg[3+NRF_CTRL_MsgBytes*i]|recvmsg[4+NRF_CTRL_MsgBytes*i])
                    {
                        //电位计
                        potData=0;
                        potData32=0;
                        potIndex = recvmsg[2+NRF_CTRL_MsgBytes*i];
                        potData |= (uint16_t)(recvmsg[2+3+NRF_CTRL_MsgBytes*i])<<8;
                        potData |= (uint16_t)(recvmsg[2+4+NRF_CTRL_MsgBytes*i]);
                        potData32 = (int32_t)potData;
                        
                        NRF_CTRL_DEBUG("pot:%d data:%d", potIndex, potData);
                        NRF_CTRL_DEBUG("pot:%d potData32:%ld", potIndex, potData32);

                        if (potIndex==0)//左右调步距
                        {
                            if (potData32>0)
                            {
                                step*=10.0;
                            }
                            else if (potData32<0)
                            {
                                step/=10.0;
                            }
                            lastAdjTime = NRFCTR_GetTime();
                        }
                        else if (potIndex==3)//前后调参数
                        {
                            if (potData32>0)
                            {
                                tmp_Param=step;
                            }
                            else if (potData32<0)
                            {
                                tmp_Param=-step;
                            }
                            lastAdjTime = NRFCTR_GetTime();
                        }
                       
                        break;
                    }
                    else
                    {
                        //按钮
                        uint8_t KeyIndex = recvmsg[2+NRF_CTRL_MsgBytes*i];
                        uint8_t KeyData = recvmsg[6+NRF_CTRL_MsgBytes*i];

                        if (KeyIndex==0)
                        {
                            if (KeyData==Key1_DPR)//按钮0双击 切换参数
                            {
                                ParamFlag++;
                                if (ParamFlag>ParamFlagMAX)
                                {
                                    ParamFlag = 0;
                                }
                                
                            }
                        }
                        else if (KeyIndex==1)//
                        {
                            if (KeyData==Key1_R)//按钮1单击，正向调参
                            {
                                tmp_Param=step;
                            }                  
                        }
                        else if (KeyIndex==2)//
                        {
                            if (KeyData==Key1_R)//按钮2单击，反向调参
                            {
                                tmp_Param=-step;
                            }                  
                        }
                        lastAdjTime = NRFCTR_GetTime();
                        break;
                    }
                }
                uint8_t ackLen=0;
                G_NRF_CTRL_NRF_HardStruct.NRF_S_ACKMsg = 1;
                G_NRF_CTRL_S_ACKMsg[0] = NRF_CTRL_CMD_CAHR;
                char* tmpMSG = ((char*)&G_NRF_CTRL_S_ACKMsg[2]);
                switch (ParamFlag)
                {
                case 0://直立环KP
                {
                    G_CTRL_TWSBV_Struct.KP_vert += tmp_Param;
                    ackLen = snprintf((char*)tmpMSG, WRX_PAYLOAD_WIDTH-2, "KP_vert:%.5f,step:%.5f", G_CTRL_TWSBV_Struct.KP_vert,step);
                }break;
                case 1://直立环KD
                {
                    G_CTRL_TWSBV_Struct.KD_vert += tmp_Param;
                    ackLen = snprintf((char*)tmpMSG, WRX_PAYLOAD_WIDTH-2, "KD_vert:%.5f,step:%.5f", G_CTRL_TWSBV_Struct.KD_vert,step);
                }break;
                case 2://速度环KP
                {
                    G_CTRL_TWSBV_Struct.KP_velo += tmp_Param;
                    ackLen = snprintf((char*)tmpMSG, WRX_PAYLOAD_WIDTH-2, "KP_velo:%.5f,step:%.5f", G_CTRL_TWSBV_Struct.KP_velo,step);
                }break;
                case 3://速度环KI
                {
                    G_CTRL_TWSBV_Struct.KI_velo += tmp_Param;
                    ackLen = snprintf((char*)tmpMSG, WRX_PAYLOAD_WIDTH-2, "KI_velo:%.5f,step:%.5f", G_CTRL_TWSBV_Struct.KI_velo,step);
                }break;
                case 4://转向环KP
                {
                    G_CTRL_TWSBV_Struct.KP_turn += tmp_Param;
                    ackLen = snprintf((char*)tmpMSG, WRX_PAYLOAD_WIDTH-2, "KP_turn:%.5f,step:%.5f", G_CTRL_TWSBV_Struct.KP_turn,step);
                }break;
                case 5://转向环KD
                {
                    G_CTRL_TWSBV_Struct.KD_turn += tmp_Param;
                    ackLen = snprintf((char*)tmpMSG, WRX_PAYLOAD_WIDTH-2, "KD_turn:%.5f,step:%.5f", G_CTRL_TWSBV_Struct.KD_turn,step);
                }break;
                case 6://位置环KD
                {
                    G_CTRL_TWSBV_Struct.KD_pos += tmp_Param;
                    ackLen = snprintf((char*)tmpMSG, WRX_PAYLOAD_WIDTH-2, "KD_pos:%.5f,step:%.5f", G_CTRL_TWSBV_Struct.KD_pos,step);
                }break;
                case 7://位置环KI
                {
                    G_CTRL_TWSBV_Struct.KI_pos += tmp_Param;
                    ackLen = snprintf((char*)tmpMSG, WRX_PAYLOAD_WIDTH-2, "KI_pos:%.5f,step:%.5f", G_CTRL_TWSBV_Struct.KI_pos,step);
                }break;
                case 8://中值
                {
                    G_CTRL_TWSBV_Struct.Med += tmp_Param;
                    ackLen = snprintf((char*)tmpMSG, WRX_PAYLOAD_WIDTH-2, "Med:%.5f,step:%.5f", G_CTRL_TWSBV_Struct.Med,step);
                }break;
                case 9://转向偏置
                {
                    G_CTRL_TWSBV_Struct.TurnErr += tmp_Param;
                    ackLen = snprintf((char*)tmpMSG, WRX_PAYLOAD_WIDTH-2, "TurnErr:%d,step:%.5f", G_CTRL_TWSBV_Struct.TurnErr,step);
                }break;
            
                default:
                    {
                        G_NRF_CTRL_NRF_HardStruct.NRF_S_ACKMsg = 0;
                    }break;
                }
                G_NRF_CTRL_S_ACKMsg[WRX_PAYLOAD_WIDTH-1] = '\0';
                G_NRF_CTRL_S_ACKMsg[1] = ackLen;
                NRF_CTRL_DEBUG("ackLen:%d",ackLen);
                NRF_CTRL_DEBUG("G_NRF_CTRL_S_ACKMsg[0]:%d",G_NRF_CTRL_S_ACKMsg[0]);
                NRF_CTRL_DEBUG("NRF_S_ACKMsg:%d",G_NRF_CTRL_NRF_HardStruct.NRF_S_ACKMsg);
                NRF_CTRL_DEBUG("%s",(char*)(&G_NRF_CTRL_S_ACKMsg[2]));
               
            }
            else if (recvmsg[0]==NRF_CTRL_CMD_ADJP)//调参完成
			{
                NRF_CTRL_DEBUG("adjp ok");
                CTRL_SaveSBVParam();//保存参数
                G_NRF_CTRL_NRF_HardStruct.NRF_S_ACKMsg = 1;
                G_NRF_CTRL_S_ACKMsg[0] = NRF_CTRL_CMD_CAHR;
                G_NRF_CTRL_S_ACKMsg[1] = snprintf((char*)(&G_NRF_CTRL_S_ACKMsg[2]), WRX_PAYLOAD_WIDTH-2, "adjp ok");
				return;
			}
        }

    }
}

#endif //#if BSP_SBV_PARAMADJ_ON


/**
  * @brief   打包数据，第一字节为指令，第二字节为有效数据长度，后面每5个字节为一组数据
  * @param   index : 该数据为一包数据中的第几组 最大为6
  * @param   Msg[IN] : 待打包的5Bytes数据
  * @param   Package[IN/OUT] : 32Bytes数据起始地址
  * @retval  
**/
void CTRL_DECODE_PackMSG(uint8_t index, uint8_t* Msg, uint8_t* Package)
{
    if (index > NRF_CTRL_MAXMsgPerPackage)
    {
        return;
    }
    uint8_t indexBias = NRF_CTRL_MsgBytes*(index-1) + 2;
    for (uint8_t i = 0; i < NRF_CTRL_MsgBytes; i++)
    {
        Package[indexBias+i] = Msg[i];
    }
        
}


void CTRL_DECODE_PackSBVACKMsg(void)
{


}



#if 0

/**
  * @brief   修改小车两轮速度(百分比)
  * @param   Lw : 左轮速度
  * @param   Rw : 右轮速度
  * @retval  
**/
void CTRL_DECODE_ModifyWheel(int32_t Lw, int32_t Rw)
{
#if NRF_CTRL_USE_KEYIOSCAN
    CTRL_DECODE_LeftWheel_Speed += Lw;
    CTRL_DECODE_RightWheel_Speed += Rw;
#endif //NRF_CTRL_USE_KEYIOSCAN

#if NRF_CTRL_USE_ADC
    CTRL_DECODE_LeftWheel_Speed = Lw;
    CTRL_DECODE_RightWheel_Speed = Rw;
#endif //#if NRF_CTRL_USE_ADC


    return;
}


/**
  * @brief   小车两轮速度归零
  * @param   
  * @retval  
**/
void CTRL_DECODE_ResetWheelSpeed(void)
{
    CTRL_DECODE_LeftWheel_Speed = 0;
    CTRL_DECODE_RightWheel_Speed = 0;
    return;
}


/**
  * @brief   获取小车两轮速度（百分比）
  * @param   Lw : 左轮速度
  * @param   Rw : 右轮速度
  * @retval  
**/
void CTRL_DECODE_GetWheelSpeed(int32_t *Lw, int32_t *Rw)
{

    *Lw = CTRL_DECODE_LeftWheel_Speed;
    *Rw = CTRL_DECODE_RightWheel_Speed;

    return;
}


/**
  * @brief   重置虚拟按键
  * @param   
  * @retval  
**/
void CTRL_DECODE_ResetVirtualKey(void)
{
    SG_CTRL_DECODE_VirtualKey = 0;
    return;
}


/**
  * @brief   读取并重置虚拟按键
  * @param   
  * @retval  
**/
void CTRL_DECODE_GetVirtualKey(uint8_t *VKey)
{
    *VKey = SG_CTRL_DECODE_VirtualKey;
    SG_CTRL_DECODE_VirtualKey = 0;
    return;
}




#endif
