
#include "bsp_hc_sr04.h"

#include "bsp_exti.h"
#include "bsp_SysTick.h"
#include "bsp_time.h"
#include "bsp_gpio.h"

/**
* @brief   HC_SR04声波测距传感器初始化,默认计数器计数周期1us，1ms产生一次中断
* @param   HC_SR04_Struct:传感器对象结构体
* @retval  
*/
void HC_SR04_Init(HC_SR04_Typedef* HC_SR04_Struct)
{
	BSP_GPIO_Config(HC_SR04_Struct->Trig_PORT, HC_SR04_Struct->Trig_PIN,  GPIO_Mode_Out_PP);
	BSP_GPIO_Config(HC_SR04_Struct->Echo_PORT, HC_SR04_Struct->Echo_PIN,  GPIO_Mode_IPD);

	KEY_EXTI_Register( HC_SR04_Struct->Echo_PORT,  HC_SR04_Struct->Echo_PIN, 
						EXTI_Mode_Interrupt,  EXTI_Trigger_Rising_Falling);//上升沿下降沿
	
	HC_SR04_Struct->Echo_EXTI_Line = HC_SR04_Struct->Echo_PIN;

	
	BSP_TIMx_TIME_Init(HC_SR04_Struct->Time_TIM_Struct.TIMx, TIMx72M_1ms_Period, TIMx72M_1ms_Prescaler);
	TIM_Cmd(HC_SR04_Struct->Time_TIM_Struct.TIMx,DISABLE); //关闭定时器
	TIM_ITConfig( HC_SR04_Struct->Time_TIM_Struct.TIMx,  TIM_IT_Update,  ENABLE);//开启更新中断
	TIM_ClearITPendingBit( HC_SR04_Struct->Time_TIM_Struct.TIMx,  TIM_IT_Update);//清除中断标志位
	HC_SR04_Struct->Time_TIM_Struct.TIMx->CNT = 0;//复位计数器
	HC_SR04_Struct->Time_TIM_Struct.TIMx_DelayT_Count = 0;//复位定时的Tick
}


/**
  * @brief   HC_SR04开始一次测量
  * @param   HC_SR04_Struct[IN] ：HC_SR04 
  * @retval 
  */
void HC_SR04_StartMeasure(HC_SR04_Typedef* HC_SR04_Struct)
{
	TIM_ClearITPendingBit( HC_SR04_Struct->Time_TIM_Struct.TIMx,  TIM_IT_Update);//清除中断标志位
	HC_SR04_Struct->Time_TIM_Struct.TIMx->CNT = 0;//复位计数器
	HC_SR04_Struct->Time_TIM_Struct.TIMx_DelayT_Count = 0;//复位定时的Tick
	HC_SR04_Struct->usResult = 0;
	GPIO_SetBits( HC_SR04_Trip_PORTx,  HC_SR04_Trip_PINx);
	delay_us(20);
	GPIO_ResetBits( HC_SR04_Trip_PORTx,  HC_SR04_Trip_PINx);

}


/**
  * @brief   获取HC_SR04声波测距结果
  * @param   HC_SR04_Struct[IN] ：HC_SR04 
  * @param   dis[OUT] ：HC_SR04 所测距离/m
  * @retval  1 ：数据正确；0：数据错误
  */
uint8_t HC_SR04_get_distance(HC_SR04_Typedef* HC_SR04_Struct, float *dis)
{

	if (HC_SR04_Struct->usResult)
	{
		*dis = ((double)(HC_SR04_Struct->usResult)/1000000.0 )* 170.0 ;
	}
	else
	{
		return 0;
	}
	
	return 1;
}



//中断处理函数
void HC_SR04_IRQHandler(HC_SR04_Typedef* HC_SR04_Struct)
{
	if(EXTI_GetITStatus(HC_SR04_Struct->Echo_EXTI_Line) !=RESET )//判断是否进入中断，若进入中断则执行后面的中断程序
	{
		/**********************中断程序*********************/
		
		if(HC_SR04_Struct->Echo_PORT->IDR & HC_SR04_Struct->Echo_PIN)//高电平
		{
			TIM_Cmd(HC_SR04_Struct->Time_TIM_Struct.TIMx,ENABLE); //开启定时器
		}
		else
		{
			TIM_Cmd(HC_SR04_Struct->Time_TIM_Struct.TIMx,DISABLE); //关闭定时器
			HC_SR04_Struct->usResult = HC_SR04_Struct->Time_TIM_Struct.TIMx_DelayT_Count*1000 + HC_SR04_Struct->Time_TIM_Struct.TIMx->CNT;
		}
		
		/**************************************************/
		EXTI_ClearITPendingBit(HC_SR04_Struct->Echo_EXTI_Line);//完成中断后清除中断标志位
	}
	
}



