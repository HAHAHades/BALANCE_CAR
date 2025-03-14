
#include "bsp_tim.h"

#include "bsp_encoder.h"//编码器使用定时器计算速度

static uint8_t GTaskDelay_Num;//需要用到定时的任务数(无需修改)
static uint32_t GTask_DelayT_List[]={10-1, Encoder_Count_Period};//每个任务的定时时间 x ，(每 (x+1)*Delay_T 执行一次)
static uint32_t GTask_DelayT_List_BCK[]={10-1, Encoder_Count_Period};//备份
_Bool GTask_DelayFlag_List[] = {0,0};//每个任务的状态，0：定时未到，1：定时已到

static unsigned long TIMx_Delay_Count;
static uint32_t TIMx_10us_TickCount;//最多计数1小时

/**
  * @brief   定时器定时功能初始化
  * @param   
  *		@arg PWM_TIMx：要使用的定时器
  *		@arg reload：预装载比较值
  *		@arg psc：时钟分频因子；频率=72M/(psc+1*reload+1)； 定时时间:Delay_T = 1 / 频率(s)
  * @retval  
  */


void TIMx_Delay_Register(TIM_TypeDef * TIMx, u16 reload, u16 psc)
{
	
	if(!IS_TIM_ALL_PERIPH(TIMx))return;

	GTaskDelay_Num = ARRAY_LENGTH(GTask_DelayT_List);
	
	
	
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
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter =0;// 未用
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


//延时count*Delay_T
void TIMx_Delay_xCount(uint32_t count)
{
	
	TIMx_Delay_Count = count;
	
	while(TIMx_Delay_Count);

}

//获取10us计数值
void TIMx_Get_10usTicCount(uint32_t* re)
{
	*re = TIMx_10us_TickCount;
}

//中断处理函数
#if ENCODER_GET_COUNT_IN_IT
extern int volatile G_ENCODERA_COUNT;
extern int volatile G_ENCODERB_COUNT;
#endif //ENCODER_GET_COUNT_IN_IT
void TIMx_10us_IRQHandler(void)
{
	if(TIM_GetITStatus( TIMx_Delay,  TIM_IT_Update) != RESET)
	{
		
		for(uint8_t i=0; i<GTaskDelay_Num; i++)
		{
			if(GTask_DelayT_List[i])
			{
				GTask_DelayT_List[i]-- ; 
			}
			else
			{
				GTask_DelayT_List[i] = GTask_DelayT_List_BCK[i];
				GTask_DelayFlag_List[i] = 1; 
			}
		}
		
		#if ENCODER_GET_COUNT_IN_IT
		if(GTask_DelayFlag_List[Encoder_Count_GTask_DelayFlag_List_at])
		{
			
			G_ENCODERA_COUNT = (int16_t)TIM_GetCounter(EncoderA_TIMx);
			TIM_SetCounter( EncoderA_TIMx,  0);//计数器清零
			G_ENCODERB_COUNT = (int16_t)TIM_GetCounter(EncoderB_TIMx);
			TIM_SetCounter( EncoderB_TIMx,  0);//计数器清零
			GTask_DelayFlag_List[Encoder_Count_GTask_DelayFlag_List_at]=0;
			
		}
		#endif //ENCODER_GET_COUNT_IN_IT
		
		TIMx_Delay_Count--;//延时计数
		TIMx_10us_TickCount++;//10us计数
		TIM_ClearITPendingBit( TIMx_Delay,  TIM_IT_Update);//清除中断标志位
	}
}


	




