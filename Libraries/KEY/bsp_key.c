
#include "bsp_key.h"


uint8_t Registered_KEY_Count = 0;//已注册的KEY个数			  				  
uint8_t KEY_NUM_List[MAX_KEY_NUM];
GPIO_TypeDef* KEY_GPIO_POTRx_List[MAX_KEY_NUM];
uint16_t KEY_GPIO_PINx_List[MAX_KEY_NUM];






static void Modify_LED_List(uint8_t KEY_num, GPIO_TypeDef* KEY_GPIO_POTRx, uint16_t KEY_GPIO_PINx)
{
	uint8_t i=0;
	for(i=0; i<Registered_KEY_Count;i++)//查找LED是否注册
	{
		if(KEY_NUM_List[i]==KEY_num)
		{
			KEY_GPIO_POTRx_List[i] = KEY_GPIO_POTRx;
			KEY_GPIO_PINx_List[i] = KEY_GPIO_PINx;
		
		
			break;
		}
	
	}
	
	if(i>=Registered_KEY_Count)//输入LED序号未注册
	{
		KEY_NUM_List[Registered_KEY_Count] = KEY_num;
		KEY_GPIO_POTRx_List[Registered_KEY_Count] = KEY_GPIO_POTRx;
		KEY_GPIO_PINx_List[Registered_KEY_Count] = KEY_GPIO_PINx;
	
		Registered_KEY_Count++;
	}
	return;
}


/**
  * @brief   初始化KEY
  * @param   
  *		@arg KEY_num ：按键序号 <=MAX_KEY_NUM
  *		@arg KEY_GPIO_POTRx ：按键绑定的IO端口
  *		@arg KEY_GPIO_PINx ：按键绑定的IO引脚
  * @retval  
  */
void KEY_Register(uint8_t KEY_num, GPIO_TypeDef* KEY_GPIO_POTRx, uint16_t KEY_GPIO_PINx)
{	
	if(KEY_num==0||KEY_num>MAX_KEY_NUM) return; 
	GPIO_InitTypeDef GPIO_InitStruct;
	

	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	

	uint32_t KEY_RCC_APB2Periph_GPIOx  = RCC_APB2Periph_GPIOA << \
										(((uint32_t)KEY_GPIO_POTRx - (uint32_t)GPIOA) / 0x0400 );
	
	RCC_APB2PeriphClockCmd(KEY_RCC_APB2Periph_GPIOx, ENABLE);//打开KEY的外设时钟
	GPIO_InitStruct.GPIO_Pin = KEY_GPIO_PINx;
	GPIO_Init(KEY_GPIO_POTRx, &GPIO_InitStruct);//初始化LED灯对应的端口，将配置\
                                              的模式、速度等写入寄存器中	
	
	Modify_LED_List( KEY_num,  KEY_GPIO_POTRx,  KEY_GPIO_PINx);

}

/**
  * @brief   扫描按键是否被按下
  * @param   
  *		@arg KEY_num ：KEY序号
* @retval  Key_Sta : 1被按下  0未按下
  */
_Bool KEY_Scan(uint8_t KEY_num)
{
	
	if(KEY_num==0||KEY_num>MAX_KEY_NUM) return 0;
	
	_Bool Key_Sta = 0;
	
	
	uint8_t i;
	for(i=0; i<Registered_KEY_Count;i++)
	{
		if(KEY_NUM_List[i]==KEY_num)break;
	}
	
	if(GPIO_ReadInputDataBit(KEY_GPIO_POTRx_List[i], KEY_GPIO_PINx_List[i])==Bit_SET)
	{
		while(GPIO_ReadInputDataBit(KEY_GPIO_POTRx_List[i], KEY_GPIO_PINx_List[i])==Bit_SET);
		Key_Sta = 1;
	}

	return Key_Sta;
}






