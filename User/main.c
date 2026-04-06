
#include "main.h"

/******************************************************/


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
	
	
	#if NRF_CTRL_M_ON
	BSP_USART_Print_Init(USART1, BSP_USARTxStructInit(115200) , USART1_IO_Reamp1);
	#else //NRF_CTRL_M_ON
	BSP_USART_Print_Init(USART2, BSP_USARTxStructInit(115200) , USART2_IO_Reamp0);
	#endif
	UsartPrint( "TEST...\n");

	UsartPrint( "HCLK_Frequency:%d\n", RCC_Clocks.HCLK_Frequency);
	UsartPrint( "PCLK1_Frequency:%d\n", RCC_Clocks.PCLK1_Frequency);
	UsartPrint( "PCLK2_Frequency:%d\n", RCC_Clocks.PCLK2_Frequency);
	UsartPrint( "SYSCLK_Frequency:%d\n", RCC_Clocks.SYSCLK_Frequency);

#endif //USART_Print_ON
	

#if NRF_CTRL_M_ON
	if (NRF_Controller_Config(NRF_CTRL_MASTER))
	{
		LED_ON(1);
		UsartPrint( "NRF_Controller_Config err\n");
		while(1);
	}
	
#else //NRF_CTRL_M_ON

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
        UsartPrint("\r\n没有检测到MPU6050传感器！\r\n");
		#endif //USART_Print_ON
        LED_ON(1); 
        while(1);	
    }
	#endif //MPU_ON

	//NRF遥控器配置
	#if NRF_CTRL_ON //

	if (NRF_Controller_Config(NRF_CTRL_SLAVER))
	{
		LED_ON(1);
		UsartPrint( "NRF_Controller_Config err\n");
		while(1);
	}

	#endif // NRF_CTRL_ON

	#if HC_SR04_ON //
	//定时器延时初始化
	TIMx_Delay_Register(TIM3,  TIMx_10us_Period,  TIMx_10us_Prescaler);//10us中断
	
	//声波测距传感器初始化
	HC_SR04_Init( HC_SR04_Trip_PORTx,  HC_SR04_Trip_PINx,
				  HC_SR04_Echo_PORTx,  HC_SR04_Echo_PINx);//声波测距模块初始化
	#endif //HC_SR04_ON
	
	#if BANLANCE_CAR_ON
	
	TWSBV_Init();//小车初始化参数
	#if USART_Print_ON
	CTRL_PrintSBVParam();//输出小车参数
	#endif
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
	#endif //BANLANCE_CAR_ON


#endif //NRF_CTRL_M_ON
}


void Test(void)
{
	
	//电机测试
	#if Motor_DEBUG
	BSP_520Motor_Rotation( Motor_520_A,  Motor_520_ForwardRotation,  20.0);
	BSP_520Motor_Rotation( Motor_520_B,  Motor_520_ForwardRotation,  20.0);
	#endif //Motor_DEBUG

	//电机停止
//	BSP_520Motor_Stop( Motor_520_A);
//	BSP_520Motor_Stop( Motor_520_B);


}


int main(void)
{
	Hardware_Init();//初始化外围硬件
	Test();//外设测试例程
	
	//板载LED以1Hz的频率闪烁3次；
	for(uint8_t count=0;count<3;count++)
	{
		LED_ON(1);
		SisTic_Delay_ms(500);
		LED_OFF(1);
		SisTic_Delay_ms(500);
	}
	

#if NRF_CTRL_M_ON //作为遥控器
	NRF_Controller_Run_Typedef RunStruct = NRF_Controller_Run_TypedefDefaultVal;
	while (1)
	{
		NRF_Controller_Run(&RunStruct);
	}
#else //NRF_CTRL_M_ON


#if BANLANCE_CAR_ON //启动平衡车 	

#if NRF_CTRL_ON //
	//NRF_Controller_WaitForConnect();
#endif //NRF_CTRL_ON


	//开始控制小车

    uint32_t nowTime = 0;
    uint32_t timeDiff = 0;
    uint32_t LED_LastRunTime = 0;
    uint32_t LED_RunPeriod = 300;
	#if NRF_CTRL_ON //开启遥控
	NRF_Controller_Run_Typedef RunStruct = NRF_Controller_Run_TypedefDefaultVal;
	#endif //NRF_CTRL_ON
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
		
		#if NRF_CTRL_ON //开启遥控
				NRF_Controller_Run(&RunStruct);
		#endif //NRF_CTRL_ON

		#if OLED_ON
		OLED_Display_XxX_ASCII( 0, 16,  8, 12, 1, "%5.2f %5.2f %5.2f\n", G_Euler_RPY[0], G_Euler_RPY[1], G_Euler_RPY[2]);
		OLED_Refresh();
		#endif //OLED_ON

	}	
	
#endif // BANLANCE_CAR_ON

#endif //NRF_CTRL_M_ON

	while (1)
	{
		
	}

}



