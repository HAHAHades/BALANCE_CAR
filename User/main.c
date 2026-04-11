
#include "main.h"

/******************************************************/
#if HC_SR04_ON
HC_SR04_Typedef G_HC_SR04_Struct;
#endif //HC_SR04_ON

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
	UsartPrint_DEBUG( "TEST...\n");
	UsartPrint_DEBUG( "HCLK_Frequency:%d\n", RCC_Clocks.HCLK_Frequency);
	UsartPrint_DEBUG( "PCLK1_Frequency:%d\n", RCC_Clocks.PCLK1_Frequency);
	UsartPrint_DEBUG( "PCLK2_Frequency:%d\n", RCC_Clocks.PCLK2_Frequency);
	UsartPrint_DEBUG( "SYSCLK_Frequency:%d\n", RCC_Clocks.SYSCLK_Frequency);
#endif //USART_Print_ON

#if NRF_CTRL_M_ON
	if (NRF_Controller_Config(NRF_CTRL_MASTER))
	{
		LED_ON(1);
		UsartPrint_DEBUG( "NRF_Controller_Config err\n");
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


	#if HC_SR04_ON //
	HC_SR04_Typedef HC_SR04_Struct = HC_SR04_Typedef_DefaultVal;
	G_HC_SR04_Struct = HC_SR04_Struct;
	HC_SR04_Init(&G_HC_SR04_Struct);
	#endif //HC_SR04_ON
	

#endif //NRF_CTRL_M_ON
}


void Test(void)
{
	

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

    uint32_t nowTime = 0;
    uint32_t timeDiff = 0;
    uint32_t LED_LastRunTime = 0;
    uint32_t LED_RunPeriod = 300;

	#if OLED_SHOW_MPU
    uint32_t OLED_MPULastDipTime = 0;
    uint32_t OLED_MPUDispPeriod = 500;
	#endif //OLED_SHOW_MPU

	#if HC_SR04_ON //
    uint32_t HC_SR04_LastDetcTime = 0;
    uint32_t HC_SR04_DetcPeriod = 500;
	#endif //	#if HC_SR04_ON //


	BSP_SBVInit();
	while (1)
	{
        get_tick_count(&nowTime);
        timeDiff = nowTime - LED_LastRunTime;
        if (timeDiff > 2*LED_RunPeriod)
        {
			LED_OFF(1);
			get_tick_count(&LED_LastRunTime);
        }
		else if (timeDiff > LED_RunPeriod)
		{
			LED_ON(1);
			 //printf("\n加速度(g/s)\t\t: %7.5f; %7.5f; %7.5f;\n", G_ACCEL_XYZ[0], G_ACCEL_XYZ[1], G_ACCEL_XYZ[2]);

		}



		#if OLED_SHOW_MPU
		timeDiff = nowTime - OLED_MPULastDipTime;
        if (timeDiff > OLED_MPUDispPeriod)
        {
			OLED_Clear();
			OLED_Display_XxX_ASCII( 0, 0,  8, 12, 1, "%.2f %.2f %.2f\n", G_Euler_RPY[0], G_Euler_RPY[1], G_Euler_RPY[2]);
			OLED_Refresh();
			get_tick_count(&OLED_MPULastDipTime);
        }
		#endif //OLED_SHOW_MPU


		#if HC_SR04_ON //

        if (G_HC_SR04_Struct.usResult)
        {
			float hc_dis = 0;
			HC_SR04_get_distance(&G_HC_SR04_Struct,&hc_dis);
			G_HC_SR04_Struct.usResult = 0;
			#if OLED_SHOW_HC_SR04
			OLED_Clear();
			OLED_Display_XxX_ASCII( 0, 0,  8, 12, 1, "%.5f\n", hc_dis);
			OLED_Refresh();
			#endif //OLED_SHOW_HC_SR04
        }

		timeDiff = nowTime - HC_SR04_LastDetcTime;
        if (timeDiff > HC_SR04_DetcPeriod)
        {
			HC_SR04_StartMeasure( &G_HC_SR04_Struct);
			UsartPrint_DEBUG("HC_SR04 Detect...");
			get_tick_count(&HC_SR04_LastDetcTime);
        }
		#endif //	#if HC_SR04_ON //

		BSP_SBV_RunOnece();


	}

#endif //NRF_CTRL_M_ON
}



