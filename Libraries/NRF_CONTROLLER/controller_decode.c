#include "controller_decode.h"

#if 0
/*小车两轮速度，百分比包含符号*/
static int32_t CTRL_DECODE_LeftWheel_Speed;//左轮速度 
static int32_t CTRL_DECODE_RightWheel_Speed;//右轮速度 
static uint8_t SG_CTRL_DECODE_VirtualKey;//虚拟键值
static uint8_t SG_CTRL_DECODE_DistanceDetectFlag=0;//距离检测开启状态


extern uint8_t NRF_CTRL_ConnectionDetect(void);

/**
  * @brief   按键信息处理，检测被按下的按键，做出相应处理。
  * @param   Keys : 接受到的按键数据
  * @param   keys_num : 接受到的按键数据个数
  * @retval  
**/
void CTRL_DECODE_CopeCmdKeyMsg(uint8_t* Keys, uint8_t keys_num)
{
#if NRF_CTRL_USE_ADC
    int32_t lw;
    int32_t rw;
    int32_t tlw;
    int32_t trw;
    int tmp_p;
#endif //NRF_CTRL_USE_ADC
    for (uint8_t i = 0; i < keys_num; i++)
    {

        if (Keys[i] == CTRL_DECODE_Key_M1M2_P)
        {
            /* 中键单击 */
            CTRL_DECODE_ResetWheelSpeed();
            SG_CTRL_DECODE_VirtualKey |= CTRL_DECODE_Key_UPDATAWHEEL;
            continue;
        }
        else if (Keys[i] == CTRL_DECODE_Key_M1M2_LP)
        {
            /* 中键长按 */
            SG_CTRL_DECODE_VirtualKey |= CTRL_DECODE_Key_CONNDET;
            continue;
        }
        else if (Keys[i] == CTRL_DECODE_Key_M1M2_DP)
        {
            /* 中键双击 */
            if (SG_CTRL_DECODE_DistanceDetectFlag)
            {
                SG_CTRL_DECODE_VirtualKey |= CTRL_DECODE_Key_DISTANCEDETOFF;
                SG_CTRL_DECODE_DistanceDetectFlag = 0;
            }
            else
            {
                SG_CTRL_DECODE_VirtualKey |= CTRL_DECODE_Key_DISTANCEDETON;
                SG_CTRL_DECODE_DistanceDetectFlag = 1;
            }
            continue;
        }
        else
        {
#if NRF_CTRL_USE_ADC


            /* 遍历所有按键 */
            switch (Keys[i]/KEY_FIFOFLAG_NUM)
            {
            case CTRL_DECODE_KeyPot1_LP:
            {
                /* 左侧左右 */

            }break;
            case CTRL_DECODE_KeyPot2_LP:
            {
                /* 左侧上下 */
                KeyPot_CalPercentage(&tmp_p, KeyPot_FindByFlag(Keys[i]));
                tmp_p/=CTRL_DECODE_SpeedLimit;
                lw = (int32_t)tmp_p;
                rw = (int32_t)tmp_p;
                CTRL_DECODE_ModifyWheel(lw, rw);
            }break;
            case CTRL_DECODE_KeyPot3_LP:
            {
                /* 右侧左右 */
                KeyPot_CalPercentage(&tmp_p, KeyPot_FindByFlag(Keys[i]));
                tmp_p/=CTRL_DECODE_DIRSpeedLimit;
                tlw = (int32_t)(-tmp_p);
                trw = (int32_t)tmp_p;
                CTRL_DECODE_ModifyWheel(lw, rw);
            }break;
            case CTRL_DECODE_KeyPot4_LP:
            {
                /* 右侧上下 */

            }break;
#endif //NRF_CTRL_USE_ADC

#if NRF_CTRL_USE_KEYIOSCAN
            case CTRL_DECODE_Key_UP_PLP:
            {
                /* 上 */
                CTRL_DECODE_ModifyWheel(CTRL_DECODE_SpeedIncrement, CTRL_DECODE_SpeedIncrement);
                SG_CTRL_DECODE_VirtualKey |= CTRL_DECODE_Key_UPDATAWHEEL;
            }break;
            case CTRL_DECODE_Key_DOWN_PLP:
            {
                /* 下 */
                CTRL_DECODE_ModifyWheel(-1*CTRL_DECODE_SpeedIncrement, -1*CTRL_DECODE_SpeedIncrement);
                SG_CTRL_DECODE_VirtualKey |= CTRL_DECODE_Key_UPDATAWHEEL;
            }break;
            case CTRL_DECODE_Key_LEFT_PLP:
            {
                /* 左 */
                CTRL_DECODE_ModifyWheel(-1*CTRL_DECODE_SpeedIncrement, CTRL_DECODE_SpeedIncrement);
                SG_CTRL_DECODE_VirtualKey |= CTRL_DECODE_Key_UPDATAWHEEL;
            }break;
            case CTRL_DECODE_Key_RIGHT_PLP:
            {
                /* 右 */
                CTRL_DECODE_ModifyWheel(CTRL_DECODE_SpeedIncrement, -1*CTRL_DECODE_SpeedIncrement);
                SG_CTRL_DECODE_VirtualKey |= CTRL_DECODE_Key_UPDATAWHEEL;
            }break;
            case CTRL_DECODE_Key_LU_PLP:
            {
                /* 左上 */
                CTRL_DECODE_ModifyWheel(CTRL_DECODE_SpeedIncrement, 0);
                SG_CTRL_DECODE_VirtualKey |= CTRL_DECODE_Key_UPDATAWHEEL;
            }break;
            case CTRL_DECODE_Key_LD_PLP:
            {
                /* 左下 */
                CTRL_DECODE_ModifyWheel(-1*CTRL_DECODE_SpeedIncrement, 0);
                SG_CTRL_DECODE_VirtualKey |= CTRL_DECODE_Key_UPDATAWHEEL;
            }break;
            case CTRL_DECODE_Key_RU_PLP:
            {
                /* 右上 */
                CTRL_DECODE_ModifyWheel(0, CTRL_DECODE_SpeedIncrement);
                SG_CTRL_DECODE_VirtualKey |= CTRL_DECODE_Key_UPDATAWHEEL;
            }break;
            case CTRL_DECODE_Key_RD_PLP:
            {
                /* 右下 */
                CTRL_DECODE_ModifyWheel(0, -1*CTRL_DECODE_SpeedIncrement);
                SG_CTRL_DECODE_VirtualKey |= CTRL_DECODE_Key_UPDATAWHEEL;
            }break;
#endif //NRF_CTRL_USE_KEYIOSCAN
            default:
                break;
            }
#if NRF_CTRL_USE_ADC
            lw += tlw;
            rw += trw;
            CTRL_DECODE_ModifyWheel(lw, rw);
#endif //NRF_CTRL_USE_ADC
        }
        
    }



    return;
}

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
