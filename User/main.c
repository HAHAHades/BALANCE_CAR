

#include "main.h"


/*********************************************

测试流程：
上电；
1.板载LED以1Hz的频率闪烁3次；

*******************************************/

// extern _Bool GTask_DelayFlag_List[];





/******************************************************/

#if ENCODER_GET_COUNT_IN_IT
extern int volatile G_ENCODERA_COUNT;
extern int volatile G_ENCODERB_COUNT;
#endif //ENCODER_GET_COUNT_IN_IT


void Hardware_Init(void)
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	/* 配置SysTick定时器和中断 */
	SysTick_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置
	
	//板载LED注册
	LED_Register(1, GPIOC, GPIO_Pin_13, LED_Trigger_Level_Low);//配置LED灯的端口

#if USART_Print_ON
	//串口初始化
	#if BANLANCE_CAR_ON
	BSP_USARTx_Init(USART_DEBUG, BSP_USARTxStructInit(115200) , USART2_IO_Reamp0);
	#else 
	BSP_USARTx_Init(USART_DEBUG, BSP_USARTxStructInit(115200) , USART1_IO_Reamp1);
	#endif //BANLANCE_CAR_ON
	UsartPrintf(USART_DEBUG, "TEST...\n");

	UsartPrintf(USART_DEBUG, "HCLK_Frequency:%d\n", RCC_Clocks.HCLK_Frequency);
	UsartPrintf(USART_DEBUG, "PCLK1_Frequency:%d\n", RCC_Clocks.PCLK1_Frequency);
	UsartPrintf(USART_DEBUG, "PCLK2_Frequency:%d\n", RCC_Clocks.PCLK2_Frequency);
	UsartPrintf(USART_DEBUG, "SYSCLK_Frequency:%d\n", RCC_Clocks.SYSCLK_Frequency);

#endif //USART_Print_ON
	

	//OLED初始化
	#if OLED_Display_ON
	//OLED屏幕相关外设初始化
	IICxInit(OLED_IICx, BSP_IICxStructInit(400000),  OLED_IICx_IO_Reamp);//400k
	SisTic_Delay_ms(500);

	OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
	OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	
	//OLED屏幕输出样例信息；
	OLED_Display_XxX_ASCII(0, 0, 8, 16, 1, "TEST...");
	OLED_Refresh();

	#endif //OLED_Display_ON


	#if MPU_ON
    // MPU6050 IIC 初始化
	
    #ifdef soft_IIC //mpu使用软/硬件IIC
		// 使用了B3 B4等引脚
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
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
		#if USART_Print_ON
        UsartPrintf(USART_DEBUG,"\r\n没有检测到MPU6050传感器！\r\n");
		#endif //USART_Print_ON
        LED_ON(1); 
        while(1);	
    }
	#endif //MPU_ON

	//NRF遥控器配置
	#if NRF_CTRL_ON //



	#endif // NRF_CTRL_ON

	#if HC_SR04_ON //
	//定时器延时初始化
	TIMx_Delay_Register(TIM3,  TIMx_10us_Period,  TIMx_10us_Prescaler);//10us中断
	
	//声波测距传感器初始化
	HC_SR04_Init( HC_SR04_Trip_PORTx,  HC_SR04_Trip_PINx,
				  HC_SR04_Echo_PORTx,  HC_SR04_Echo_PINx);//声波测距模块初始化
	#endif //HC_SR04_ON
	
	#if BANLANCE_CAR_ON
	//电机初始化
	TIM_DeInit(Motor_520_APWM_TIMx);
	
	BSP_520Motor_Config( Motor_520_APWM_TIMx,  Motor_520_APWM_TIM_Channel_x, 
						Motor_520_AIN1_PORTx,  Motor_520_AIN1_Pinx, 
						 Motor_520_AIN2_PORTx,  Motor_520_AIN2_Pinx);
	
	BSP_520Motor_Config( Motor_520_BPWM_TIMx,  Motor_520_BPWM_TIM_Channel_x, 
						Motor_520_BIN1_PORTx,  Motor_520_BIN1_Pinx, 
						 Motor_520_BIN2_PORTx,  Motor_520_BIN2_Pinx);
	
	//编码器初始化
	TIMx_CHx_ENCODER_Init( EncoderA_TIMx,  EncoderA_TIMx_ACH_x, EncoderA_TIMx_IO_Reamp ,  0); //
	Get_Encoder_Count(EncoderA_TIMx);//计数器清零
	
	TIMx_CHx_ENCODER_Init( EncoderB_TIMx,  EncoderB_TIMx_ACH_x, EncoderB_TIMx_IO_Reamp ,  0); //
	Get_Encoder_Count(EncoderB_TIMx);//计数器清零
	#endif //BANLANCE_CAR_ON
}


void C8_6T6_Test(void)
{
	
	//电机测试
	#if Motor_DEBUG
	BSP_520Motor_Rotation( Motor_520_A,  Motor_520_ForwardRotation,  20.0);
	BSP_520Motor_Rotation( Motor_520_B,  Motor_520_ForwardRotation,  20.0);
	#endif //Motor_DEBUG

	
	//电机停止
//	BSP_520Motor_Stop( Motor_520_A);
//	BSP_520Motor_Stop( Motor_520_B);

	
	/****************************************/

}



int main(void)
{
	Hardware_Init();//初始化外围硬件
	C8_6T6_Test();//核心板外设测试例程
	
	//1.板载LED以1Hz的频率闪烁3次；
	for(uint8_t count=0;count<3;count++)
	{
		LED_ON(1);
		SisTic_Delay_ms(500);
		LED_OFF(1);
		SisTic_Delay_ms(500);
	}
	

	NRF_Controller_UsageDemo();
	while (1)
	{
		/* code */
	}
	
#if 0
	if (NRF_Controller_Config())
	{
		LED_ON(1);
		UsartPrintf(USART_DEBUG, "NRF_Controller_Config err\n");
		while(1);
	}
#endif //NRF_CTRL_ON

#if NRF_CTRL_M_ON
	if (NRF_Controller_Config())
	{
		LED_ON(1);
		UsartPrintf(USART_DEBUG, "NRF_Controller_Config err\n");
		while(1);
	}
	while (1)
	{

		NRF_Controller_Run();
	}
#endif //NRF_CTRL_M_ON


	#if USART_DEBUG_MPU
	while (1)
	{
		#if (!MPU_GetEuler_IN_IT)
		MPU_GetEuler(G_Euler_RPY, G_ACCEL_XYZ, G_GYRO_XYZ);
		#endif //
		UsartPrintf(USART_DEBUG, "%5.2f %5.2f %5.2f\n", G_Euler_RPY[0], G_Euler_RPY[1], G_Euler_RPY[2]);
		SisTic_Delay_ms(500);
	}
	#endif //USART_DEBUG_MPU


#if BANLANCE_CAR_ON //平衡车测试 	
	//2.开始控制小车




	#if CONTROL_CAR_IN_IT

    uint32_t nowTime = 0;
    uint32_t timeDiff = 0;
    uint32_t LED_LastRunTime = 0;
    uint32_t LED_RunPeriod = 300;

	while(1)
	{
        nowTime = NRFCTR_GetTime();
        timeDiff = nowTime - LED_LastRunTime;
        if (timeDiff > 2*LED_RunPeriod)
        {
          	
			LED_OFF(1);
			LED_LastRunTime = NRFCTR_GetTime();
        }
		else if (timeDiff > LED_RunPeriod)
		{
			LED_ON(1);
		}
		
#if NRF_CTRL_ON
		NRF_Controller_Run();
#endif //NRF_CTRL_ON




		#if OLED_ON
		OLED_Display_XxX_ASCII( 0, 16,  8, 12, 1, "%5.2f %5.2f %5.2f\n", G_Euler_RPY[0], G_Euler_RPY[1], G_Euler_RPY[2]);
		OLED_Refresh();
		#endif //OLED_ON

	}	
	#endif //CONTROL_CAR_IN_IT
	
	#if MPU_GetEuler_IN_IT
	#else
	while(1)
	{

		MPU_GetEuler(G_Euler_RPY, G_ACCEL_XYZ, G_GYRO_XYZ);
		int16_t Encoder_CountA = Get_Encoder_Count(EncoderA_TIMx);//读取编码器
		int16_t Encoder_CountB = -Get_Encoder_Count(EncoderB_TIMx);
		Control_PWM( 0.0,  G_Euler_RPY[0],  G_GYRO_XYZ[0],  G_BSPCTRL_TargetSpeed,  Encoder_CountA,  Encoder_CountB, G_BSPCTRL_TurnSpeed,G_GYRO_XYZ[2]);//控制小车

//	    float SpeedA = (Encoder_CountA/11.0)*100.0*(60.0/(30*4.0)); // rpm ,定时器在正交编码器模式下电机一圈产生四个上/下计数
//		float SpeedB = (Encoder_CountB/11.0)*100.0*(60.0/(30*4.0)); // rpm
//		OLED_Display_XxX_ASCII( 0, 0,  8, 16, 1, "Arpm:%08.4f\n", SpeedA);
//		OLED_Display_XxX_ASCII( 0, 16,  8, 16, 1, "Brpm:%08.4f\n", SpeedB);
//		OLED_Refresh();
//		OLED_Display_XxX_ASCII( 0, 0,  8, 12, 1, "euler:R,P,Y\n", Euler_RPY[0], Euler_RPY[1], Euler_RPY[2]);
//		OLED_Display_XxX_ASCII( 0, 16,  8, 12, 1, "%5.2f %5.2f %5.2f\n", Euler_RPY[0], Euler_RPY[1], Euler_RPY[2]);
//		OLED_Refresh();
		SisTic_Delay_ms(8);//
	}
	#endif //MPU_GetEuler_IN_IT
		#if OLED_SHOW_HC_SR04
		float HC_distance;
		HC_SR04_StartMeasure();
		if(HC_SR04_get_distance(&HC_distance))
		{
			OLED_Display_XxX_ASCII( 0, 0,  8, 16, 1, "HC_distance:\n");
			OLED_Display_XxX_ASCII( 0, 16,  8, 16, 1, "%7.5f\n", HC_distance);
			OLED_Refresh();
		}
		#endif //OLED_SHOW_HC_SR04
		
	#if OLED_SHOW_Encoder_Speed
		#if !ENCODER_GET_COUNT_IN_IT
		if(GTask_DelayFlag_List[Encoder_Count_GTask_DelayFlag_List_at])
		#endif //ENCODER_GET_COUNT_IN_IT
		{
			#if !ENCODER_GET_COUNT_IN_IT
				GTask_DelayFlag_List[Encoder_Count_GTask_DelayFlag_List_at] = 0;
			#endif //ENCODER_GET_COUNT_IN_IT
		
			#if OLED_SHOW_Encoder_Speed
			
				#if ENCODER_GET_COUNT_IN_IT
			
				SpeedA = ((float)G_ENCODERA_COUNT/11.0)*100.0*(60.0/(30*4)); // rpm ,定时器在正交编码器模式下电机一圈产生四个上/下计数
				SpeedB = ((float)G_ENCODERB_COUNT/11.0)*100.0*(60.0/(30*4)); // rpm
				OLED_Display_XxX_ASCII( 0, 0,  8, 16, 1, "Arpm:%.3f\n", SpeedA);
				OLED_Display_XxX_ASCII( 0, 16,  8, 16, 1, "Brpm:%.3f\n", SpeedB);
				OLED_Refresh();
			
				#else
			
				int16_t Encoder_CountA = Get_Encoder_Count(EncoderA_TIMx);
				int16_t Encoder_CountB = Get_Encoder_Count(EncoderB_TIMx);
				SpeedA = (Encoder_CountA/11.0)*100.0*(60.0/(30*4.0)); // rpm ,定时器在正交编码器模式下电机一圈产生四个上/下计数
				SpeedB = (Encoder_CountB/11.0)*100.0*(60.0/(30*4.0)); // rpm
				OLED_Display_XxX_ASCII( 0, 0,  8, 16, 1, "Arpm:%08.4f\n", SpeedA);
				OLED_Display_XxX_ASCII( 0, 16,  8, 16, 1, "Brpm:%08.4f\n", SpeedB);
				OLED_Refresh();
				#endif //ENCODER_GET_COUNT_IN_IT
				
			//mdelay(20);
			#endif //OLED_SHOW_Encoder_Speed

		}
	#endif //OLED_SHOW_Encoder_Speed
#else //BANLANCE_CAR_ON


#endif // BANLANCE_CAR_ON




}



