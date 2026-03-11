#include "bsp_520Motor.h"//板级支持包board support package\
                      仅支持某一个或一批同样的开发板
					  		  
#include "bsp_pwm.h"
#include "bsp_gpio.h"



/**
  * @brief   520电机控制端口配置
  * @param   
  *		@arg PWM_TIMx ：电机PWM使用的定时器
  *		@arg PWM_TIM_Channel_x ：电机PWM使用的定时器通道
  *		@arg INy_GPIO_PORTx ：电机正反转输入y端口
  *		@arg INy_GPIO_Pinx ：电机正反转输入y引脚
  * @retval  
  */
void BSP_520Motor_Config(TIM_TypeDef * PWM_TIMx, uint16_t PWM_TIM_Channel_x, 
						GPIO_TypeDef* IN1_GPIO_PORTx, uint16_t IN1_GPIO_Pinx, 
						GPIO_TypeDef* IN2_GPIO_PORTx, uint16_t IN2_GPIO_Pinx)
{
	
	TIMx_CHx_PWM1_Init( PWM_TIMx,  PWM_TIM_Channel_x, 0x00 , 
						Motor_520_PWM_10KHz_Period, Motor_520_PWM_10KHz_Prescaler);//PWM频率10k

	BSP_GPIO_Config(IN1_GPIO_PORTx,  IN1_GPIO_Pinx,  GPIO_Mode_Out_PP);
	BSP_GPIO_Config(IN2_GPIO_PORTx,  IN2_GPIO_Pinx,  GPIO_Mode_Out_PP);
}



/**
  * @brief   520电机正反转
  * @param   
	*	@arg Motor_Num ：电机号，0:A电机 1:B电机
	*	@arg Dir ：旋转方向 0:反转  1:正转
	*	@arg Speed ：旋转速度 : (Speed/100)*100%
  * @retval  
  */
void BSP_520Motor_Rotation(uint8_t Motor_Num, _Bool Dir, float Speed)
{
	if(Motor_Num == Motor_520_A)
	{
		if(Dir)
		{
			GPIO_ResetBits(Motor_520_AIN1_PORTx,  Motor_520_AIN1_Pinx);
			GPIO_SetBits(Motor_520_AIN2_PORTx,  Motor_520_AIN2_Pinx);
		}
		else
		{
			GPIO_SetBits(Motor_520_AIN1_PORTx,  Motor_520_AIN1_Pinx);
			GPIO_ResetBits(Motor_520_AIN2_PORTx,  Motor_520_AIN2_Pinx);
		}
		
		PWM_SetSta(Motor_520_APWM_TIMx, Motor_520_APWM_TIM_Channel_x, 1);
		PWM_SetCompare(Motor_520_APWM_TIMx, Motor_520_APWM_TIM_Channel_x, (uint16_t)((Speed/100)*Motor_520_PWM_10KHz_Period));
		
	}
	else if(Motor_Num == Motor_520_B)
	{
		if(Dir)
		{
			GPIO_ResetBits(Motor_520_BIN1_PORTx,  Motor_520_BIN1_Pinx);
			GPIO_SetBits(Motor_520_BIN2_PORTx,  Motor_520_BIN2_Pinx);
		}
		else
		{
			GPIO_SetBits(Motor_520_BIN1_PORTx,  Motor_520_BIN1_Pinx);
			GPIO_ResetBits(Motor_520_BIN2_PORTx,  Motor_520_BIN2_Pinx);
		}
		
		PWM_SetSta(Motor_520_BPWM_TIMx, Motor_520_BPWM_TIM_Channel_x, 1);
		PWM_SetCompare(Motor_520_BPWM_TIMx, Motor_520_BPWM_TIM_Channel_x, (uint16_t)((Speed/100)*Motor_520_PWM_10KHz_Period));
	}


}

/**
  * @brief   520电机停止
  * @param   
	*	@arg Motor_Num ：电机号，0:A电机 1:B电机
  * @retval  
  */
void BSP_520Motor_Stop(uint8_t Motor_Num)
{
	if(Motor_Num == Motor_520_A)
	{
		GPIO_ResetBits(Motor_520_AIN1_PORTx,  Motor_520_AIN1_Pinx);
		GPIO_ResetBits(Motor_520_AIN2_PORTx,  Motor_520_AIN2_Pinx);
		PWM_SetSta(Motor_520_APWM_TIMx, Motor_520_APWM_TIM_Channel_x, 0);
	}
	else if(Motor_Num == Motor_520_B)
	{
		GPIO_ResetBits(Motor_520_BIN1_PORTx,  Motor_520_BIN1_Pinx);
		GPIO_ResetBits(Motor_520_BIN2_PORTx,  Motor_520_BIN2_Pinx);
		PWM_SetSta(Motor_520_BPWM_TIMx, Motor_520_BPWM_TIM_Channel_x, 0);
	}
	
}








