#include "controller_decode.h"
#include "nrf_controller.h"

/*小车两轮速度，百分比包含符号*/
// static int32_t CTRL_DECODE_LeftWheel_Speed;//左轮速度 
// static int32_t CTRL_DECODE_RightWheel_Speed;//右轮速度 
// static uint8_t SG_CTRL_DECODE_VirtualKey;//虚拟键值
// static uint8_t SG_CTRL_DECODE_DistanceDetectFlag=0;//距离检测开启状态


extern uint8_t NRF_CTRL_ConnectionDetect(void);
extern int32_t G_BSPCTRL_TargetSpeed ;//前后速度
extern int32_t G_BSPCTRL_TurnSpeed ;//转向速度，正为左转，负为右转


/**
  * @brief   主控端按键信息处理，根据按键标识，做出相应处理，输出处理后的MSG数据。
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
            potData = (int16_t)(tmpFlag & KEY_POT_FIFO_MASK)/CTRL_DECODE_POTDataRatio;
            tmpMsg[0] = potIndex;
    
            tmpMsg[3] = (potData&0xff00)>>8;
            tmpMsg[4] = potData&0xff;
            CTRL_DECODE_PackMSG(ret_num,  tmpMsg,  Msg);
        }
        else if (tmpFlag)
        {
            //按钮
            ret_num++;
            
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
        potData=0;
        potData32=0;
        potIndex = Msg[2+NRF_CTRL_MsgBytes*i];
        potData |= (uint16_t)(Msg[2+3+NRF_CTRL_MsgBytes*i])<<8;
        potData |= (uint16_t)(Msg[2+4+NRF_CTRL_MsgBytes*i]);
        potData32 = (int32_t)potData;
        
        NRF_CTRL_DEBUG("pot:%d data:%d", potIndex, potData);
        NRF_CTRL_DEBUG("pot:%d potData32:%ld", potIndex, potData32);


        #if NRF_CTRL_ON
        if (potIndex==0)
        {
            G_BSPCTRL_TurnSpeed = potData32*10;
        }
        else if (potIndex==3)
        {
            G_BSPCTRL_TargetSpeed = potData32;
        }
        #endif //NRF_CTRL_ON

    }

    return ret_num;
}





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
