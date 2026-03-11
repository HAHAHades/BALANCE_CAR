
#include "bsp_hc_sr04.h"
#include "bsp_tim.h"


static uint32_t HC_SR04_Echo_EXTI_Linex;


/**
  * @brief   HC_SR04声波测距传感器 IO初始化
  * @param   
  *		@arg HC_Trig_GPIO_POTRx ：HC_SR04 的测距触发引脚 （给10us以上高电平开始一次测距）
  *		@arg HC_Trig_GPIO_PINx ：HC_SR04 的测距触发引脚
  *		@arg HC_Echo_GPIO_POTRx ：HC_SR04 的测距声波持续时间输出引脚 （输出高电平持续时间即为声波发送开始到接收完成的时间）
  *		@arg HC_Echo_GPIO_PINx ： HC_SR04 的测距声波持续时间输出引脚
  * @retval  
  */
static void HC_SR04_GPIO_Config(GPIO_TypeDef* HC_Trig_GPIO_POTRx, uint16_t HC_Trig_GPIO_PINx,
						 GPIO_TypeDef* HC_Echo_GPIO_POTRx, uint16_t HC_Echo_GPIO_PINx)
{


	uint32_t HC_Trig_PORT_RCC_APB2Periph  = RCC_APB2Periph_GPIOA << \
											(((uint32_t)HC_Trig_GPIO_POTRx - (uint32_t)GPIOA) / 0x0400 );
	uint32_t HC_Echo_PORT_RCC_APB2Periph  = RCC_APB2Periph_GPIOA << \
											(((uint32_t)HC_Echo_GPIO_POTRx - (uint32_t)GPIOA) / 0x0400 );
	
	RCC_APB2PeriphClockCmd(HC_Trig_PORT_RCC_APB2Periph|HC_Echo_PORT_RCC_APB2Periph, ENABLE);//打开时钟
	
	
	if(HC_Trig_GPIO_POTRx==GPIOB )
	{
	
		if(HC_Trig_GPIO_PINx & GPIO_Pin_3 || HC_Trig_GPIO_PINx & GPIO_Pin_4)
		{
			// 使用了B3 B4等引脚
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
		}
		
	}
	if(HC_Echo_GPIO_POTRx==GPIOB )
	{
	
		if(HC_Echo_GPIO_PINx & GPIO_Pin_3 || HC_Echo_GPIO_PINx & GPIO_Pin_4)
		{
			// 使用了B3 B4等引脚
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
		}
		
	}
	
	
	
	GPIO_InitTypeDef  GPIO_InitStructure; 
	
	/* HC_SR04_Trip 、 HC_SR04_Echo*/
	GPIO_InitStructure.GPIO_Pin = HC_Trig_GPIO_PINx;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	       
	GPIO_Init(HC_Trig_GPIO_POTRx, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = HC_Echo_GPIO_PINx;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	     
	GPIO_Init(HC_Echo_GPIO_POTRx, &GPIO_InitStructure);	

	
	
	
}



static void HC_SR04_EXTI_Config(GPIO_TypeDef* HC_Echo_GPIO_POTRx, uint16_t HC_Echo_GPIO_PINx)
{

	KEY_EXTI_Register( HC_Echo_GPIO_POTRx,  HC_Echo_GPIO_PINx, 
						EXTI_Mode_Interrupt,  EXTI_Trigger_Rising_Falling);//上升沿下降沿
	
	HC_SR04_Echo_EXTI_Linex = HC_Echo_GPIO_PINx;//中断线
}


void HC_SR04_Init(GPIO_TypeDef* HC_Trig_GPIO_POTRx, uint16_t HC_Trig_GPIO_PINx,
						 GPIO_TypeDef* HC_Echo_GPIO_POTRx, uint16_t HC_Echo_GPIO_PINx)
{

	HC_SR04_GPIO_Config( HC_Trig_GPIO_POTRx,  HC_Trig_GPIO_PINx,
						 HC_Echo_GPIO_POTRx, HC_Echo_GPIO_PINx);

	HC_SR04_EXTI_Config( HC_Echo_GPIO_POTRx,  HC_Echo_GPIO_PINx);
	
}


/**
  * @brief   HC_SR04开始一次测量
  * @param   
  *	
  * @retval 
  */
void HC_SR04_StartMeasure(void)
{
	GPIO_SetBits( HC_SR04_Trip_PORTx,  HC_SR04_Trip_PINx);
	SisTic_Delay_ms(1);
	GPIO_ResetBits( HC_SR04_Trip_PORTx,  HC_SR04_Trip_PINx);
}



static  uint32_t HC_SR04_Last_TimeStamp;//*10us
static  uint32_t  HC_SR04_Now_TimeStamp;//*10us
static  uint32_t HC_SR04_Echo_H_Time;//*10us
static volatile  _Bool HC_SR04_Echo_EXTI_Falg = 0;//1已产生上升沿，0未产生上升沿


/**
  * @brief   获取HC_SR04声波测距结果
  * @param   
  *		@arg[OUT] dis ：HC_SR04 所测距离/m
  * @retval  1 ：数据正确；0：数据错误
  */
_Bool HC_SR04_get_distance(float *dis)
{
	
	*dis = ((float)HC_SR04_Echo_H_Time/HC_SR04_Echo_H_T_Div10us)*17;//m
	if(HC_SR04_Echo_H_Time)
		return 1;
	else
		return 0;
}




//中断处理函数

void HC_SR04_IRQHandler(void)
{
	if(EXTI_GetITStatus(HC_SR04_Echo_EXTI_Linex) !=RESET )//判断是否进入中断，若进入中断则执行后面的中断程序
	{
		/**********************中断程序*********************/
		
		if(HC_SR04_Echo_EXTI_Falg)
		{
			// TIMx_Get_10usTicCount(&HC_SR04_Now_TimeStamp);//10us
			#if (HC_SR04_TIME_BSPTIMxStruct!=NULL)
			TIMx_Get_TIMx_DelayTCount(HC_SR04_TIME_BSPTIMxStruct, &HC_SR04_Now_TimeStamp);
			#else
			#warning There are undefined actual parameters: HC_SR04_TIME_BSPTIMxStruct
			#endif //(HC_SR04_TIME_BSPTIMxStruct!=NULL)
			HC_SR04_Echo_H_Time = HC_SR04_Now_TimeStamp - HC_SR04_Last_TimeStamp;
			HC_SR04_Echo_EXTI_Falg = 0;
		}
		else
		{
			// TIMx_Get_10usTicCount(&HC_SR04_Last_TimeStamp);//10us
			#if (HC_SR04_TIME_BSPTIMxStruct!=NULL)
			TIMx_Get_TIMx_DelayTCount(HC_SR04_TIME_BSPTIMxStruct, &HC_SR04_Last_TimeStamp);
			#else
			#warning There are undefined actual parameters: HC_SR04_TIME_BSPTIMxStruct
			#endif //(HC_SR04_TIME_BSPTIMxStruct!=NULL)
			HC_SR04_Echo_EXTI_Falg = 1;
		}
		
		/**************************************************/
	}
	EXTI_ClearITPendingBit(HC_SR04_Echo_EXTI_Linex);//完成中断后清除中断标志位

}



