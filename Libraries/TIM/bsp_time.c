
#include "bsp_time.h"

#include "bsp_encoder.h"//编码器使用定时器计算速度

// static uint8_t GTaskDelay_Num;//需要用到定时的任务数(无需修改)
// static uint32_t GTask_DelayT_List[]={10-1, Encoder_Count_Period};//每个任务的定时时间 x ，(每 (x+1)*Delay_T 执行一次)
// static uint32_t GTask_DelayT_List_BCK[]={10-1, Encoder_Count_Period};//备份
// _Bool GTask_DelayFlag_List[] = {0,0};//每个任务的状态，0：定时未到，1：定时已到

// static unsigned long TIMx_Delay_Count;//
// static uint32_t TIMx_DelayT_Count;//TIMx_DelayT的计数值,最多计数1小时



void BSP_TIMx_TIME_UsageDemo(void)
{

	BSP_TIMx_TypeDef BSP_TIM3_Struct;//建议使用全局对象
	BSP_TIM3_Struct.TIMx = TIM3;

	BSP_TIMx_TIME_Init(TIM3, 36-1, 1000-1);//1ms
	/* 中断函数示例
	void TIM3_IRQHandler(void)
	{
	#if BSP_TIM3_IT_ON
	TIMx_DelayT_IRQHandler(BSP_TIM3_Struct);
	#endif //#if BSP_TIM3_IT_ON
	}
	*/
	TIMx_Delay_xCount(&BSP_TIM3_Struct, 10);

}



/**
  * @brief   定时器定时功能初始化；时基默认1分频，向上计数，不重复计数 
  * @param TIMx：要使用的定时器
  * @param reload：预装载比较值
  * @param psc：时钟分频因子； 定时时间(s): @ref TIMx_DelayT 
  * @retval  
  */
void BSP_TIMx_TIME_Init(TIM_TypeDef * TIMx, u16 reload, u16 psc)
{
	
	if(!IS_TIM_ALL_PERIPH(TIMx))return;

	TIM_Cmd(TIMx,DISABLE); //关闭定时器
	
	void (*TIMx_RCC_APBxPeriphClockCmd)(uint32_t RCC_APBxPeriph, FunctionalState NewState);
	uint32_t TIMx_CLK;

	NVIC_InitTypeDef NVIC_InitStruct;
	uint8_t NVIC_IRQChannel;
	
	if (TIMx==TIM2)//TIM2 的 CH1234 分别在 A0 A1 A2 A3 
	{	
		TIMx_CLK = RCC_APB1Periph_TIM2;
		TIMx_RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		
		NVIC_IRQChannel = TIM2_IRQn;
	}
	else if (TIMx==TIM3)//TIM3 的 CH1234 分别在 A6 A7 B0 B1
	{	
		TIMx_CLK = RCC_APB1Periph_TIM3;
		TIMx_RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		
		NVIC_IRQChannel = TIM3_IRQn;
	}	
	else if (TIMx==TIM4)//TIM4 的 CH1234 分别在 B6 B7 B8 B9
	{
		TIMx_CLK = RCC_APB1Periph_TIM4;
		TIMx_RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		
		NVIC_IRQChannel = TIM4_IRQn;
	}
	else //TIM1 在 APB2
	{
		TIMx_CLK = RCC_APB2Periph_TIM1;
		TIMx_RCC_APBxPeriphClockCmd = RCC_APB2PeriphClockCmd;
		
		NVIC_IRQChannel = TIM1_UP_IRQn;
	}
	

	TIMx_RCC_APBxPeriphClockCmd(TIMx_CLK, ENABLE);//开启TIMx时钟

	/******************  TIM配置 ****************/

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;


	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = reload;// 自动重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc;// 分频器值
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;// 未用
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct);//时基结构体配置
	
	
	
	/***********************中断配置*******************/
	
	TIM_ITConfig( TIMx,  TIM_IT_Update,  ENABLE);//开启更新中断，当计数器达到自动重装载值时产生中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//选择中断优先级分组
	
	NVIC_InitStruct.NVIC_IRQChannel = NVIC_IRQChannel;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_ClearITPendingBit( TIMx,  TIM_IT_Update);//清除中断标志位
	
	
	//自动重装载
	TIM_ARRPreloadConfig(TIMx,ENABLE);	//使能ARR上的TIMx外设预加装寄存器											
																
	TIM_Cmd(TIMx,ENABLE); //开启定时器
								
}


//延时count*TIMx_DelayT
void TIMx_Delay_xCount(BSP_TIMx_TypeDef* BSP_TIMx_Struct, uint32_t count)
{
	BSP_TIMx_Struct->TIMx_Delay_Count = count;
	while(BSP_TIMx_Struct->TIMx_Delay_Count);
}

//获取TIMx_DelayT计数值
void TIMx_Get_TIMx_DelayTCount(BSP_TIMx_TypeDef* BSP_TIMx_Struct, uint32_t *re)
{
	*re = BSP_TIMx_Struct->TIMx_DelayT_Count;
}

//中断处理函数
void TIMx_DelayT_IRQHandler(BSP_TIMx_TypeDef* BSP_TIMx_Struct)
{
	if(TIM_GetITStatus( BSP_TIMx_Struct->TIMx,  TIM_IT_Update) != RESET)
	{
		if (BSP_TIMx_Struct->TIMx_Delay_Count)
		{
			BSP_TIMx_Struct->TIMx_Delay_Count--;//延时计数
		}
		BSP_TIMx_Struct->TIMx_DelayT_Count++;//TIMx_DelayT_Count计数
		TIM_ClearITPendingBit( BSP_TIMx_Struct->TIMx,  TIM_IT_Update);//清除中断标志位
	}
}


	




