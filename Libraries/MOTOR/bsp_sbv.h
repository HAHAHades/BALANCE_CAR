#ifndef _BSP_SBV_H
#define _BSP_SBV_H

#include "stm32f10x.h"
#include "bsp_usart.h"

#define BSP_SBV_DEBUG_ON 1 //是否输出调试信息


#define BSP_SBV_PARAMADJ_ON 1 //是否开启调参功能
#define BSP_SBV_PARAM_NUM 10 //需要调整的参数个数

#define CONTROL_CAR_IN_IT  1 // 是否在中断中控制小车，在主函数里控制小车可能导致控制周期不稳定

#define BSP_SBV_SpeedDivAngle 4 

//小车结构体
typedef struct 
{
    float KP_vert 	        ; //700; //直立环比例
    float KD_vert 	        ; //60直立环微分
    float KP_velo 	        ; //速度环比例
    float KI_velo 	        ; //速度环积分
    float KP_turn 	        ; //转向环比例
    float KD_turn 	        ; //转向环微分
    float KD_pos 	        ; //位置环微分
    float KI_pos            ; //位置环积分
    float Med               ; //机械中值 
    float Tilt_Angle_Limit  ; //倾斜限制,超出倾角小车停止
    int32_t TargetSpeed     ; //前后速度
    int32_t TurnSpeed       ; //转向速度，正为左转，负为右转
    int TurnErr             ; //转向偏置，中和硬件缺陷
    uint8_t CAR_ON          ; //小车启动开关
    uint8_t Ctroller_ON     ; //遥控启动开关
    uint8_t CAR_Tilt        ; //小车倾倒检测结果
}BSP_TWSBV_Typedef;


//小车默认参数
#define BSP_TWSBV_Typedef_DefaultVal {.KP_vert=420,\
                                      .KD_vert=36,\
                                      .KP_velo=-0.57,\
                                      .KI_velo=-0.57/200.0,\
                                      .KP_turn=20,\
                                      .KD_turn=0.8,\
                                      .KD_pos=0.5,\
                                      .KI_pos=0.0065,\
                                      .Med=-1.0,\
                                      .Tilt_Angle_Limit=45,\
                                      .TargetSpeed=0,\
                                      .TurnSpeed=0,\
                                      .TurnErr=-50,\
                                      .CAR_ON=1,\
                                      .Ctroller_ON=0,\
                                      .CAR_Tilt=0}

                                   





#define BSP_SBV_DEBUG(fmt,arg...)          do{\
                                          if(BSP_SBV_DEBUG_ON)\
                                          printf("<<-CTRL-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)


                                      

/***************函数申明***************/
void BSP_SBVInit(void);
void BSP_SBV_ParamInit(void);
void BSP_SBV_RunOnece(void);

int Vertical_Loop(float tatget_angle, float real_angle, float gyro_X);
float Velocity_Loop(int tatget_speed, int real_speed);
int Turn_Loop(int32_t turn_speed ,float gyro_Z);
float Position_Loop(int pos);
void Control_PWM(float tatget_angle, float real_angle, float gyro_X, float acc_X, int32_t tatget_speed, int encoder_L, int encoder_R, int32_t target_turn, float gyro_Z);

void Control_Tilt_Detect(float real_angle);
uint8_t pwm_limit_abs(int* data);
void Control_Car_IRQHandler(void);
void CTRL_SaveSBVParam(void);
void CTRL_PrintSBVParam(void);
/*************************************/


#endif //_BSP_SBV_H



