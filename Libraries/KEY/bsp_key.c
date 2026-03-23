
#include "bsp_key.h"
#include "bsp_gpio.h"
#include "stdlib.h"


//组合键的标识在所有实体按键的标识之后
static uint8_t SG_KEY_Registered_Button_Count = 0;//已注册的实体按键个数		  				  
static uint8_t SG_KEY_Registered_Comb_Count = 0;//已注册的组合键个数	
static KEY_Button_TypeDef* SG_KEY_ButtonStruct_List = NULL;//已注册的实体按键对象列表
static KEY_COMBINATION_t* SG_KEY_CombStruct_List = NULL;//已注册的组合键对象列表
static uint32_t SG_KEY_ButtonPressFlag;//实体按键按下标志位，指示所有按键按下状态
static uint32_t SG_KEY_CombPressFlag;//组合键触发标志位，指示触发了组合键的所有按键状态
/* 按键回调标识FIFO储存器 */
static KEY_FIFO_t SG_KeyCBFlagFIFO[KEY_FIFO_CAPACITY]={KEY_FIFO_DUMMY};
static KEY_FIFORW_t SG_KeyFifoRW_Obj; 


uint8_t G_key_TesksTick = 0; // 按键任务运行计时



//电位计
static uint8_t SG_KEY_Registered_Pot_Count = 0;//已注册的电位计按键个数
static KEY_Pot_TypeDef* SG_KeyPotStruct_List;


/*****************************************************************/



void BSP_KEY_UsageDemo(void)
{
	uint8_t initSta; 
	uint8_t Button_Count = 4;
	uint8_t Comb_Count = 2;
	uint8_t Pot_Count = 4;

	initSta = BSP_KEY_InitParam( Button_Count,  Comb_Count, Pot_Count);
	if (initSta)
	{
		//return init failed code
		return;
	}
	
	//先注册配置实体按钮，索引从0开始，小于Button_Count
	KEY_Button_TypeDef ButtonStruct;
	//设置按钮0参数为 PA0引脚 低电平有效，开启单击按下、长按、双击回调
	BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_8, KEY_PL_RESET, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Button_Register( 0,  ButtonStruct);
	BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_9, KEY_PL_RESET, (KEY_Flag_P|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Button_Register( 1,  ButtonStruct);
	BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_10, KEY_PL_RESET, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Button_Register( 2,  ButtonStruct);
	BSP_KEY_Button_StructInit(&ButtonStruct,  GPIOA, GPIO_Pin_11, KEY_PL_RESET, (KEY_Flag_P|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Button_Register( 3,  ButtonStruct);


	//再注册配置组合键，按键数多的索引应越小，否则会出现歧义(比如A+B+C会触发A+B)，索引从0开始，小于Comb_Count
	KEY_COMBINATION_t CombStruct;
	//组合键0 包含3个按键 索引分别为 0，2，4，开启单击释放、长按、双击回调
	uint8_t indexList1[] = {0,2};
	BSP_KEY_COMB_StructInit( &CombStruct,  2, indexList1, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Comb_Register(0 , CombStruct);
	uint8_t indexList2[] = {0,3};
	BSP_KEY_COMB_StructInit( &CombStruct,  2, indexList2, (KEY_Flag_R|KEY_Flag_LP|KEY_Flag_DPR));
	BSP_KEY_Comb_Register(1 , CombStruct);

	//再注册电位计按键
	KEY_Pot_TypeDef PotStruct;
	KEY_Pot_StructInit(&PotStruct, ADC1,  ADC_Channel_0);
	BSP_KEY_Pot_Register(0,  PotStruct);
	KEY_Pot_StructInit(&PotStruct, ADC1,  ADC_Channel_1);
	BSP_KEY_Pot_Register(1,  PotStruct);
	KEY_Pot_StructInit(&PotStruct, ADC1,  ADC_Channel_2);
	BSP_KEY_Pot_Register(2,  PotStruct);
	KEY_Pot_StructInit(&PotStruct, ADC1,  ADC_Channel_3);
	BSP_KEY_Pot_Register(3,  PotStruct);


	BSP_KEY_GpioConfig();



	//重复执行 Key_TickTask 更新io状态
	//根据需求重复执行void KeyDetectButton(void)更新按键FIFO

	uint32_t nowTime = 0;
	uint32_t timeDiff = 0;


	uint32_t Key_TickTaskTime = 10;
	uint32_t Key_TickTask_LastRunTime = 0;

	uint32_t KeyDetectButton_Time = 50;
	uint32_t KeyDetectButton_LastRunTime = 0;

	uint32_t ReadFIFO_Time = 200;
	uint32_t LastReadFIFO_Time = 0;	
	uint16_t tmpFlag;
	while (1)
	{
		//非中断式扫描

		nowTime = Key_GetTime();

		timeDiff = nowTime - Key_TickTask_LastRunTime;
		if (timeDiff > Key_TickTaskTime)
		{
			Key_TickTask();
			Key_TickTask_LastRunTime = nowTime;
		}


		timeDiff = nowTime - KeyDetectButton_LastRunTime;
		if (timeDiff > KeyDetectButton_Time)
		{
			KeyDetectButton();
			KeyDetectButton_LastRunTime = nowTime;
		}


		uint8_t FlagCount=0;
		KEY_FIFO_t Flag[KEY_FIFO_CAPACITY];
		
		//读取所有flag
		timeDiff = nowTime - LastReadFIFO_Time;
		if (timeDiff > ReadFIFO_Time)
		{
			LastReadFIFO_Time = nowTime;
			FlagCount = KeyGetFIFOFlag(Flag,  KEY_FIFO_CAPACITY);

			uint8_t tmpV1;
			uint8_t tmpV2;
			uint8_t potIndex;
			for (uint8_t i = 0; i < FlagCount; i++)
			{

				if (Flag[i] & KEY_POT_FIFO_FLAG)
				{
					//电位计数据

					KEY_POT_PCT_t potData = (KEY_POT_PCT_t)(Flag[i]&KEY_POT_FIFO_MASK);
					potIndex = (Flag[i]&KEY_POT_FIFO_I_MASK)>>KEY_POT_FIFO_I_BIT_S;
					KEY_DEBUG("pot%d data%d", potIndex, potData);
					continue;
				}
				
				// if (Flag[i]==Key1_P)
				// {
				// 	//按键1按下
				// }
				// else if (Flag[i]==Key2_P)
				// {
				// 	//按键2按下
				// }
				//.......
				tmpFlag = Flag[i]&KEY_BUTTON_FIFO_MASK;
				KEY_DEBUG("KeyFlag:%d", tmpFlag);
				tmpV1 = tmpFlag/KEY_FLAG_NUM;
				tmpV2 = tmpFlag%KEY_FLAG_NUM;
				if (tmpV2==Key1_P)
				{
					KEY_DEBUG("Key%d单击按下", tmpV1);
				}
				else if (tmpV2==Key1_R)
				{
					KEY_DEBUG("Key%d单击释放", tmpV1);
				}
				else if (tmpV2==Key1_DPR)
				{
					KEY_DEBUG("Key%d双击释放", tmpV1);
				}
				else if (tmpV2==Key1_LP)
				{
					KEY_DEBUG("Key%d长按", tmpV1);
				}
				
			}
		}
		
	}
	
	//重新配置前需清除原有配置
	// BSP_KEY_ClearButton();

}


/**
  * @brief   获取时间戳/ms
  * @param   
  * @retval  
**/
uint32_t Key_GetTime(void) 
{
	unsigned long count;
	get_tick_count(&count);
	return count;
}


/**
  * @brief   配置所有按钮的外设
  * @param   
  * @retval  
**/
 void BSP_KEY_GpioConfig(void)
{
	GPIO_TypeDef* GPIO_POTRx;
	uint16_t GPIO_PINx;
	GPIOMode_TypeDef GPIO_Mode;
	for (uint8_t i = 0; i < SG_KEY_Registered_Button_Count; i++)
	{
		GPIO_POTRx = SG_KEY_ButtonStruct_List[i].IO_Struct.gpio;
		GPIO_PINx = SG_KEY_ButtonStruct_List[i].IO_Struct.pin;
		GPIO_Mode = (SG_KEY_ButtonStruct_List[i].IO_Struct.pressedLevel==KEY_PL_RESET)? GPIO_Mode_IPU:GPIO_Mode_IPD;
		BSP_GPIO_Config(GPIO_POTRx,  GPIO_PINx,  GPIO_Mode);
	}
	
	uint32_t ADCx_Mode = ADC_Mode_Independent;
	uint8_t ADC_Channel_xList[SG_KEY_Registered_Pot_Count] ;

	//默认采用ADC1 开启DMA
	for (uint8_t i = 0; i < SG_KEY_Registered_Pot_Count; i++)
	{
		ADC_Channel_xList[i] = SG_KeyPotStruct_List[i].ADC_Channel_x;

	}
	if (SG_KEY_Registered_Pot_Count)
	{
		ADCx_CHx_IndependentCongfig(SG_KeyPotStruct_List[0].ADCx, ADC_Channel_xList,SG_KEY_Registered_Pot_Count);
		
		ADC_START(SG_KeyPotStruct_List[0].ADCx,  ADCx_Mode);
	}
	

	
}

/**
  * @brief   按默认方式初始化按钮对象
  * @param   Button: 需要初始化的按键对象
  * @param   gpio: 
  * @param   pin: 
  * @param   pressedLevel: 触发电平 @ref BSP_KEY_PressedLevel
  * @param   keyEnableFlag: 使能的按键状态输出标识 @ref KEY_Enable_Flag
  * @retval  
**/
void BSP_KEY_Button_StructInit(KEY_Button_TypeDef* Button, GPIO_TypeDef* gpio, uint16_t pin, 
								uint16_t pressedLevel,uint8_t keyEnableFlag)
{
	Button->IO_Struct.gpio = gpio;
	Button->IO_Struct.pin = pin;
	Button->IO_Struct.pressedLevel = pressedLevel;
	Button->IO_Struct.currentLevel = !pressedLevel;
	Button->IO_Struct.updataTime = 0;

	Button->Sta_Struct.status = keySta_dummy;
	Button->Sta_Struct._LPFTime = G_KEY_LP_FT;
	Button->Sta_Struct._LPRPeriod = G_KEY_LP_RT;
	Button->Sta_Struct.updataTime = 0;
	Button->Sta_Struct.keyEnableFlag = keyEnableFlag;


	return;
}

/**
  * @brief   按默认方式初始化按钮对象
  * @param   Comb: 需要初始化的按键对象
  * @param   keyCount: 该组合键包含实体按键数量
  * @param   keyIndexList: 所包含按键在按键列表中的索引  
  * @param   keyEnableFlag: 使能的按键状态输出标识 @ref KEY_Enable_Flag
  * @retval  
**/
void BSP_KEY_COMB_StructInit(KEY_COMBINATION_t* Comb, uint8_t keyCount, 
						uint8_t* keyIndexList, uint8_t keyEnableFlag)
{

	uint32_t keyPressFlag = 0;
	Comb->keyCount = keyCount;
	Comb->keyIndexList = (uint8_t*)malloc(keyCount*sizeof(uint8_t));
	if (Comb->keyIndexList==NULL)
	{
		KEY_DEBUG("key comb struct init failed!");
		return;
	}
	
	for (uint8_t i = 0; i < keyCount; i++)
	{
		Comb->keyIndexList[i] = keyIndexList[i];
		keyPressFlag |= (uint32_t)1 << keyIndexList[i];
	}
	Comb->keyPressFlag = keyPressFlag;
	Comb->keyNowFlagSta = 0;

	Comb->Sta_Struct.status = keySta_dummy;
	Comb->Sta_Struct._LPFTime = G_KEY_LP_FT;
	Comb->Sta_Struct._LPRPeriod = G_KEY_LP_RT;
	Comb->Sta_Struct.updataTime = 0;
	Comb->Sta_Struct.keyEnableFlag = keyEnableFlag;

	return;
}

/**
  * @brief   按键注册配置
  * @param   Index: 该按键在按键列表中的索引(应小于实体按键的数量)
  * @param   Button_Struct: 按键对象
  * @retval  0:成功，else：失败
**/
uint8_t BSP_KEY_Button_Register(uint8_t Index, KEY_Button_TypeDef Button_Struct)
{
	uint8_t rVal=0;
	if (Index<SG_KEY_Registered_Button_Count)
	{
		SG_KEY_ButtonStruct_List[Index] = Button_Struct;
	}
	else
	{
		rVal = 1;
		KEY_DEBUG("too many buttons! ignore button with index:%d", Index);
	}
	return rVal;
}


/**
  * @brief   组合键注册配置
  * @param   Index: 该按键在按键列表中的索引(应小于实体按键的数量)
  * @param   Comb_Struct: 按键对象
  * @retval  0:成功，else：失败
**/
uint8_t BSP_KEY_Comb_Register(uint8_t Index, KEY_COMBINATION_t Comb_Struct)
{
	uint8_t rVal=0;
	if (Index<SG_KEY_Registered_Comb_Count)
	{
		SG_KEY_CombStruct_List[Index] = Comb_Struct;
	}
	else
	{
		rVal = 1;
		KEY_DEBUG("too many combination buttons! ignore button with index:%d", Index);
	}
	return rVal;
}


/**
  * @brief   按键初始化参数
  * @param   Button_Count: 所有实体按键数，不包括电位计
  * @param   Comb_Count: 组合键数量
  * @param   Pot_Count: 电位计键数量
  * @retval  0：成功，else：失败
**/
uint8_t BSP_KEY_InitParam(uint8_t Button_Count, uint8_t Comb_Count, uint8_t Pot_Count)
{
	uint8_t rVal=0;

	SG_KEY_Registered_Button_Count = Button_Count;

	SG_KEY_Registered_Comb_Count = Comb_Count;

	SG_KEY_Registered_Pot_Count = Pot_Count;

	SG_KEY_ButtonStruct_List = (KEY_Button_TypeDef*)malloc(Button_Count*sizeof(KEY_Button_TypeDef));
	
	SG_KEY_CombStruct_List = (KEY_COMBINATION_t*)malloc(Comb_Count*sizeof(KEY_COMBINATION_t));

	SG_KeyPotStruct_List = (KEY_Pot_TypeDef*)malloc(Pot_Count*sizeof(KEY_Pot_TypeDef));



	if ((SG_KEY_ButtonStruct_List==NULL) || (SG_KEY_CombStruct_List==NULL) || (SG_KeyPotStruct_List==NULL))
	{
		rVal = 1;
		BSP_KEY_ClearButton();
		KEY_DEBUG("key init param failed!");
	}
	
	return rVal;
}


/**
  * @brief   清除所有按钮配置
  * @param   
  * @retval  
**/
void BSP_KEY_ClearButton(void)
{
	for (uint8_t i = 0; i < SG_KEY_Registered_Comb_Count; i++)
	{
		free(SG_KEY_CombStruct_List[i].keyIndexList);
	}
	
	free(SG_KEY_CombStruct_List);
	free(SG_KEY_ButtonStruct_List);
	free(SG_KeyPotStruct_List);

	SG_KEY_Registered_Button_Count = 0;
	SG_KEY_Registered_Comb_Count = 0;
	SG_KEY_Registered_Pot_Count = 0;
}


/**
  * @brief   读取按键FIFO里的Flag
  * @param   Flag 返回的Flag
  * @param   maxFlagNum 最多返回多少个Flag
  * @retval  0：未读取到Flag，else：实际返回的Flag个数
**/
uint8_t KeyGetFIFOFlag(KEY_FIFO_t *Flag, uint8_t maxFlagNum)
{
	uint8_t ret = 0;
	KEY_FIFO_t rFlag;
	do
	{
		/* 一直读取FIFO */
		rFlag = KeyReadFIFO();
		if (rFlag != KEY_FIFO_DUMMY)
		{
			/* 读取到有效标识 */
			Flag[ret] = rFlag;
			ret++;
		}
	} while ((rFlag != KEY_FIFO_DUMMY)&&(ret<maxFlagNum));

	return ret;
}


/**
  * @brief   向按键FIFO里储存器里写入按键标识
  * @param   KeyFlag : 按键回调标识
  * @retval  
**/
void KeyWriteFIFO(KEY_FIFO_t KeyFlag)
{
	SG_KeyCBFlagFIFO[SG_KeyFifoRW_Obj.WP] = KeyFlag;
	SG_KeyFifoRW_Obj.DataLen++;
	SG_KeyFifoRW_Obj.WP++;
	if (SG_KeyFifoRW_Obj.WP >= KEY_FIFO_CAPACITY)
	{
		/* 超出边界重置指针 */
		SG_KeyFifoRW_Obj.WP = 0;
	}
	if (SG_KeyFifoRW_Obj.DataLen>KEY_FIFO_CAPACITY)
	{
		//数据溢出，覆盖旧数据
		SG_KeyFifoRW_Obj.DataLen=KEY_FIFO_CAPACITY;
		SG_KeyFifoRW_Obj.RP++;
		if (SG_KeyFifoRW_Obj.RP >= KEY_FIFO_CAPACITY)
		{
			/* 超出边界重置指针 */
			SG_KeyFifoRW_Obj.RP = 0;
		}
	}
	
	return;
}


/**
  * @brief   从按键FIFO储存器里读取按键标识
  * @param   
  * @retval  KEY_FIFO_DUMMY: 无标识 else: 读取的第一个按键回调标识
**/
KEY_FIFO_t KeyReadFIFO(void)
{
	KEY_FIFO_t ret = KEY_FIFO_DUMMY;

	if ( SG_KeyFifoRW_Obj.DataLen>0)
	{
		/* 可读 */
		ret = SG_KeyCBFlagFIFO[SG_KeyFifoRW_Obj.RP];
		SG_KeyFifoRW_Obj.DataLen--;
		SG_KeyFifoRW_Obj.RP++;
		if (SG_KeyFifoRW_Obj.RP >= KEY_FIFO_CAPACITY)
		{
			/* 超出边界重置指针 */
			SG_KeyFifoRW_Obj.RP = 0;
		}
	}

	return ret;
}

/**
  * @brief   清除按键FIFO储存器
  * @param   
  * @retval  
**/
void KeyClearFIFO(void)
{
	SG_KeyFifoRW_Obj.RP = 0;
	SG_KeyFifoRW_Obj.WP = 0;
	SG_KeyFifoRW_Obj.DataLen = 0;
	return;
}


/**
  * @brief   扫描检测所有按键包括电位计，并更新状态
  * @param   
  * @retval  
**/
void KeyDetectButton(void)
{
	uint8_t isPressed;
	uint32_t tmpFlag;
	uint32_t nowTime;
	uint32_t diffTime;
	for (uint8_t i = 0; i < SG_KEY_Registered_Comb_Count; i++)
	{
		tmpFlag = SG_KEY_CombStruct_List[i].keyPressFlag;
		if (((tmpFlag & SG_KEY_CombPressFlag)==tmpFlag)&&(tmpFlag != SG_KEY_CombPressFlag))
		{
			continue;//跳过被包含的组合键
		}
		
		isPressed = ((tmpFlag & SG_KEY_ButtonPressFlag)==tmpFlag) ? 1:0;
		KeyUpdataButtonSta(&(SG_KEY_CombStruct_List[i].Sta_Struct), isPressed, i+SG_KEY_Registered_Button_Count);
		if (SG_KEY_CombStruct_List[i].Sta_Struct.status==keySta_dummy)
		{
			SG_KEY_CombPressFlag &= ~(tmpFlag);
		}
		else
		{
			SG_KEY_CombPressFlag |= (tmpFlag);
		}
	}
	
	for (uint8_t i = 0; i < SG_KEY_Registered_Button_Count; i++)
	{
		tmpFlag = (uint32_t)1<<i;
		nowTime = Key_GetTime();
		if (tmpFlag & SG_KEY_CombPressFlag)
		{
			
			SG_KEY_ButtonStruct_List[i].Sta_Struct.status = keySta_dummy;
			SG_KEY_ButtonStruct_List[i].intoCombTime = nowTime;
			continue;//跳过触发组合键的按钮
		}
		diffTime = nowTime - SG_KEY_ButtonStruct_List[i].intoCombTime;
		if (diffTime > G_KEY_OUT_COMB_TIME)
		{
			//达到再次触发单键条件
			isPressed = ((tmpFlag & SG_KEY_ButtonPressFlag)==tmpFlag) ? 1:0;
			KeyUpdataButtonSta(&(SG_KEY_ButtonStruct_List[i].Sta_Struct), isPressed, i);
		}
		
	}


	KeyDetectPot();
	
}


/**
  * @brief	按键重复任务，建议10ms执行一次以保证按键响应速度
  * @param  
  * @retval  
**/
void Key_TickTask(void)
{
	for (uint8_t i = 0; i < SG_KEY_Registered_Button_Count; i++)
	{
		UpdataButtonIOLevel(&SG_KEY_ButtonStruct_List[i], i);
	}
}


/**
  * @brief	读取并更新按键GPIO电平
  * @param   Button_Struct: 需要更新的按键
  * @param   keyIndexBias: 按键在列表中的索引
  * @retval  
**/
static void UpdataButtonIOLevel(KEY_Button_TypeDef *Button_Struct, uint8_t keyIndexBias)
{

	uint8_t tmpLevel = KEY_GET_IO_STA(Button_Struct->IO_Struct.gpio, Button_Struct->IO_Struct.pin) ? 1:0;
	uint32_t nowTime = Key_GetTime();
	uint32_t timeDiff = nowTime - Button_Struct->IO_Struct.updataTime;
	if (tmpLevel!=Button_Struct->IO_Struct.currentLevel)
	{
		if (timeDiff > G_KEY_FILTER_TIME)
		{
			//消抖完成
			Button_Struct->IO_Struct.currentLevel = tmpLevel;
			Button_Struct->IO_Struct.updataTime = nowTime;
			if (tmpLevel==Button_Struct->IO_Struct.pressedLevel)
			{
				SG_KEY_ButtonPressFlag |= ((uint32_t)1 << keyIndexBias);
			}
			else
			{
				SG_KEY_ButtonPressFlag &= ~((uint32_t)1 << keyIndexBias);
			}
		}
	}
	
	return ;
}


/**
  * @brief	更新按键状态
  * @param   Sta_Struct: 需要更新的按键
  * @param   isPressed: 按键是否按下
  * @param   keyIndexBias: 按键在列表中的索引(组合键排在实体按键之后)
  * @retval  
**/
static void KeyUpdataButtonSta(KEY_STA_TypeDef* Sta_Struct, uint8_t isPressed, uint8_t keyIndexBias)
{
	uint32_t nowTime = Key_GetTime();
	uint32_t timeDiff = nowTime - Sta_Struct->updataTime;
	switch (Sta_Struct->status)
	{
	case keySta_dummy:
	{
		if (isPressed)
		{
			//按键被按下
			Sta_Struct->status = keySta_pressed;
			Sta_Struct->updataTime = nowTime;
			
			if (Sta_Struct->keyEnableFlag & KEY_Flag_P)
			{
				//对应回调使能，将回调标识写入FIFO 
				KeyWriteFIFO((KEY_FIFO_t)(Key1_P+(keyIndexBias*4)));
			}
		}
	}break;
	case keySta_pressed:
	{
		if (isPressed)
		{
			if (timeDiff > Sta_Struct->_LPFTime)
			{
				// 达到长按判断条件 
				Sta_Struct->status = keySta_longpress;
				Sta_Struct->updataTime = nowTime;
		
				//执行长按回调 
				if (Sta_Struct->keyEnableFlag & KEY_Flag_LP)
				{
					//对应回调使能，将回调标识写入FIFO 
					KeyWriteFIFO( (KEY_FIFO_t)(Key1_LP+(keyIndexBias*4)));
				}
			}
		}
		else
		{
			// 切换为短按/单击释放状态 
			Sta_Struct->status = keySta_release;
			Sta_Struct->updataTime = nowTime;
			// 执行单击释放回调 ,为确保双击不输出单击，需先判断是否触发双击
			// if (Sta_Struct->keyEnableFlag & KEY_Flag_R)
			// {
			// 	//对应回调使能，将回调标识写入FIFO 
			// 	KeyWriteFIFO( (KEY_FIFO_t)(Key1_R+(keyIndexBias*4)));
			// }
		}
	}break;
	case keySta_release:
	{
		if (timeDiff < G_KEY_DOUBLEPRESS_TIME)
		{
			//在双击间隔内
			if (isPressed)
			{
				//再次按下，切换为双击
				Sta_Struct->status = keySta_doublepress;
				Sta_Struct->updataTime = nowTime;
			}
		}
		else
		{
			// 超出双击时间间隔，视为单击，切换为空闲 
			Sta_Struct->status = keySta_dummy;
			Sta_Struct->updataTime = nowTime;

			// 执行单击释放回调 ,为确保双击不输出单击，需先判断是否触发双击
			if (Sta_Struct->keyEnableFlag & KEY_Flag_R)
			{
				//对应回调使能，将回调标识写入FIFO 
				KeyWriteFIFO( (KEY_FIFO_t)(Key1_R+(keyIndexBias*4)));
			}

			// 空闲回调 
		}
	}break;
	case keySta_longpress:
	{
		if (isPressed)
		{
			if (timeDiff > Sta_Struct->_LPRPeriod)
			{
				// 再次达到长按触发时间 再次执行长按回调 
				Sta_Struct->updataTime = nowTime;
				if (Sta_Struct->keyEnableFlag & KEY_Flag_LP)
				{
					//对应回调使能，将回调标识写入FIFO 
					KeyWriteFIFO( (KEY_FIFO_t)(Key1_LP+(keyIndexBias*4)));
				}
			}
		}
		else
		{
			// 切换为空闲/释放状态 
			Sta_Struct->status = keySta_dummy;
			Sta_Struct->updataTime = nowTime;

			// 长按释放回调 
			// 空闲回调 
			
		}
	}break;
	case keySta_doublepress:
	{
		if (isPressed)
		{
			if (timeDiff > Sta_Struct->_LPFTime)
			{
				// 达到长按判断条件，切换为长按状态 
				Sta_Struct->status = keySta_longpress;
				Sta_Struct->updataTime = nowTime;
	
				//执行长按回调 
				if (Sta_Struct->keyEnableFlag & KEY_Flag_LP)
				{
					//对应回调使能，将回调标识写入FIFO 
					KeyWriteFIFO( (KEY_FIFO_t)(Key1_LP+(keyIndexBias*4)));
				}
			}
		}
		else
		{
			//松开 切换为空闲/释放状态 
			Sta_Struct->status = keySta_dummy;
			Sta_Struct->updataTime = nowTime;

			//执行双击释放回调 
			if (Sta_Struct->keyEnableFlag & KEY_Flag_DPR)
			{
				//对应回调使能，将回调标识写入FIFO 
				KeyWriteFIFO( (KEY_FIFO_t)(Key1_DPR+(keyIndexBias*4)));
			}
		}
	}break;
	default:
		break;
	}


}


/**
  * @brief   扫描检测电位计按键,并更新FIFO
  * @param   
  * @retval  
**/
void KeyDetectPot(void)
{
	KEY_POT_Level_t tmp_keyLevel;

	uint32_t nowTime;
	uint32_t diffTime;
	KEY_POT_PCT_t tmpPct;
	nowTime = Key_GetTime();
	for (uint8_t i = 0; i < SG_KEY_Registered_Pot_Count; i++)
	{
		
		diffTime = nowTime - SG_KeyPotStruct_List[i].updataTime;

		if (diffTime > SG_KeyPotStruct_List[i]._RTime)
		{
			//达到更新要求
			tmp_keyLevel = KEY_getPotVal(i);
			tmpPct = KEY_calPotPct(tmp_keyLevel,i);
			if (BSP_KEY_CmpDiff(tmpPct, SG_KeyPotStruct_List[i].pct_value, SG_KeyPotStruct_List[i]._PctStep))
			{
				continue;
			}
			
			SG_KeyPotStruct_List[i].pct_value = tmpPct;
			KeyWriteFIFO((KEY_FIFO_t)((KEY_POT_FIFO_FLAG|(SG_KeyPotStruct_List[i].pct_value&KEY_POT_FIFO_MASK))|(i<<KEY_POT_FIFO_I_BIT_S)));
			SG_KeyPotStruct_List[i].updataTime = nowTime;
		}
		
	}

}


/**
  * @brief    读取电位计数值
  * @param    index: 电位计索引
  * @retval   读取的数值
**/
static KEY_POT_Level_t KEY_getPotVal(uint8_t index)
{

	uint16_t val;
	KEY_GET_POT_VAL(&val, index);

	return ((KEY_POT_Level_t)val);
}


/**
  * @brief    计算电位计数值百分比
  * @param    level: 电位计数值
  * @param    index: 电位计索引
  * @retval   百分比
**/
static KEY_POT_PCT_t KEY_calPotPct(KEY_POT_Level_t level ,uint8_t index)
{
	KEY_POT_PCT_t pct;
	if (level > SG_KeyPotStruct_List[index]._PLLevel )
	{
		pct = (KEY_POT_PCT_t)(((float)(level - SG_KeyPotStruct_List[index]._PLLevel)*100.0)/ SG_KeyPotStruct_List[index]._PRange);
	}
	else if (level < SG_KeyPotStruct_List[index]._NHLevel)
	{
		pct = (KEY_POT_PCT_t)(-((float)(SG_KeyPotStruct_List[index]._NHLevel - level)*100.0)/ SG_KeyPotStruct_List[index]._NRange);
	}
	else
	{
		pct = 0;
	}
	return pct;

}


/**
  * @brief    比较两数的差值大小
  * @param    A: 
  * @param    B: 
  * @param    diff: 需比较的差值
  * @retval   0：|A-B| > diff  else: |A-B| <= diff
**/
static uint8_t BSP_KEY_CmpDiff(int16_t A, int16_t B, uint16_t diff)
{
	uint8_t rVal = 0;

	rVal = (abs(A-B)>diff) ? 0 : 1;

	return rVal;
}


/**
  * @brief   电位计按键结构体初始化函数
  * @param   key[IN/OUT]:需要按默认值初始化的结构体
  * @param   ADCx[IN]:按键使用的ADC
  * @param   ADC_Channel_x[IN]:按键使用的ADC通道
  * @retval  
**/
void KEY_Pot_StructInit(KEY_Pot_TypeDef* key, ADC_TypeDef* ADCx, uint8_t ADC_Channel_x)
{

	key->ADCx = ADCx;
	key->ADC_Channel_x = ADC_Channel_x;
	key->_PRange = G_KeyPot_PR;
	key->_PLLevel = G_KeyPot_PLL;
	key->_NHLevel = G_KeyPot_NHL;
	key->_NRange = G_KeyPot_NR;
	key->_RTime = G_KeyPot_RTime;
	key->_PctStep = G_KeyPot_PctStep;
	key->pct_value = 0;
	key->updataTime = 0;


}


/**
  * @brief   电位计按键结构体初始化函数
  * @param   Index [IN]:该按键在按键列表中的索引
  * @param   potStruct[IN]:按键
  * @retval  0:成功，else：失败
**/
uint8_t BSP_KEY_Pot_Register(uint8_t Index, KEY_Pot_TypeDef potStruct)
{
	uint8_t rVal = 1;
	if (Index < SG_KEY_Registered_Pot_Count)
	{
		SG_KeyPotStruct_List[Index] = potStruct;
		rVal = 0;
	}
	
	return rVal;

}




