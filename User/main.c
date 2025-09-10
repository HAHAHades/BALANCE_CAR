

#include "stdio.h"
#include "stm32f10x.h"


#include "bsp_SysTick.h"

#include "bsp_led.h"
#include "bsp_usart.h"
#include "oled.h"


#include "mpu6050.h"
#include "mpu6050_SL.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "invensense.h"
#include "inv_mpu.h"
//#include "invensense_adv.h"
#include "eMPL_outputs.h"

#include "bsp_hc_sr04.h"

#ifdef   soft_IIC //MPU
#include "bsp_soft_i2c.h"
#else
#include "bsp_hard_i2c.h"
#endif

#include "bsp_520Motor.h"
#include "bsp_encoder.h"
#include "bsp_control.h"

#include "nrf_controller.h"


/*
	printf重定向到USART1
*/

/*************硬件连接*******************
c8t6(USART1)   (USART2)     USB-TTL-CH340
PA9(TXD)   	     PA2            RX     
PA10(RXD)	     PA3            TX    
PA8(SCK)         PA4            SCK

*******************************************/


/*********************************************

测试流程：
上电；
1.板载LED以1Hz的频率闪烁3次；

*******************************************/

#define SisTic_Delay_ms mdelay

extern _Bool GTask_DelayFlag_List[];

float volatile G_Euler_RPY[3];//欧拉角
float volatile G_GYRO_XYZ[3];//角速度
float volatile G_ACCEL_XYZ[3];//加速度

extern int32_t G_BSPCTRL_TargetSpeed ;//前后速度
extern int32_t G_BSPCTRL_TurnSpeed ;//转向速度，正为左转，负为右转

/******************************************************/

#if ENCODER_GET_COUNT_IN_IT
extern int volatile G_ENCODERA_COUNT;
extern int volatile G_ENCODERB_COUNT;
#endif //ENCODER_GET_COUNT_IN_IT


#define UART_Printtf 1 //是否使用UART输出信息
#define OLED_ON 0 //OLED 输出信息
#define HC_SR04_ON 0 //是否开启声波测距
#define OLED_SHOW_MPU 0 //OLED 显示mpu的信息
#define USART_DEBUG_MPU 0 //USART 调试mpu
#define OLED_SHOW_HC_SR04 0 //OLED 显示声波传感器的信息
#define OLED_SHOW_Encoder_Speed 0 //OLED 显示编码器信息
#define Motor_DEBUG 0 //调试电机
#define CONTROL_CAR_IN_IT  1 // 是否在中断中控制小车，在主函数里控制小车无法同时做其他事情，如刷新OLED等
#define BANLANCE_CAR_TEST 1 //平衡车测试
#define NRF_CTRL_ON 0 //是否使用NRF遥控器

void Hardware_Init(void)
{
    /* 配置SysTick定时器和中断 */
    SysTick_Init(); //配置 SysTick 为 1ms 中断一次，在中断里读取传感器数据
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //启动定时器
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置
	
	//板载LED注册
	LED_Register(1, GPIOC, GPIO_Pin_13, LED_Trigger_Level_Low);//配置LED灯的端口
	
	//串口初始化
	#if UART_Printtf //
	Usart2_Init( 115200);
	UsartPrintf(USART_DEBUG, "TEST...\n");
	#endif // UART_Printtf

	//OLED初始化
	#if OLED_ON //
	IICxInit( OLED_IICx, OLED_IICx_PinRemapping,  I2C_Speed);
	OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
	OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	OLED_Display_XxX_ASCII( 0, 0,  8, 16, 1, "TEST...\n");
	OLED_Refresh();
	#endif // OLED_ON
	
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
		#if UART_Printtf
        printf("\r\n没有检测到MPU6050传感器！\r\n");
		#endif //UART_Printtf
        LED_ON(1); 
        while(1);	
    }
	
    #if UART_Printtf
    printf("\r\n检测到MPU6050传感器\r\n");
    #endif //UART_Printtf
	
	
	//NRF遥控器配置
	#if NRF_CTRL_ON //
	uint8_t tmpret=0;
	tmpret = NRF_Controller_Config();
	if (!tmpret)
	{
		#if UART_Printtf
		UsartPrintf(USART_DEBUG, "NRF Controller Config failed!\n");
		#endif
		LED_ON(1);
		while (1);
	}
	else
	{
		#if UART_Printtf
		UsartPrintf(USART_DEBUG, "NRF Controller Config OK!\n");
		#endif
	}
	#endif // NRF_CTRL_ON

	#if HC_SR04_ON //
	//定时器延时初始化
	TIMx_Delay_Register(TIM3,  TIMx_10us_Period,  TIMx_10us_Prescaler);//10us中断
	
	//声波测距传感器初始化
	HC_SR04_Init( HC_SR04_Trip_PORTx,  HC_SR04_Trip_PINx,
				  HC_SR04_Echo_PORTx,  HC_SR04_Echo_PINx);//声波测距模块初始化
	#endif //HC_SR04_ON
	

	//电机初始化
	TIM_DeInit( TIM1);
	
	BSP_520Motor_Config( Motor_520_APWM_TIMx,  Motor_520_APWM_TIM_Channel_x, 
						Motor_520_AIN1_PORTx,  Motor_520_AIN1_Pinx, 
						 Motor_520_AIN2_PORTx,  Motor_520_AIN2_Pinx);
	
	BSP_520Motor_Config( Motor_520_BPWM_TIMx,  Motor_520_BPWM_TIM_Channel_x, 
						Motor_520_BIN1_PORTx,  Motor_520_BIN1_Pinx, 
						 Motor_520_BIN2_PORTx,  Motor_520_BIN2_Pinx);
	
	//编码器初始化
	TIMx_CHx_ENCODER_Init( EncoderA_TIMx,  EncoderA_TIMx_ACH_x, 0x00 ,  0); //
	Get_Encoder_Count(EncoderA_TIMx);//计数器清零
	
	TIMx_CHx_ENCODER_Init( EncoderB_TIMx,  EncoderB_TIMx_ACH_x, 0x00 ,  0); //
	Get_Encoder_Count(EncoderB_TIMx);//计数器清零
	
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
	RCC_ClocksTypeDef get_rcc_clock;   
	RCC_GetClocksFreq(&get_rcc_clock); 

	#if UART_Printtf
	UsartPrintf(USART_DEBUG, "HCLK_Frequency:%d\n", get_rcc_clock.HCLK_Frequency);
	UsartPrintf(USART_DEBUG, "PCLK1_Frequency:%d\n", get_rcc_clock.PCLK1_Frequency);
	UsartPrintf(USART_DEBUG, "PCLK2_Frequency:%d\n", get_rcc_clock.PCLK2_Frequency);
	UsartPrintf(USART_DEBUG, "SYSCLK_Frequency:%d\n", get_rcc_clock.SYSCLK_Frequency);
    #endif //UART_Printtf
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
	
	#if USART_DEBUG_MPU
	while (1)
	{
		UsartPrintf(USART_DEBUG, "%5.2f %5.2f %5.2f\n", G_Euler_RPY[0], G_Euler_RPY[1], G_Euler_RPY[2]);
		SisTic_Delay_ms(500);
	}
	#endif //USART_DEBUG_MPU


#if BANLANCE_CAR_TEST //平衡车测试 	
	//2.开始控制小车
	#if CONTROL_CAR_IN_IT
	while(1)
	{
		#if NRF_CTRL_ON
		NRF_Controller_RunOnce();
		#endif // NRF_CTRL_ON

		#if OLED_ON
		OLED_Display_XxX_ASCII( 0, 16,  8, 12, 1, "%5.2f %5.2f %5.2f\n", G_Euler_RPY[0], G_Euler_RPY[1], G_Euler_RPY[2]);
		OLED_Refresh();
		#endif //OLED_ON
		LED_ON(1);
		SisTic_Delay_ms(500);
		LED_OFF(1);
		SisTic_Delay_ms(500);
	}	
	#endif //CONTROL_CAR_IN_IT
	
	
	#if !CONTROL_CAR_IN_IT
    unsigned char new_temp = 0;
    unsigned long timestamp;
	float Euler_RPY[3];//欧拉角
	float GYRO_XYZ[3];//角速度
    while (1)
    {
        /* 接收到INT传来的中断信息后继续往下 */
        if (!hal.sensors || !hal.new_gyro)
        {
            continue;
        }

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
				#if UART_Printtf
                printf("数据错误\n");
				#endif //UART_Printtf
            }

         
            if (inv_get_sensor_type_euler(data, &accuracy, (inv_time_t *)&timestamp))
            {
                Euler_RPY[0] = data[0] * 1.0 / (1 << 16);
                Euler_RPY[1] = data[1] * 1.0 / (1 << 16);
                Euler_RPY[2] = data[2] * 1.0 / (1 << 16);
				
				#if UART_Printtf
                printf("\r\n欧拉角(rad)\t\t: %7.5f\t %7.5f\t %7.5f\t", Euler_RPY[0], Euler_RPY[1], Euler_RPY[2]);
				#endif //UART_Printtf
				
				#if OLED_SHOW_MPU
				OLED_Display_XxX_ASCII( 0, 0,  8, 16, 1, "euler:R,P,Y\n", Euler_RPY[0], Euler_RPY[1], Euler_RPY[2]);
				OLED_Display_XxX_ASCII( 0, 16,  8, 16, 1, "%5.2f %5.2f %5.2f\n", Euler_RPY[0], Euler_RPY[1], Euler_RPY[2]);
				OLED_Refresh();
				#endif //OLED_SHOW_MPU
            }
//            if (inv_get_sensor_type_accel(data, &accuracy, (inv_time_t *)&timestamp))
//            {
//                float_data[0] = data[0] * 1.0 / (1 << 16);
//                float_data[1] = data[1] * 1.0 / (1 << 16);
//                float_data[2] = data[2] * 1.0 / (1 << 16);
//				#if UART_Printtf
//                printf("\r加速度(g/s)\t\t: %7.5f\t %7.5f\t %7.5f\t\r", float_data[0], float_data[1], float_data[2]);
//				#endif //UART_Printtf
//			}

            if (inv_get_sensor_type_gyro(data, &accuracy, (inv_time_t *)&timestamp))
            {
                GYRO_XYZ[0] = data[0] * 1.0 / (1 << 16);
                GYRO_XYZ[1] = data[1] * 1.0 / (1 << 16);
                GYRO_XYZ[2] = data[2] * 1.0 / (1 << 16);
				#if UART_Printtf
                printf("角速度(rad/s)\t\t: %7.5f\t %7.5f\t %7.5f\t\r\n", GYRO_XYZ[0], GYRO_XYZ[1], GYRO_XYZ[2]);
				#endif //UART_Printtf+
            }		
//            mdelay(10);
        }//end if (new_data)

	#if BANLANCE_CAR_TEST
		int16_t Encoder_CountA = Get_Encoder_Count(EncoderA_TIMx);//读取编码器
		int16_t Encoder_CountB = -Get_Encoder_Count(EncoderB_TIMx);
		
//	    float SpeedA = (Encoder_CountA/11.0)*100.0*(60.0/(30*4.0)); // rpm ,定时器在正交编码器模式下电机一圈产生四个上/下计数
//		float SpeedB = (Encoder_CountB/11.0)*100.0*(60.0/(30*4.0)); // rpm
//		OLED_Display_XxX_ASCII( 0, 0,  8, 16, 1, "Arpm:%08.4f\n", SpeedA);
//		OLED_Display_XxX_ASCII( 0, 16,  8, 16, 1, "Brpm:%08.4f\n", SpeedB);
//		OLED_Refresh();
		
		
		Control_PWM( 0.0,  Euler_RPY[0],  GYRO_XYZ[0],  0.0,  Encoder_CountA,  Encoder_CountB,  GYRO_XYZ[2]);//控制小车
//		
//		OLED_Display_XxX_ASCII( 0, 0,  8, 12, 1, "euler:R,P,Y\n", Euler_RPY[0], Euler_RPY[1], Euler_RPY[2]);
//		OLED_Display_XxX_ASCII( 0, 16,  8, 12, 1, "%5.2f %5.2f %5.2f\n", Euler_RPY[0], Euler_RPY[1], Euler_RPY[2]);
//		OLED_Refresh();
		
		
	#endif //BANLANCE_CAR_TEST
		
		
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
			
//			mdelay(20);
		#endif //OLED_SHOW_Encoder_Speed

		}
	#endif //OLED_SHOW_Encoder_Speed
		
//		mdelay(10);
    }//end while(1)
	#endif //!CONTROL_CAR_IN_IT
#endif // BANLANCE_CAR_TEST

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

	#if BANLANCE_CAR_TEST
		int16_t Encoder_CountA = Get_Encoder_Count(EncoderA_TIMx);//读取编码器
		int16_t Encoder_CountB = -Get_Encoder_Count(EncoderB_TIMx);
		
		Control_PWM( 0.0,  G_Euler_RPY[0],  G_GYRO_XYZ[0],  G_BSPCTRL_TargetSpeed,  Encoder_CountA,  Encoder_CountB, G_BSPCTRL_TurnSpeed , G_GYRO_XYZ[2]);//控制小车
	#endif //BANLANCE_CAR_TEST
		
}


