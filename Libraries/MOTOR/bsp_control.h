#ifndef _BSP_CONTROL_H
#define _BSP_CONTROL_H

#include "stm32f10x.h"

#include "mpu6050.h"


extern int32_t G_BSPCTRL_TargetSpeed ;//前后速度
extern int32_t G_BSPCTRL_TurnSpeed ;//转向速度，正为左转，负为右转

#define BSP_CTRL_DEBUG_ON 1 //是否调试输出
#define CTRL_SBV_PARAMADJ_ON 1 //是否开启调参功能
#define CTRL_SBV_PARAM_NUM 8 //需要调整的参数个数

//小车结构体
typedef struct 
{
    float KP_vert 	        ; //700; //直立环比例
    float KD_vert 	        ; //60直立环微分
    float KP_velo 	        ; //速度环比例
    float KI_velo 	        ; //速度环积分
    float KP_turn 	        ; //转向环比例
    float KD_turn 	        ; //转向环微分
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
                                      .Med=-3.0,\
                                      .Tilt_Angle_Limit=30,\
                                      .TargetSpeed=0,\
                                      .TurnSpeed=0,\
                                      .TurnErr=80,\
                                      .CAR_ON=1,\
                                      .Ctroller_ON=0,\
                                      .CAR_Tilt=0}

                                   


#define BSP_CTRL_DEBUG(fmt,arg...)          do{\
                                          if(BSP_CTRL_DEBUG_ON)\
                                          UsartPrint("<<-CTRL-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)


                                      

/***************函数申明***************/

void TWSBV_Init(void);
int Vertical_Loop(float tatget_angle, float real_angle, float gyro_Y);
float Velocity_Loop(int tatget_speed, int real_speed);
int Turn_Loop(int32_t turn_speed ,float gyro_Z);
uint8_t pwm_limit_abs(int* data);
void Control_PWM(float tatget_angle, float real_angle, float gyro_Y, int32_t tatget_speed, int encoder_L, int encoder_R, int32_t target_turn, float gyro_Z);
void Control_Car_IRQHandler(void);
void Control_SetCarSta(uint8_t newSta);
uint8_t Control_GetCarSta(void);
void Control_SetCtrollerSta(uint8_t newSta);
uint8_t Control_GetCtrollerSta(void);
void Control_Tilt_Detect(float real_angle);
void CTRL_SaveSBVParam(void);
void CTRL_PrintSBVParam(void);
/*************************************/


#endif //_BSP_CONTROL_H



