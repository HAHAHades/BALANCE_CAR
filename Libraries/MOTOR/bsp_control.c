#include "bsp_control.h"//板级支持包board support package\
                      仅支持某一个或一批同样的开发板
#include "bsp_SysTick.h"  		  
#include "bsp_520Motor.h"
#include "bsp_encoder.h"


static float KP_ver 	= 420; //700; //直立环比例
static float KD_ver 	= 36; //60直立环微分
static float KP_vel 	= -0.57; //速度环比例
static float KI_vel 	= -0.57/200.0; //速度环积分
#define S_LIMIT 1000 //积分限幅

static float KP_turn 	= 20; //转向环比例
static float KD_turn 	= 0.8; //转向环微分
static float Med = -3.0; //机械中值 

#define PWM_LIMIT 100000 //PWM限幅，占空比*100

int32_t G_BSPCTRL_TargetSpeed = 0;//前后速度
int32_t G_BSPCTRL_TurnSpeed = 0;//转向速度，正为左转，负为右转
static int SG_BSPCTRL_TrunErr = 80; //转向偏置，中和硬件缺陷



/**
	输出 = KP*偏差 + KD*偏差微分
	偏差微分 = (本次偏差 - 上次偏差)/单位时间
	E_angle = tatget_angle - real_angle
	E_angle_last = tatget_angle_last - real_angle_last
	out = KP_ver*E_angle + KD_ver*(E_angle - E_angle_last);
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
	
	out = KP_ver*(tatget_angle - real_angle) + KD_ver*(tatget_angle-tatget_angle_last - gyro_X);
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
	
	out = KP_vel*Espeed +  KI_vel*S_Espeed;
	
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
	
	return KP_turn*turn_speed + KD_turn*gyro_Z;
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
	Vertical_out = Vertical_Loop( Velocity_out+Med,  real_angle,  gyro_X);
	Turn_out = Turn_Loop(target_turn , gyro_Z);
	
	PWM_A = Vertical_out - Turn_out;
	PWM_B = Vertical_out + Turn_out + SG_BSPCTRL_TrunErr;
	
	
	//控制电机
	uint8_t Dir_A = pwm_limit_abs(&PWM_A);
	uint8_t Dir_B = pwm_limit_abs(&PWM_B);
	BSP_520Motor_Rotation( Motor_520_A,  Dir_A,  PWM_A/100.0);
	BSP_520Motor_Rotation( Motor_520_B,  Dir_B,  PWM_B/100.0);
	
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
	
	/* 接收到中断信息后继续往下 */
	if (!hal.new_gyro)
	{
		return;
	}
	
    unsigned char new_temp = 0;
    unsigned long timestamp;

	unsigned long sensor_timestamp;
	int new_data = 0;
		
	/* 每过500ms读取一次温度 */
	get_tick_count(&timestamp);
	if (timestamp > hal.next_temp_ms)
	{
		hal.next_temp_ms = timestamp + TEMP_READ_MS;
		new_temp = 1;
	}

	/* 接收到新数据 并且 开启DMP */
	if (hal.new_gyro && hal.dmp_on)
	{
		short gyro[3], accel_short[3], sensors;
		unsigned char more;
		long accel[3], quat[4], temperature;
		/* 当DMP正在使用时，该函数从FIFO获取新数据 */
		dmp_read_fifo(gyro, accel_short, quat, &sensor_timestamp, &sensors, &more);
		/* 如果more=0，则数据被获取完毕 */
		if (!more)
			hal.new_gyro = 0;

		/* 读取相对应的新数据，推入MPL */
		//四元数数据
		if (sensors & INV_WXYZ_QUAT)
		{
			inv_build_quat(quat, 0, sensor_timestamp);
			new_data = 1;
		}

		//陀螺仪数据
		if (sensors & INV_XYZ_GYRO)
		{
			inv_build_gyro(gyro, sensor_timestamp);
			new_data = 1;
			if (new_temp)
			{
				new_temp = 0;
				/* 获取温度，仅用于陀螺温度比较 */
				mpu_get_temperature(&temperature, &sensor_timestamp);
				inv_build_temp(temperature, sensor_timestamp);
			}
		}
		
		//加速度计数据
		if (sensors & INV_XYZ_ACCEL)
		{
			accel[0] = (long)accel_short[0];
			accel[1] = (long)accel_short[1];
			accel[2] = (long)accel_short[2];
			inv_build_accel(accel, 0, sensor_timestamp);
			new_data = 1;
		}
	}//end if (hal.new_gyro && hal.dmp_on)

	if (new_data)
	{
		long data[9];
		int8_t accuracy;
		/* 处理接收到的数据 */
		if (inv_execute_on_data())
		{
			//数据错误
			new_data = 0;
			return;
//			#if UART_Printtf
//			printf("数据错误\n");
//			#endif //UART_Printtf
		}
		
		//欧拉角
		if (inv_get_sensor_type_euler(data, &accuracy, (inv_time_t *)&timestamp))
		{
			G_Euler_RPY[0] = data[0] * 1.0 / (1 << 16);
			G_Euler_RPY[1] = data[1] * 1.0 / (1 << 16);
			G_Euler_RPY[2] = data[2] * 1.0 / (1 << 16);
		}
		
		//加速度
		if (inv_get_sensor_type_accel(data, &accuracy, (inv_time_t *)&timestamp))
		{
			G_ACCEL_XYZ[0] = data[0] * 1.0 / (1 << 16);
			G_ACCEL_XYZ[1] = data[1] * 1.0 / (1 << 16);
			G_ACCEL_XYZ[2] = data[2] * 1.0 / (1 << 16);
		}
		
		//角速度
		if (inv_get_sensor_type_gyro(data, &accuracy, (inv_time_t *)&timestamp))
		{
			G_GYRO_XYZ[0] = data[0] * 1.0 / (1 << 16);
			G_GYRO_XYZ[1] = data[1] * 1.0 / (1 << 16);
			G_GYRO_XYZ[2] = data[2] * 1.0 / (1 << 16);
		}		

	}//end if (new_data)


		int16_t Encoder_CountA = Get_Encoder_Count(EncoderA_TIMx);//读取编码器
		int16_t Encoder_CountB = -Get_Encoder_Count(EncoderB_TIMx);
		
		Control_PWM( 0.0,  G_Euler_RPY[0],  G_GYRO_XYZ[0],  G_BSPCTRL_TargetSpeed,  Encoder_CountA,  Encoder_CountB, G_BSPCTRL_TurnSpeed , G_GYRO_XYZ[2]);//控制小车
	
		
}







