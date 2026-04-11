#include "bsp_sbv.h"
#include "bsp_SysTick.h"
#include "mpu6050.h"

#ifdef   soft_IIC //MPU
#include "bsp_soft_i2c.h"
#else
#include "bsp_hard_i2c.h"
#endif

#include "bsp_520Motor.h"
#include "bsp_encoder.h"
#include "bsp_flash.h"
#include "bsp_led.h"
#include "nrf_controller.h"



extern float  G_Euler_RPY[3];//欧拉角
extern float  G_GYRO_XYZ[3];//角速度
extern float  G_ACCEL_XYZ[3];//加速度


//小车参数储存起始位置，放在Flash最后
#define BSP_SBV_ParamBuff_SP ((uint32_t)(FLASH_END_ADDR-(4*BSP_SBV_PARAM_NUM)))
//小车参数依次为 直立环KP/KD、速度环KP/KI、转向环KP/KD、位置环KD/KI、中值、转向偏置
static float SG_SBV_ParamBuff[BSP_SBV_PARAM_NUM] __attribute__ ((at(BSP_SBV_ParamBuff_SP)));

BSP_TWSBV_Typedef G_CTRL_TWSBV_Struct;//小车对象

static NRF_Controller_Run_Typedef SG_SBV_RunStruct = NRF_Controller_Run_TypedefDefaultVal;
static int S_Espeed = 0;//速度偏差的积分
static int Spos = 0;//位置积分
static int last_pos = 0;

#define S_LIMIT 60 //积分限幅
#define PWM_LIMIT 100000 //PWM限幅，占空比*100




void BSP_SBVInit(void)
{

    // MPU6050 IIC 初始化
	
    #ifdef soft_IIC //mpu使用软/硬件IIC
		// 使用了B3 B4等引脚
		// RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		// GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
        i2c_GPIO_Config();
    #else
        MPU_I2C_Config();
    #endif
    
    //初始化MPUINT引脚
    MPU_INT_Init();
    
    //初始化MPU6050
    MPU6050_mpu_init();
    MPU6050_mpl_init();
    MPU6050_config();

    //检测MPU6050
    if(MPU6050ReadID() == 0)
    {
        BSP_SBV_DEBUG("\r\n没有检测到MPU6050传感器！\r\n");
        LED_ON(1); 
        while(1);	
    }

	//NRF遥控器配置
	if (NRF_Controller_Config(NRF_CTRL_SLAVER))
	{
		LED_ON(1);
		BSP_SBV_DEBUG( "NRF_Controller_Config err\n");
		while(1);
	}


	BSP_SBV_ParamInit();//小车初始化参数

	#if BSP_SBV_DEBUG_ON
	CTRL_PrintSBVParam();//输出小车参数
	#endif //BSP_SBV_DEBUG_ON
	//电机初始化
	TIM_DeInit(Motor_520_APWM_TIMx);
	BSP_520Motor_Config( Motor_520_APWM_TIMx,  Motor_520_APWM_TIM_Channel_x, 
						Motor_520_AIN1_PORTx,  Motor_520_AIN1_Pinx, 
						 Motor_520_AIN2_PORTx,  Motor_520_AIN2_Pinx);
	
	BSP_520Motor_Config( Motor_520_BPWM_TIMx,  Motor_520_BPWM_TIM_Channel_x, 
						Motor_520_BIN1_PORTx,  Motor_520_BIN1_Pinx, 
						 Motor_520_BIN2_PORTx,  Motor_520_BIN2_Pinx);
	
	//编码器初始化
	TIMx_CHx_ENCODER_Init( EncoderA_TIMx,  EncoderA_TIMx_ACH_x, EncoderA_TIMx_IO_Reamp ,  BSP_Encoder_DirF); //
	Get_Encoder_Count(EncoderA_TIMx);//计数器清零
	
	TIMx_CHx_ENCODER_Init( EncoderB_TIMx,  EncoderB_TIMx_ACH_x, EncoderB_TIMx_IO_Reamp ,  BSP_Encoder_DirF); //
	Get_Encoder_Count(EncoderB_TIMx);//计数器清零

}


/**
  * @brief   小车参数初始化
  * @param   
  * @retval  
  */
void BSP_SBV_ParamInit(void)
{
	BSP_TWSBV_Typedef tmpStruct = BSP_TWSBV_Typedef_DefaultVal;
	G_CTRL_TWSBV_Struct = tmpStruct;

	// G_CTRL_TWSBV_Struct.KP_vert = SG_SBV_ParamBuff[0];
	// G_CTRL_TWSBV_Struct.KD_vert = SG_SBV_ParamBuff[1];
	// G_CTRL_TWSBV_Struct.KP_velo = SG_SBV_ParamBuff[2];
	// G_CTRL_TWSBV_Struct.KI_velo = SG_SBV_ParamBuff[3];
	// G_CTRL_TWSBV_Struct.KP_turn = SG_SBV_ParamBuff[4];
	// G_CTRL_TWSBV_Struct.KD_turn = SG_SBV_ParamBuff[5];
	// G_CTRL_TWSBV_Struct.KD_pos = SG_SBV_ParamBuff[6];
	// G_CTRL_TWSBV_Struct.KI_pos = SG_SBV_ParamBuff[7];
	// G_CTRL_TWSBV_Struct.Med = SG_SBV_ParamBuff[8];
	// G_CTRL_TWSBV_Struct.TurnErr = SG_SBV_ParamBuff[9];
}



void BSP_SBV_RunOnece(void)
{
	NRF_Controller_Run(&SG_SBV_RunStruct);
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
	
	static int Espeed = 0;//速度偏差
	int out;
	// uint8_t sErr = 0;
	//速度偏差，低通滤波限制速度变化
	Espeed = 0.7*Espeed + 0.3*(tatget_speed - real_speed);//Espeed为上次偏差，(tatget_speed - real_speed)为本次偏差
	//速度偏差超过10倍,停止积分累加
	// if (abs(real_speed)/(abs(tatget_speed)+1)>10)
	// {
	// 	sErr = 1;
	// }
	// else
	// {
	// 	S_Espeed += Espeed;
	// }
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
  * @brief   转向环控制
  * @param   turn_speed ：期望转向速度
  * @param   gyro_Z ：当前偏航角速度
  * @retval  电机PWM占空比*100；符号表示旋转方向
  */
int Turn_Loop(int32_t turn_speed ,float gyro_Z)
{
	
	return G_CTRL_TWSBV_Struct.KP_turn*turn_speed + G_CTRL_TWSBV_Struct.KD_turn*gyro_Z;
}


/**
  * @brief   位置环控制
  * @param   posA ：电机编码器数值
  * @retval  电机速度(速度环输入)
  */
float Position_Loop(int pos)
{
	int VPos;
	int outP;
	VPos = pos - last_pos;
	Spos+= pos;
	outP = G_CTRL_TWSBV_Struct.KD_pos*VPos + G_CTRL_TWSBV_Struct.KI_pos*Spos;
	last_pos = pos;
	return outP;
}


/**
  * @brief   计算PWM，控制电机
  * @param   tatget_angle ：目标倾角角度
  * @param   real_angle(Roll) ：当前倾角角度
  * @param   gyro_X ：当前倾角速度
  * @param   acc_X ：小车正向加速度
  * @param   tatget_speed ：目标速度 单位：正交编码器每10ms的计数值（单脉冲的4倍）； rps轮子转每秒 = 100*tatget_speed/(4 * 11编码器每转单脉冲数*30减速比) tatget_speed=13.2时约1rps
  * @param   encoder_L ：A电机编码器每10ms的正交计数值,前+后-，由于每次读取后清零，所以读取的是相对位置
  * @param   encoder_R ：B电机编码器每10ms的正交计数值,前+后-
  * @param   target_turn ：目标转向速度 正左转 负右转 车轮速度与tatget_speed相同
  * @param   gyro_Z ：当前偏航速度
  * @retval  
  */
void Control_PWM(float tatget_angle, float real_angle, float gyro_X, float acc_X, int32_t tatget_speed, int encoder_L, int encoder_R, int32_t target_turn, float gyro_Z)
{
	int Vertical_out=0, Turn_out=0, PWM_A=0, PWM_B=0;
	float posOut = 0;
	float Velocity_out;
	tatget_angle = tatget_speed/BSP_SBV_SpeedDivAngle;
	
	Control_Tilt_Detect(real_angle);//倾斜检测
	if ((!G_CTRL_TWSBV_Struct.CAR_ON)||G_CTRL_TWSBV_Struct.CAR_Tilt)
	{
		BSP_520Motor_Stop(Motor_520_A|Motor_520_B);
		S_Espeed = 0;//积分累计清零
		G_CTRL_TWSBV_Struct.TargetSpeed = 0;
		G_CTRL_TWSBV_Struct.TurnSpeed = 0;
		Spos= 0;
		last_pos = 0;
		return;
	}
	else if ((abs(tatget_speed)<1)&&(abs(target_turn)<1))
	{
		
		posOut = Position_Loop(encoder_L+encoder_R);
	}
	else
	{
		Spos= 0;
		last_pos = 0;
	}

	Velocity_out = Velocity_Loop(2*tatget_speed, (encoder_L+encoder_R));//encoder_L+encoder_R)/2 表示两轮子中心的速度（每10ms正交计数值）
	Vertical_out = Vertical_Loop( posOut + Velocity_out+G_CTRL_TWSBV_Struct.Med+tatget_angle,  real_angle,  gyro_X);
	Turn_out = Turn_Loop(target_turn , gyro_Z);
	
	PWM_A = Vertical_out - Turn_out ;
	PWM_B = Vertical_out + Turn_out + G_CTRL_TWSBV_Struct.TurnErr;
	
	//控制电机
	uint8_t Dir_A = pwm_limit_abs(&PWM_A);
	uint8_t Dir_B = pwm_limit_abs(&PWM_B);

	BSP_520Motor_Rotation( Motor_520_A,  Dir_A,  PWM_A/100.0);
	BSP_520Motor_Rotation( Motor_520_B,  Dir_B,  PWM_B/100.0);

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
	Control_PWM( 0.0,  G_Euler_RPY[0],  G_GYRO_XYZ[0], G_ACCEL_XYZ[0], G_CTRL_TWSBV_Struct.TargetSpeed,  Encoder_CountA,  Encoder_CountB, G_CTRL_TWSBV_Struct.TurnSpeed , G_GYRO_XYZ[2]);//控制小车
}



#if BSP_SBV_PARAMADJ_ON

/**
  * @brief   将小车参数保存在Flash
  * @param
  * @retval  
  */
void CTRL_SaveSBVParam(void)
{
	float tmpp[BSP_SBV_PARAM_NUM];
	CTRL_PrintSBVParam();
	tmpp[0] = G_CTRL_TWSBV_Struct.KP_vert;
	tmpp[1] = G_CTRL_TWSBV_Struct.KD_vert;
	tmpp[2] = G_CTRL_TWSBV_Struct.KP_velo;
	tmpp[3] = G_CTRL_TWSBV_Struct.KI_velo;
	tmpp[4] = G_CTRL_TWSBV_Struct.KP_turn;
	tmpp[5] = G_CTRL_TWSBV_Struct.KD_turn;
	tmpp[6] = G_CTRL_TWSBV_Struct.KD_pos;
	tmpp[7] = G_CTRL_TWSBV_Struct.KI_pos;
	tmpp[8] = G_CTRL_TWSBV_Struct.Med;
	tmpp[9] = G_CTRL_TWSBV_Struct.TurnErr;

	FlashEraseSegment((uint32_t)SG_SBV_ParamBuff, (uint32_t)(SG_SBV_ParamBuff+BSP_SBV_PARAM_NUM));
	FlashWriteWords( (uint32_t)SG_SBV_ParamBuff,  (uint32_t*)tmpp,  BSP_SBV_PARAM_NUM);
}

#endif //BSP_SBV_PARAMADJ_ON


void CTRL_PrintSBVParam(void)
{
	BSP_SBV_DEBUG("KP_vert:%.5f", SG_SBV_ParamBuff[0]);
	BSP_SBV_DEBUG("KD_vert:%.5f", SG_SBV_ParamBuff[1]);
	BSP_SBV_DEBUG("KP_velo:%.5f", SG_SBV_ParamBuff[2]);
	BSP_SBV_DEBUG("KI_velo:%.5f", SG_SBV_ParamBuff[3]);
	BSP_SBV_DEBUG("KP_turn:%.5f", SG_SBV_ParamBuff[4]);
	BSP_SBV_DEBUG("KD_turn:%.5f", SG_SBV_ParamBuff[5]);
	BSP_SBV_DEBUG("KD_pos:%.5f", SG_SBV_ParamBuff[6]);
	BSP_SBV_DEBUG("KI_pos:%.5f", SG_SBV_ParamBuff[7]);
	BSP_SBV_DEBUG("Med:%.5f", SG_SBV_ParamBuff[8]);
	BSP_SBV_DEBUG("TurnErr:%.5f", SG_SBV_ParamBuff[9]);
}



