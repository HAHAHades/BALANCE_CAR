#include "bsp_control.h"
#include "bsp_SysTick.h"  		  
#include "bsp_520Motor.h"
#include "bsp_encoder.h"
#include "bsp_flash.h"

//小车参数储存位置，放在Flash最后
#define CTRL_SBV_ParamBuff_SP ((uint32_t)(FLASH_END_ADDR-(4*CTRL_SBV_PARAM_NUM)))
//小车参数依次为 直立环KP/KD、速度环KP/KI、转向环KP/KD、中值、转向偏置
static float SG_CTRL_SBV_ParamBuff[CTRL_SBV_PARAM_NUM] __attribute__ ((at(CTRL_SBV_ParamBuff_SP)));

BSP_TWSBV_Typedef G_CTRL_TWSBV_Struct;//小车对象

// static float KP_vert 	= 420; //700; //直立环比例
// static float KD_vert 	= 36; //60直立环微分
// static float KP_velo 	= -0.57; //速度环比例
// static float KI_velo 	= -0.57/200.0; //速度环积分
#define S_LIMIT 1000 //积分限幅

// static float KP_turn 	= 20; //转向环比例
// static float KD_turn 	= 0.8; //转向环微分
// static float Med = -3.0; //机械中值 

#define PWM_LIMIT 100000 //PWM限幅，占空比*100

// static float Tilt_Angle_Limit = 30;//倾斜限制,超出倾角小车停止

// int32_t G_BSPCTRL_TargetSpeed = 0;//前后速度
// int32_t G_BSPCTRL_TurnSpeed = 0;//转向速度，正为左转，负为右转
// static int SG_BSPCTRL_TurnErr = 80; //转向偏置，中和硬件缺陷
// static uint8_t SG_BSP_CTRL_CAR_ON = 0;//小车启动开关
// static uint8_t SG_BSP_CTRL_Ctroller_ON = 0;//遥控启动开关
// static uint8_t SG_BSP_CTRL_CAR_Tilt = 0;//小车倾倒检测结果


/**
  * @brief   小车参数初始化
  * @param   
  * @retval  
  */
void TWSBV_Init(void)
{
	BSP_TWSBV_Typedef tmpStruct = BSP_TWSBV_Typedef_DefaultVal;
	G_CTRL_TWSBV_Struct = tmpStruct;

}


/**
	输出 = KP*偏差 + KD*偏差微分
	偏差微分 = (本次偏差 - 上次偏差)/单位时间
	E_angle = tatget_angle - real_angle
	E_angle_last = tatget_angle_last - real_angle_last
	out = KP_vert*E_angle + KD_vert*(E_angle - E_angle_last);
	gyro_X = (real_angle - real_angle_last)/10ms

  * @brief   直立环PD控制
  * @param   tatget_angle ：目标角度（速度环输出+机械中值）
  * @param   real_angle ：当前角度（小车倾角）
  * @param   gyro_X ：当前角速度
  * @retval  电机PWM占空比*100；符号表示旋转方向
  */

int Vertical_Loop(float tatget_angle, float real_angle, float gyro_X)
{
	int out;
	static float tatget_angle_last = 0;
	
	out = G_CTRL_TWSBV_Struct.KP_vert*(tatget_angle - real_angle) + G_CTRL_TWSBV_Struct.KD_vert*(tatget_angle-tatget_angle_last - gyro_X);
	tatget_angle_last = tatget_angle;
	
	return out;
}

/**
  * @brief   速度环PI控制
  * @param   tatget_speed ：目标速度
  * @param   real_speed ：当前速度
  * @retval  由tatget_speed引起的小车倾角
  */
float Velocity_Loop(int tatget_speed, int real_speed)
{
	static int S_Espeed = 0;//速度偏差的积分
	static int Espeed = 0;//速度偏差
	int out;
	
	//速度偏差，低通滤波限制速度变化
	Espeed = 0.7*Espeed + 0.3*(tatget_speed - real_speed);//Espeed为上次偏差，(tatget_speed - real_speed)为本次偏差
	
	S_Espeed += Espeed;
	
	//积分限幅
	if(S_Espeed>S_LIMIT)
	{
		S_Espeed = S_LIMIT;
	}
	else if (S_Espeed<-S_LIMIT)
	{
		S_Espeed = -S_LIMIT;
	}
	
	out = G_CTRL_TWSBV_Struct.KP_velo*Espeed +  G_CTRL_TWSBV_Struct.KI_velo*S_Espeed;
	
	return out;

}

/**
  * @brief   转向环P控制
  * @param   turn_speed ：期望转向速度
  * @param   gyro_Z ：当前偏航角速度
  * @retval  电机PWM占空比*100；符号表示旋转方向
  */
int Turn_Loop(int32_t turn_speed ,float gyro_Z)
{
	
	return G_CTRL_TWSBV_Struct.KP_turn*turn_speed + G_CTRL_TWSBV_Struct.KD_turn*gyro_Z;
}



/**
  * @brief   计算PWM，控制电机
  * @param   tatget_angle ：目标倾角角度
  * @param   real_angle(Roll) ：当前倾角角度
  * @param   gyro_X ：当前倾角速度
  * @param   tatget_speed ：目标速度 单位：正交编码器每10ms的计数值（单脉冲的4倍）； rps轮子转每秒 = 100*tatget_speed/(4 * 11编码器每转单脉冲数*30减速比) tatget_speed=25时约1rps
  * @param   encoder_L ：A电机编码器每10ms的正交计数值
  * @param   encoder_R ：B电机编码器每10ms的正交计数值
  * @param   target_turn ：目标转向速度 正左转 负右转 车轮速度与tatget_speed相同
  * @param   gyro_Z ：当前偏航速度
  * @retval  
  */
void Control_PWM(float tatget_angle, float real_angle, float gyro_X, int32_t tatget_speed, int encoder_L, int encoder_R, int32_t target_turn, float gyro_Z)
{
	int Vertical_out, Turn_out, PWM_A, PWM_B;
	
	float Velocity_out;
	
	Velocity_out = Velocity_Loop(2*tatget_speed, (encoder_L+encoder_R));//encoder_L+encoder_R)/2 表示两轮子中心的速度（每10ms正交计数值）
	Vertical_out = Vertical_Loop( Velocity_out+G_CTRL_TWSBV_Struct.Med,  real_angle,  gyro_X);
	Turn_out = Turn_Loop(target_turn , gyro_Z);
	
	PWM_A = Vertical_out - Turn_out;
	PWM_B = Vertical_out + Turn_out + G_CTRL_TWSBV_Struct.TurnErr;
	
	//控制电机
	uint8_t Dir_A = pwm_limit_abs(&PWM_A);
	uint8_t Dir_B = pwm_limit_abs(&PWM_B);

	Control_Tilt_Detect(real_angle);//倾斜检测

	if (G_CTRL_TWSBV_Struct.CAR_ON&&(!G_CTRL_TWSBV_Struct.CAR_Tilt))
	{
		BSP_520Motor_Rotation( Motor_520_A,  Dir_A,  PWM_A/100.0);
		BSP_520Motor_Rotation( Motor_520_B,  Dir_B,  PWM_B/100.0);
	}
	else
	{
		BSP_520Motor_Stop(Motor_520_A|Motor_520_B);
	}
	
}


/**
 * @brief   倾斜检测
 * @param	real_angle:当前角度
 * @retval  
 */
void Control_Tilt_Detect(float real_angle)
{
	float tmp = real_angle - G_CTRL_TWSBV_Struct.Med;
	if (tmp<0)
	{
		tmp = -tmp;
	}
	if (tmp>G_CTRL_TWSBV_Struct.Tilt_Angle_Limit)
	{
		G_CTRL_TWSBV_Struct.CAR_Tilt = 1;//小车倾斜
	}
	else
	{
		G_CTRL_TWSBV_Struct.CAR_Tilt = 0;
	}
}


/**
  * @brief   PWM限幅，并返回其绝对值
  * @param[out]   data ：需要判断并取绝对值的数
  * @retval  Motor_520_ReverseRotation ：data<0 ，  Motor_520_ForwardRotation: else
  */
uint8_t pwm_limit_abs(int* data)
{
	uint8_t res = Motor_520_ForwardRotation;
	if(*data<0)
	{
		*data = *data < -PWM_LIMIT ? PWM_LIMIT :  -(*data);
		res = Motor_520_ReverseRotation;
	}
	else
	{
		*data = *data > PWM_LIMIT ? PWM_LIMIT : (*data);
	}
	return res;
}


/**
  * @brief   小车控制的中断服务程序
  * @param
  * @retval  
  */
void Control_Car_IRQHandler(void)
{

	MPU_GetEuler(G_Euler_RPY, G_ACCEL_XYZ, G_GYRO_XYZ);
	int16_t Encoder_CountA = Get_Encoder_Count(EncoderA_TIMx);//读取编码器
	int16_t Encoder_CountB = -Get_Encoder_Count(EncoderB_TIMx);
	
	Control_PWM( 0.0,  G_Euler_RPY[0],  G_GYRO_XYZ[0],  G_CTRL_TWSBV_Struct.TargetSpeed,  Encoder_CountA,  Encoder_CountB, G_CTRL_TWSBV_Struct.TurnSpeed , G_GYRO_XYZ[2]);//控制小车

}


#if CTRL_SBV_PARAMADJ_ON

/**
  * @brief   将小车参数保存在Flash
  * @param
  * @retval  
  */
void CTRL_SaveSBVParam(void)
{

	float tmpp[CTRL_SBV_PARAM_NUM];

	CTRL_PrintSBVParam();

	tmpp[0] = G_CTRL_TWSBV_Struct.KP_vert;
	tmpp[1] = G_CTRL_TWSBV_Struct.KD_vert;
	tmpp[2] = G_CTRL_TWSBV_Struct.KP_velo;
	tmpp[3] = G_CTRL_TWSBV_Struct.KI_velo;
	tmpp[4] = G_CTRL_TWSBV_Struct.KP_turn;
	tmpp[5] = G_CTRL_TWSBV_Struct.KD_turn;
	tmpp[6] = G_CTRL_TWSBV_Struct.Med;
	tmpp[7] = G_CTRL_TWSBV_Struct.TurnErr;


	FlashEraseSegment((uint32_t)SG_CTRL_SBV_ParamBuff, (uint32_t)(SG_CTRL_SBV_ParamBuff+CTRL_SBV_PARAM_NUM));



	FlashWriteWords( (uint32_t)SG_CTRL_SBV_ParamBuff,  (uint32_t*)tmpp,  CTRL_SBV_PARAM_NUM);

}

#endif //CTRL_SBV_PARAMADJ_ON



void CTRL_PrintSBVParam(void)
{
	BSP_CTRL_DEBUG("KP_vert:%.5f", SG_CTRL_SBV_ParamBuff[0]);
	BSP_CTRL_DEBUG("KD_vert:%.5f", SG_CTRL_SBV_ParamBuff[1]);
	BSP_CTRL_DEBUG("KP_velo:%.5f", SG_CTRL_SBV_ParamBuff[2]);
	BSP_CTRL_DEBUG("KI_velo:%.5f", SG_CTRL_SBV_ParamBuff[3]);
	BSP_CTRL_DEBUG("KP_turn:%.5f", SG_CTRL_SBV_ParamBuff[4]);
	BSP_CTRL_DEBUG("KD_turn:%.5f", SG_CTRL_SBV_ParamBuff[5]);
	BSP_CTRL_DEBUG("Med:%.5f", SG_CTRL_SBV_ParamBuff[6]);
	BSP_CTRL_DEBUG("TurnErr:%.5f", SG_CTRL_SBV_ParamBuff[7]);
}

