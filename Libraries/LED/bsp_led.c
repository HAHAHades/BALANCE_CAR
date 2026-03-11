#include "bsp_led.h"//板级支持包board support package\
                      仅支持某一个或一批同样的开发板
					  
					  
uint8_t Registered_LED_Count = 0;//已注册的LED个数			  				  
uint8_t LED_NUM_List[MAX_LED_NUM];
GPIO_TypeDef* LED_GPIO_POTRx_List[MAX_LED_NUM];
uint16_t LED_GPIO_PINx_List[MAX_LED_NUM];
_Bool LED_Trigger_Level_List[MAX_LED_NUM];
					


static void Modify_LED_List(uint8_t LED_num, GPIO_TypeDef* LED_GPIO_POTRx, uint16_t LED_GPIO_PINx, _Bool Trigger_Level)
{
	uint8_t i=0;
	for(i=0; i<Registered_LED_Count;i++)//查找LED是否注册
	{
		if(LED_NUM_List[i]==LED_num)
		{
			LED_GPIO_POTRx_List[i] = LED_GPIO_POTRx;
			LED_GPIO_PINx_List[i] = LED_GPIO_PINx;
			LED_Trigger_Level_List[i] = Trigger_Level;
		
			break;
		}
	
	}
	
	if(i>=Registered_LED_Count)//输入LED序号未注册
	{
		LED_NUM_List[Registered_LED_Count] = LED_num;
		LED_GPIO_POTRx_List[Registered_LED_Count] = LED_GPIO_POTRx;
		LED_GPIO_PINx_List[Registered_LED_Count] = LED_GPIO_PINx;
		LED_Trigger_Level_List[Registered_LED_Count] = Trigger_Level;
		Registered_LED_Count++;
	}
	return;
}


/**
  * @brief   初始化LED
  * @param   
  *		@arg LED_num：LED序号
  *		@arg LED_GPIOx：LED绑定的IO端口
  *		@arg LED_GPIO_PINx：LED绑定的IO引脚
  *		@arg rigger_Level：触发电平
  * @retval  
  */
void LED_Register(uint8_t LED_num, GPIO_TypeDef* LED_GPIO_POTRx, uint16_t LED_GPIO_PINx, _Bool Trigger_Level)
{	
	if(LED_num==0||LED_num>MAX_LED_NUM) return; 
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
	uint32_t LED_RCC_APB2Periph_GPIOx  = RCC_APB2Periph_GPIOA << \
										(((uint32_t)LED_GPIO_POTRx - (uint32_t)GPIOA) / 0x0400 );
	
	RCC_APB2PeriphClockCmd(LED_RCC_APB2Periph_GPIOx, ENABLE);//打开LED灯的外设时钟
	GPIO_InitStruct.GPIO_Pin =LED_GPIO_PINx;
	GPIO_Init(LED_GPIO_POTRx, &GPIO_InitStruct);//初始化LED灯对应的端口，将配置\
                                              的模式、速度等写入寄存器中	
	
	if(Trigger_Level)//高电平触发
	{
		GPIO_ResetBits(LED_GPIO_POTRx, LED_GPIO_PINx);//关灯
	}
	else
	{
		GPIO_SetBits(LED_GPIO_POTRx, LED_GPIO_PINx);//关灯
	}
	Modify_LED_List( LED_num,  LED_GPIO_POTRx,  LED_GPIO_PINx,  Trigger_Level);
	
}

#if 0
/**
  * @brief   初始化LED
  * @param   
  *		@arg LED_num：LED序号
  *		@arg LED_GPIOx：LED绑定的IO端口
  *		@arg LED_GPIO_PINx：LED绑定的IO引脚
  *		@arg rigger_Level：触发电平
  * @retval  
  */
void LED_Register(uint8_t LED_num, GPIO_TypeDef* LED_GPIO_POTRx, uint16_t LED_GPIO_PINx, _Bool Trigger_Level)
{	
	if(LED_num==0||LED_num>MAX_LED_NUM) return; 
	GPIO_InitTypeDef GPIO_InitStruct;
	

	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
	uint32_t RCC_APB2Periph;
	switch ((uint32_t)LED_GPIO_POTRx)
	{
		case ((uint32_t)GPIOA) :
			RCC_APB2Periph = RCC_APB2Periph_GPIOA;
			break;
		case ((uint32_t)GPIOB) :
			RCC_APB2Periph = RCC_APB2Periph_GPIOB;
			break;
		case ((uint32_t)GPIOC) :
			RCC_APB2Periph = RCC_APB2Periph_GPIOC;
			break;
		case ((uint32_t)GPIOD) :
			RCC_APB2Periph = RCC_APB2Periph_GPIOD;
			break;
		case ((uint32_t)GPIOE) :
			RCC_APB2Periph = RCC_APB2Periph_GPIOE;
			break;
		case ((uint32_t)GPIOF) :
			RCC_APB2Periph = RCC_APB2Periph_GPIOF;
			break;
		case ((uint32_t)GPIOG) :
			RCC_APB2Periph = RCC_APB2Periph_GPIOG;
			break;
		default:
			RCC_APB2Periph = RCC_APB2Periph_GPIOC;
			break;
	}
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph, ENABLE);//打开LED灯的外设时钟
	GPIO_InitStruct.GPIO_Pin =LED_GPIO_PINx;
	GPIO_Init(LED_GPIO_POTRx, &GPIO_InitStruct);//初始化LED灯对应的端口，将配置\
                                              的模式、速度等写入寄存器中	
	
	if(Trigger_Level)//高电平触发
	{
		GPIO_ResetBits(LED_GPIO_POTRx, LED_GPIO_PINx);//关灯
	}
	else
	{
		GPIO_SetBits(LED_GPIO_POTRx, LED_GPIO_PINx);//关灯
	}

	Modify_LED_List( LED_num,  LED_GPIO_POTRx,  LED_GPIO_PINx,  Trigger_Level);
	
	
	
}
#endif //#if 0

/**
  * @brief   改变LED状态
  * @param   
  *		@arg LED_num：LED序号
*		@arg newsta：LED新状态, 1:ON;0:OFF
  * @retval  
  */
void LED_NEW_STA(uint8_t LED_num, _Bool newsta)
{
	if(LED_num>MAX_LED_NUM)return;
	
	uint8_t i;
	for(i=0; i<Registered_LED_Count;i++)
	{
		if(LED_NUM_List[i]==LED_num)break;
	
	}

	if(i>=Registered_LED_Count)return;//输入LED序号未注册
	
	//修改已注册LED状态
	if(newsta==LED_Trigger_Level_List[i])
	{
		 GPIO_SetBits(LED_GPIO_POTRx_List[i], LED_GPIO_PINx_List[i]);//高电平触发点亮，低电平触发熄灭
		
	}
	else
	{
		GPIO_ResetBits(LED_GPIO_POTRx_List[i], LED_GPIO_PINx_List[i]);//高电平触发熄灭，低电平触发点亮
		
	}
	
	return;

}


/**
  * @brief   点亮LED
  * @param   
  *		@arg LED_num：LED序号
  * @retval  
  */
void LED_ON(uint8_t LED_num)
{
	LED_NEW_STA( LED_num, 1);
	return;
}


/**
  * @brief   熄灭LED
  * @param   
  *		@arg LED_num：LED序号
  * @retval  
  */
void LED_OFF(uint8_t LED_num)
{
	LED_NEW_STA( LED_num, 0);
	return;
}

