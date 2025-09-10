
#include "bsp_key.h"
#include "bsp_gpio.h"


uint8_t Registered_KEY_Count = 0;//已注册的KEY个数			  				  
uint8_t KEY_NUM_List[MAX_KEY_NUM];
GPIO_TypeDef* KEY_GPIO_POTRx_List[MAX_KEY_NUM];
uint16_t KEY_GPIO_PINx_List[MAX_KEY_NUM];


const uint8_t KEY_IOscan10ms_ON=KEY_IOscan10_ON;//是否开启IO扫描

uint8_t G_key_TesksTick = 0; // 按键任务运行计时

/**********************user defined parameters start***********************/

/*每个按键所属的组合键的索引及在该组合键中的标识，用于初始化按键IO对象列表*/
//锚键所属的组合键索引
static const uint8_t SCG_CombM1IndexList[3] = {0, 1, 2};
static const uint8_t SCG_CombM2IndexList[3] = {0, 3, 4};

//锚键在对应组合键中的标识
static const uint8_t SCG_CombM1FlagList[3] = {G_KEYCOMBANCHOR_FLAG, G_KEYCOMBANCHOR_FLAG, G_KEYCOMBANCHOR_FLAG};
static const uint8_t SCG_CombM2FlagList[3] = {0x01, G_KEYCOMBANCHOR_FLAG, G_KEYCOMBANCHOR_FLAG};

//普通按键所属的组合键索引
static const uint8_t SCG_CombUpIndexList[1] = {3};
static const uint8_t SCG_CombDownIndexList[1] = {4};
static const uint8_t SCG_CombAIndexList[1] = {1};
static const uint8_t SCG_CombBIndexList[1] = {2};

//普通按键在对应组合键中的标识
static const uint8_t SCG_CombUpFlagList[1] = {0x01};
static const uint8_t SCG_CombDownFlagList[1] = {0x01};
static const uint8_t SCG_CombAFlagList[1] = {0x01};
static const uint8_t SCG_CombBFlagList[1] = {0x01};

//按键IO对象列表(不包括组合键锚键)
static KEY_GPIO_t SG_KeyIOList[G_KEY_NUM]=
{
	{GPIOA, GPIO_Pin_0, Pressed_Level_Low, 1, SCG_CombUpIndexList, SCG_CombUpFlagList},	//KEY_UP
	{GPIOA, GPIO_Pin_1, Pressed_Level_Low, 1, SCG_CombDownIndexList, SCG_CombDownFlagList},	//KEY_DOWN
	{GPIOA, GPIO_Pin_2, Pressed_Level_Low, 0},	//KEY_LEFT
	{GPIOA, GPIO_Pin_3, Pressed_Level_Low, 0},	//KEY_RIGHT
	{GPIOB, GPIO_Pin_12, Pressed_Level_Low, 1, SCG_CombAIndexList, SCG_CombAFlagList},	//KEY_A
	{GPIOB, GPIO_Pin_13, Pressed_Level_Low, 1, SCG_CombBIndexList, SCG_CombBFlagList},	//KEY_B
	{GPIOB, GPIO_Pin_14, Pressed_Level_Low, 0},	//KEY_C
	{GPIOB, GPIO_Pin_15, Pressed_Level_Low, 0},	//KEY_D


};

//锚键IO对象列表
#if KEY_COMB_ON //
static KEY_GPIO_t SG_CombAnchorKeyIOList[G_KEYCOMBANCHOR_NUM]=
{
	{GPIOB, GPIO_Pin_10, Pressed_Level_Low, 3, SCG_CombM1IndexList, SCG_CombM1FlagList},	//KEY_M1
	{GPIOB, GPIO_Pin_11, Pressed_Level_Low, 3, SCG_CombM2IndexList, SCG_CombM2FlagList},	//KEY_M2

};
#endif //KEY_COMB_ON

/*定义按键回调使能标识列表，普通按键在前，组合键在后，标识对应 KEY_CALLBACK_FUN_t
所展示的回调，若使能对应回调则需将标识的对应位置1*/
static uint8_t SG_KeyEnableFlag[G_KEY_NUM+G_KEYCOMB_NUM]=
{
	0x0e,//key1，使能序号123的回调
	0x0e,//key2
	0x0e,//key3
	0x0e,//key4
	0x0e,//key5
	0x0e,//key6
	0x0e,//key7
	0x0e,//key8
	0x0e,//key9
	0x01,//key10, 使能序号0的回调
	0x01,//key11
	0x01,//key12
	0x01,//key13
};

/**********************user defined parameters end***********************/

#if KEY_USE_DEFAULT_CBF //使用默认按键回调函数
/* 按键状态回调函数列表 KeyUpLongPressedCBF */
static KEY_CALLBACK_FUN_t SG_KeyCallBackFunList[G_KEYCBFLIST_NUM]=
{
	{0, KeyUpReleaseCBF, 	 KeyUpDoubleReleaseCBF, 	KeyUpLongPressedCBF},	//KEY_UP
	{0, KeyDownReleaseCBF,  KeyDownDoubleReleaseCBF,  KeyDownLongPressedCBF},  //KEY_DOWN
	{0, KeyLeftReleaseCBF,  KeyLeftDoubleReleaseCBF,  KeyLeftLongPressedCBF},  //KEY_LEFT
	{0, KeyRightReleaseCBF, KeyRightDoubleReleaseCBF, KeyRightLongPressedCBF}, //KEY_RIGHT
	{0, KeyAReleaseCBF, KeyADoubleReleaseCBF, KeyALongPressedCBF},	//KEY_A
	{0, KeyBReleaseCBF, KeyBDoubleReleaseCBF, KeyBLongPressedCBF},	//KEY_B
	{0, KeyCReleaseCBF, KeyCDoubleReleaseCBF, KeyCLongPressedCBF},	//KEY_C
	{0, KeyDReleaseCBF, KeyDDoubleReleaseCBF, KeyDLongPressedCBF},	//KEY_D
	{0, KeyComb1ReleaseCBF, KeyComb1DoubleReleaseCBF, KeyComb1LongPressedCBF},	//KEY_Comb1
};
#endif //KEY_USE_DEFAULT_CBF

static KEY_STA_t SG_KeyStaList[G_KEY_NUM]=
{
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//UP
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//DOWN
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//LEFT
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//RIGHT
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//A
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//B
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//C
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//D
};

#if KEY_COMB_ON //
//组合键状态对象列表
static KEY_STA_t SG_KeyCombStaList[G_KEYCOMB_NUM]=
{
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//M1,M2
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//M1,A
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//M1,C
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//M2,UP
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//M2,DOWN
};

//锚键状态对象列表
static KEY_STA_t SG_AnchorKeyStaList[G_KEYCOMBANCHOR_NUM]=
{
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//KEY_M1
	{ keySta_dummy, 0, 0, 0, G_KEY_LP_FT, 0, G_KEY_LP_RT, 0},	//KEY_M2
};


//组合键实时标识列表
static uint8_t SG_CombKeyFlagList[G_KEYCOMB_NUM]=
{
0x00,
0x00,
0x00,
0x00,
0x00
};

//组合键触发标识列表
static uint8_t SG_CombKeyTrigFlagList[G_KEYCOMB_NUM]=
{
0x81,
0x81,
0x81,
0x81,
0x81
};
#endif //KEY_COMB_ON


/* 按键回调标识FIFO储存器 */
static KEY_Flag_enumt SG_KeyCBFlagFIFO[KEY_FIFO_CAPACITY]={KeyDummy};

static KEY_FIFORW_t SG_KeyFifoRW_Obj; 


/*****************************************************************/

/**
  * @brief   按键测试程序
  * @param   
  * @retval  
**/
void KeyTest(void)
{

	KeyInit();
	KeyGpioConfig();
	KEY_Flag_enumt rFlag;

	uint8_t tmpIndex;
	while (1)
	{
		
		do
		{
			/* 一直读取FIFO */
			rFlag = KeyReadFIFO();
			if (rFlag != KeyDummy)
			{
				/* 读取到有效标识 */
				tmpIndex = rFlag/4;
				if (tmpIndex < 9)
				{
					/* 按键回调较全，采用回调函数列表方式执行 */
					#if KEY_USE_DEFAULT_CBF //使用默认按键回调函数
					KeyCallBackListExe(SG_KeyCallBackFunList, rFlag);
					#else
					KeyCallBackListExe(UserDefined_KeyCallBackFunList, rFlag%4);
					#endif //KEY_USE_DEFAULT_CBF
				}
				else
				{
					/* 按键回调较少较分散，采用switch执行 */
					KeyCallBackSwitchExe(rFlag);
				}
			}
		} while (rFlag);

	}
	
	//return;
}


/**
  * @brief   通过回调函数列表形式执行按键回调
  * @param   CBFList //用户自定义按键回调列表
  * @param   i //需要执行的回调函数在列表中的索引
  * @retval  
**/
void KeyCallBackListExe(KEY_CALLBACK_FUN_t *CBFList, uint8_t i)
{
	switch (i%4)
	{
	case Key1_P:
	{
		/* code */
		CBFList[i/KEY_FIFOFLAG_NUM].pressedFun();
	}break;
	case Key1_R:
	{
		/* code */
		CBFList[i/KEY_FIFOFLAG_NUM].releaseFun();
	}break;
	case Key1_DPR:
	{
		/* code */
		CBFList[i/KEY_FIFOFLAG_NUM].doubleReleaseFun();
	}break;
	case Key1_LP:
	{
		/* code */
		CBFList[i/KEY_FIFOFLAG_NUM].longPressedFun();
	}break;
	
	default:
		break;
	}
	
	
	return;
}


/**
  * @brief   通过回调函数列表形式执行按键回调
  * @param   keyFlag //按键回调标识
  * @retval  
**/
void KeyCallBackSwitchExe(KEY_Flag_enumt keyFlag )
{
	switch (keyFlag)
	{
	case Key10_P:
	{
		/* Key10 单击按下回调标识 */
		KeyComb2PressedCBF();
	}break;
	case Key11_P:
	{
		/* Key11 单击按下回调标识 */
		KeyComb3PressedCBF();
	}break;
	case Key12_P:
	{
		/* Key12 单击按下回调标识 */
		KeyComb4PressedCBF();
	}break;
	case Key13_P:
	{
		/* Key13 单击按下回调标识 */
		KeyComb5PressedCBF();
	}break;
	
	default:
		break;
	}
	return;
}


/**
  * @brief   读取按键FIFO里的Flag
  * @param   Flag 返回的Flag
  * @param   maxFlagNum 最多返回多少个Flag
  * @retval  0：未读取到Flag，else：实际返回的Flag个数
**/
uint8_t KeyGetFIFOFlag(uint8_t *Flag, uint8_t maxFlagNum)
{
	uint8_t ret = 0;
	KEY_Flag_enumt rFlag;
	do
	{
		/* 一直读取FIFO */
		rFlag = KeyReadFIFO();
		if (rFlag != KeyDummy)
		{
			/* 读取到有效标识 */
			Flag[ret] = (uint8_t)(rFlag);
			ret++;
		}
	} while ((rFlag != KeyDummy)&&(ret<maxFlagNum));

	return ret;
}


/**
  * @brief   按键初始化
  * @param   
  * @retval  
**/
void KeyInit(void)
{

	for (uint8_t i = 0; i < G_KEY_NUM; i++)
	{
		/* 普通按键状态参数初始化 */
		SG_KeyStaList[i].status = keySta_dummy;
		SG_KeyStaList[i].pressFilteringTime = 0;
		SG_KeyStaList[i].releaseFilteringTime = 0;
		SG_KeyStaList[i].longPressTime = 0;
		SG_KeyStaList[i]._LPFTime = G_KEY_LP_FT;
		SG_KeyStaList[i].longPressCount = 0;
		SG_KeyStaList[i]._LPRTime = G_KEY_LP_RT;
		SG_KeyStaList[i].doublePressTime = 0;
	}

	#if KEY_COMB_ON //

	for (uint8_t i = 0; i < G_KEYCOMBANCHOR_NUM; i++)
	{
		/* 锚键状态参数初始化 */
		SG_AnchorKeyStaList[i].status = keySta_dummy;
		SG_AnchorKeyStaList[i].pressFilteringTime = 0;
		SG_AnchorKeyStaList[i].releaseFilteringTime = 0;
		SG_AnchorKeyStaList[i].longPressTime = 0;
		SG_AnchorKeyStaList[i]._LPFTime = G_KEY_LP_FT;
		SG_AnchorKeyStaList[i].longPressCount = 0;
		SG_AnchorKeyStaList[i]._LPRTime = G_KEY_LP_RT;
		SG_AnchorKeyStaList[i].doublePressTime = 0;
	}
	#endif //KEY_COMB_ON


	KeyGpioConfig();
	KeyClearFIFO();

	for (uint8_t i = 0; i < G_KEYCOMB_NUM; i++)
	{
		/* 组合键状态标识复位 */
		SG_CombKeyFlagList[i] = 0;
	}
	

	return;
}


/**
  * @brief   按键IO及按下电平配置
  * @param   
  * @retval  
**/
void KeyGpioConfig(void)
{
	GPIOMode_TypeDef GPIO_Mode_tmp;
	for (uint8_t i = 0; i < G_KEY_NUM; i++)
	{
		/* code */
		if (SG_KeyIOList[i].pressedLevel == Pressed_Level_High)
		{
			GPIO_Mode_tmp = GPIO_Mode_IPD;
		}
		else
		{
			GPIO_Mode_tmp = GPIO_Mode_IPU;
		}
		
		BSP_GPIO_Config(SG_KeyIOList[i].gpio, SG_KeyIOList[i].pin, GPIO_Mode_tmp);
	}

	#if KEY_COMB_ON // 

	for (uint8_t i = 0; i < G_KEYCOMBANCHOR_NUM; i++)
	{
		/* code */
		if (SG_CombAnchorKeyIOList[i].pressedLevel == Pressed_Level_High)
		{
			GPIO_Mode_tmp = GPIO_Mode_IPD;
		}
		else
		{
			GPIO_Mode_tmp = GPIO_Mode_IPU;
		}
		
		BSP_GPIO_Config(SG_CombAnchorKeyIOList[i].gpio, SG_CombAnchorKeyIOList[i].pin, GPIO_Mode_tmp);
	}
	#endif //KEY_COMB_ON

	return;
}


/**
  * @brief   修改按键状态判断参数
  * @param   KeyStaObj : 按键状态对象 
  * @param   LPFTime : 长按滤波时间/10ms 
  * @param   LPRTime : 长按重复触发间隔/10ms 
  * @retval  
**/
void KeySetParam(KEY_STA_t *KeyStaObj , uint8_t LPFTime, uint8_t LPRTime)
{
	(*KeyStaObj)._LPFTime = LPFTime;
	(*KeyStaObj)._LPRTime = LPRTime;
	return;
}


/**
  * @brief   按键任务扫描周期处理函数
  * @param   
  * @retval  
**/
void KeyTesks_SysTick_Handler(void)
{
    G_key_TesksTick++;
    if (G_key_TesksTick >= KEY_TESKS_PERIOD)
    {
		G_key_TesksTick = 0;
		KeyScan10ms();
    }

	return;
}


/**
  * @brief   扫描检测组合键锚键
  * @param   
  * @retval  
**/
void KeyDetectAnchor(void)
{
	uint16_t tmpPPLevel;
	uint8_t tmpIndex;
	uint8_t tmpCombFlag;
	for (uint8_t i = 0; i < G_KEYCOMBANCHOR_NUM; i++)
	{
		/* 分别扫描每个按键的状态，切换时重置下一状态所使用参数 */
		tmpPPLevel = SG_CombAnchorKeyIOList[i].gpio->IDR & SG_CombAnchorKeyIOList[i].pin;

		switch (SG_AnchorKeyStaList[i].status)
		{
			/* 锚键只有按下（keySta_pressed）和释放（keySta_dummy）两种状态  */
		case keySta_dummy:
		{
			/* 锚键空闲 */
			if (tmpPPLevel == SG_CombAnchorKeyIOList[i].pressedLevel)
			{
				/* 按键被按下，开始滤波 */
				SG_AnchorKeyStaList[i].pressFilteringTime++;
				if (SG_AnchorKeyStaList[i].pressFilteringTime>G_KEY_FILTER_TIME)
				{
					/* 滤波时间达到要求，重置滤波时间，修改组合键实时标志及锚键状态 */
					for (uint8_t j = 0; j < SG_CombAnchorKeyIOList[i].keyCombShareNum; j++)
					{
						/* 修改锚键所属的所有组合键的实时标志 */
						tmpIndex = SG_CombAnchorKeyIOList[i].keyCombIndexList[j];
						SG_CombKeyFlagList[tmpIndex] |= SG_CombAnchorKeyIOList[i].keyCombFlagList[j];
						SG_KeyCombStaList[tmpIndex].pressFilteringTime = 0;
					
					}
					
					SG_AnchorKeyStaList[i].status = keySta_pressed;
					SG_AnchorKeyStaList[i].releaseFilteringTime = 0;

					/* 锚键 单击/短按 按下回调 */
				}
				
			}
			else
			{
				/* 存在抖动，重置消抖 */
				SG_AnchorKeyStaList[i].pressFilteringTime = 0;
			}
		}break;
		case keySta_pressed:
		{
			/* 已经是按下状态 */
			if (tmpPPLevel != SG_CombAnchorKeyIOList[i].pressedLevel)
			{
				/* 按键松开，开始滤波 */
				SG_AnchorKeyStaList[i].releaseFilteringTime++;
				if (SG_AnchorKeyStaList[i].releaseFilteringTime>G_KEY_FILTER_TIME)
				{
					/* 滤波时间达到要求，重置滤波时间，修改组合键实时标志及锚键状态 */
					for (uint8_t j = 0; j < SG_CombAnchorKeyIOList[i].keyCombShareNum; j++)
					{
						/* 修改锚键所属的所有组合键的实时标志 */
						tmpCombFlag = SG_CombAnchorKeyIOList[i].keyCombFlagList[j];
						if ( tmpCombFlag == G_KEYCOMBANCHOR_FLAG)
						{
							/* 该键为组合键中的锚键 */
							SG_CombKeyFlagList[SG_CombAnchorKeyIOList[i].keyCombIndexList[j]] = 0;//清除所有标志
						}
						else
						{
							/* 该键在组合键中作为普通按键 */
							SG_CombKeyFlagList[SG_CombAnchorKeyIOList[i].keyCombIndexList[j]] &= ~(tmpCombFlag);//清除所有标志
						}
					}
					SG_AnchorKeyStaList[i].status = keySta_dummy;
					SG_AnchorKeyStaList[i].pressFilteringTime = 0;

					/* 锚键 单击/短按 释放回调 */
				}

			}
			else
			{
				/* 存在抖动，重置消抖 */
				SG_AnchorKeyStaList[i].releaseFilteringTime = 0;
			}
			
		}break;
		default:
			break;
		}
			
	}
	
	return;
}


/**
  * @brief   扫描检测普通按键
  * @param   
  * @retval  
**/
void KeyDetectCommon(void)
{
	uint16_t tmpPPLevel;
	uint8_t tmpIndex;
	uint8_t tmpFlag=0;
	uint8_t tmpPressFilteringOk;
	uint8_t tmpReleaseFilteringOk;
	for (uint8_t i = 0; i < G_KEY_NUM; i++)
	{
		/* 分别扫描每个按键的状态，切换时重置下一状态所使用参数 */
		tmpPPLevel = SG_KeyIOList[i].gpio->IDR & SG_KeyIOList[i].pin;
	#if KEY_COMB_ON //
		tmpFlag=0;
		tmpPressFilteringOk = 0;
		tmpReleaseFilteringOk = 0;
		if (SG_KeyIOList[i].keyCombShareNum > 0)
		{
			/* 属于某组合键 */
			for (uint8_t j = 0; j < SG_KeyIOList[i].keyCombShareNum; j++)
			{
				/* 遍历按键所属的每个组合键 */
				tmpIndex = SG_KeyIOList[i].keyCombIndexList[j];
				if (SG_CombKeyFlagList[tmpIndex] & G_KEYCOMBANCHOR_FLAG)
				{
					/* 锚键已按下，进入组合键模式 */
					tmpFlag = 1;
					if (SG_CombKeyFlagList[tmpIndex]&SG_KeyIOList[i].keyCombFlagList[j])
					{
						/* 在组合键中 已为按下状态 等待释放*/
						if (tmpPPLevel == SG_KeyIOList[i].pressedLevel)
						{
							/* 存在抖动 ，重置滤波*/
							SG_KeyStaList[i].releaseFilteringTime=0;
						}
						else
						{
							/* 按键松开，开始滤波 */
							SG_KeyStaList[i].releaseFilteringTime++;
							if (tmpReleaseFilteringOk||(SG_KeyStaList[i].releaseFilteringTime>G_KEY_FILTER_TIME))
							{
								/* 滤波时间达到要求，修改组合键标志 */
								SG_CombKeyFlagList[tmpIndex] &= ~(SG_KeyIOList[i].keyCombFlagList[j]); 
								SG_KeyStaList[i].pressFilteringTime = 0;
								tmpReleaseFilteringOk = 1;
								SG_KeyStaList[i].releaseFilteringTime = 0;//防止溢出
							}
							else
							{
								/* 未达到滤波时间，结束当前循环 */
								break;
							}
						}
					}
					else
					{
						/* 在组合键中 为未按下状态 等待按下 */
						if (tmpPPLevel == SG_KeyIOList[i].pressedLevel)
						{
							/* 按键按下 开始滤波 */
							SG_KeyStaList[i].pressFilteringTime++;
							if (tmpPressFilteringOk||(SG_KeyStaList[i].pressFilteringTime>G_KEY_FILTER_TIME))
							{
								/* 滤波时间达到要求，修改组合键标志 */
								SG_CombKeyFlagList[tmpIndex] |= SG_KeyIOList[i].keyCombFlagList[j];
								SG_KeyStaList[i].releaseFilteringTime = 0;
								tmpPressFilteringOk = 1;
								SG_KeyStaList[i].pressFilteringTime = 0;//防止溢出
							}
							else
							{
								/* 未达到滤波时间，结束当前循环 */
								break;
							}
						}
						else
						{
							/* 存在抖动 ，重置滤波*/
							SG_KeyStaList[i].pressFilteringTime=0;
						}
					}
				}
			}
		}
		if (tmpFlag)
		{
			/* 锚键已按下，进入组合键模式 */
			continue;
		}
	#endif //KEY_COMB_ON

		switch (SG_KeyStaList[i].status)
		{
		/* 独立按键模式 */
		case keySta_dummy:
		{
			/* 空闲 */
			if (tmpPPLevel == SG_KeyIOList[i].pressedLevel)
			{
				/* 按键被按下,开始滤波 */
				SG_KeyStaList[i].pressFilteringTime++;
				if (SG_KeyStaList[i].pressFilteringTime>G_KEY_FILTER_TIME)
				{
					/* 滤波时间达到要求，切换为按下状态  */
					SG_KeyStaList[i].status = keySta_pressed;
					SG_KeyStaList[i].releaseFilteringTime = 0;
					SG_KeyStaList[i].longPressTime = 0;
					
					/* 独立按键 单击按下回调 */
					if (SG_KeyEnableFlag[i] & (0x01<<Key1_P))
					{
						/* 对应回调使能，将回调标识写入FIFO */
						KeyWriteFIFO((KEY_Flag_enumt)(Key1_P+(i*4)));
					}
					
				}
			}
			else
			{
				/* 存在抖动，重置消抖 */
				SG_KeyStaList[i].pressFilteringTime = 0;
			}
		}break;
		case keySta_pressed:
		{
			/* 已经是按下状态 */
			if (tmpPPLevel == SG_KeyIOList[i].pressedLevel)
			{
				/* 继续按下 长按计时 */
				SG_KeyStaList[i].longPressTime++;
				SG_KeyStaList[i].releaseFilteringTime = 0;//重置释放消抖
				if (SG_KeyStaList[i].longPressTime > SG_KeyStaList[i]._LPFTime)
				{
					/* 达到长按判断条件，重置判断时间，切换为长按状态 */
					SG_KeyStaList[i].status = keySta_longpress;
					SG_KeyStaList[i].longPressCount = 0;
					SG_KeyStaList[i].releaseFilteringTime = 0;

					/* 执行长按回调 */
					if (SG_KeyEnableFlag[i] & (0x01<<Key1_LP))
					{
						/* 对应回调使能，将回调标识写入FIFO */
						KeyWriteFIFO( (KEY_Flag_enumt)(Key1_LP+(i*4)));
					}
					
				}
			}
			else
			{
				/* 松开，软件消抖滤波 */
				SG_KeyStaList[i].releaseFilteringTime++;
				SG_KeyStaList[i].longPressTime = 0;//重置长按判断
				if (SG_KeyStaList[i].releaseFilteringTime > G_KEY_FILTER_TIME)
				{
					/* 达到消抖时间，重置消抖时间，切换为短按/单击释放状态 */
					SG_KeyStaList[i].status = keySta_release;
					SG_KeyStaList[i].doublePressTime = 0;
					SG_KeyStaList[i].pressFilteringTime = 0;

					/* 执行单击释放回调 */
					if (SG_KeyEnableFlag[i] & (0x01<<Key1_LP))
					{
						/* 对应回调使能，将回调标识写入FIFO */
						KeyWriteFIFO( (KEY_Flag_enumt)(Key1_R+(i*4)));
					}
				}
			}
			
		}break;
		case keySta_release:
		{
			/* 短按释放*/

			if (SG_KeyStaList[i].doublePressTime < G_KEY_DOUBLEPRESS_TIME)
			{
				/* 在双击间隔内 */
				if (tmpPPLevel == SG_KeyIOList[i].pressedLevel)
				{
					/* 再次按下，开始消抖 */
					SG_KeyStaList[i].pressFilteringTime++;
					if (SG_KeyStaList[i].pressFilteringTime > G_KEY_FILTER_TIME)
					{
						/* 消抖完成 */
						if (SG_KeyStaList[i].doublePressTime < G_KEY_DOUBLEPRESS_TIME)
						{
							/* 在双击时间间隔内再次按下，切换为双击 */
							SG_KeyStaList[i].status = keySta_doublepress;
							SG_KeyStaList[i].longPressTime = 0;
							SG_KeyStaList[i].releaseFilteringTime = 0;

							/* 双击按下回调 */
						}
					}
				}
				else
				{
					/* 未按下，双击间隔计时 */
					SG_KeyStaList[i].doublePressTime++;
					SG_KeyStaList[i].pressFilteringTime=0;
				}
			}
			else
			{
				/* 超出双击时间间隔，切换为空闲 */
				SG_KeyStaList[i].pressFilteringTime = 0;
				SG_KeyStaList[i].status = keySta_dummy;

				/* 空闲回调 */
			}
			
		}break;
		case keySta_longpress:
		{
			/*  */
			if (tmpPPLevel == SG_KeyIOList[i].pressedLevel)
			{
				/* 仍然按下，长按计数 */
				SG_KeyStaList[i].longPressCount++;
				SG_KeyStaList[i].releaseFilteringTime = 0;//重置释放消抖
				if (SG_KeyStaList[i].longPressCount > SG_KeyStaList[i]._LPRTime)
				{
					/* 再次达到长按触发时间，重置触发计数 */
					SG_KeyStaList[i].longPressCount = 0;

					/* 再次执行长按回调 */
					if (SG_KeyEnableFlag[i] & (0x01<<Key1_LP))
					{
						/* 对应回调使能，将回调标识写入FIFO */
						KeyWriteFIFO( (KEY_Flag_enumt)(Key1_LP+(i*4)));
					}
				}
			}
			else
			{
				/* 松开，软件消抖滤波 */
				SG_KeyStaList[i].releaseFilteringTime++;
				SG_KeyStaList[i].longPressCount=0;
				if (SG_KeyStaList[i].releaseFilteringTime > G_KEY_FILTER_TIME)
				{
					/* 达到消抖时间，重置消抖时间，切换为空闲/释放状态 */
					SG_KeyStaList[i].status = keySta_dummy;
					SG_KeyStaList[i].pressFilteringTime = 0;

					/* 长按释放回调 */
					/* 空闲回调 */
				}
			}
		}break;
		case keySta_doublepress:
		{
			/* 双击 */
			if (tmpPPLevel == SG_KeyIOList[i].pressedLevel)
			{
				/* 继续按下，开始长按判断 */
				SG_KeyStaList[i].longPressTime++;
				SG_KeyStaList[i].releaseFilteringTime = 0;//重置释放消抖
				if (SG_KeyStaList[i].longPressTime > SG_KeyStaList[i]._LPFTime)
				{
					/* 达到长按判断条件，重置判断时间，切换为长按状态 */
					SG_KeyStaList[i].status = keySta_longpress;
					SG_KeyStaList[i].longPressCount = 0;
					SG_KeyStaList[i].releaseFilteringTime = 0;

					/* 执行长按回调 */
					if (SG_KeyEnableFlag[i] & (0x01<<Key1_LP))
					{
						/* 对应回调使能，将回调标识写入FIFO */
						KeyWriteFIFO( (KEY_Flag_enumt)(Key1_LP+(i*4)));
					}
				}
				
			}
			else
			{
				/* 松开，软件消抖滤波 */
				SG_KeyStaList[i].releaseFilteringTime++;
				SG_KeyStaList[i].longPressTime = 0;//重置长按判断
				if (SG_KeyStaList[i].releaseFilteringTime > G_KEY_FILTER_TIME)
				{
					/* 达到消抖时间，重置按下消抖时间，切换为空闲/释放状态 */
					SG_KeyStaList[i].status = keySta_dummy;
					SG_KeyStaList[i].pressFilteringTime = 0;

					/* 执行双击释放回调 */
					if (SG_KeyEnableFlag[i] & (0x01<<Key1_DPR))
					{
						/* 对应回调使能，将回调标识写入FIFO */
						KeyWriteFIFO( (KEY_Flag_enumt)(Key1_DPR+(i*4)));
					}
				}
			}
		}break;
		default:
			break;
		}
	}
	
	return;
}


/**
  * @brief   扫描检测组合按键
  * @param   
  * @retval  
**/
void KeyDetectCombination(void)
{
	uint8_t tmpCombTrigFlag;
	for (uint8_t i = 0; i < G_KEYCOMB_NUM; i++)
	{
		/* 扫描所有组合键 */
		tmpCombTrigFlag = ( SG_CombKeyFlagList[i] == SG_CombKeyTrigFlagList[i] );

		switch (SG_KeyCombStaList[i].status)
		{
		case keySta_dummy:
		{	
			/* 空闲 */
			if (tmpCombTrigFlag)
			{
				/* 组合键触发，切换为按下状态(锚键独立按键已滤波 组合键无需滤波) */
				SG_KeyCombStaList[i].status = keySta_pressed;
				SG_KeyCombStaList[i].longPressTime = 0;

				/* 单击/短按 按下回调 */
				if (SG_KeyEnableFlag[G_KEY_NUM+i] & (0x01<<Key1_P))
				{
					/* 对应回调使能，将回调标识写入FIFO */
					KeyWriteFIFO( (KEY_Flag_enumt)(Key1_P+((G_KEY_NUM+i)*4)));
				}
			}
		}break;
		case keySta_pressed:
		{	
			/* 按下 */
			if (tmpCombTrigFlag)
			{
				/* 仍然按下，长按计时 */
				SG_KeyCombStaList[i].longPressTime++;
				if (SG_KeyCombStaList[i].longPressTime>SG_KeyCombStaList[i]._LPFTime)
				{
					/* 长按滤波达到，切换为长按状态 */
					SG_KeyCombStaList[i].status = keySta_longpress;
					SG_KeyCombStaList[i].longPressCount = 0;

					/* 长按按下回调 */
					if (SG_KeyEnableFlag[G_KEY_NUM+i] & (0x01<<Key1_LP))
					{
						/* 对应回调使能，将回调标识写入FIFO */
						KeyWriteFIFO( (KEY_Flag_enumt)(Key1_LP+((G_KEY_NUM+i)*4)));
					}
				}
			}
			else
			{
				/* 松开，切换为单击/短按释放 */
				SG_KeyCombStaList[i].status = keySta_release;
				SG_KeyCombStaList[i].doublePressTime = 0;

				/* 单击/短按 释放回调 */
				if (SG_KeyEnableFlag[G_KEY_NUM+i] & (0x01<<Key1_R))
				{
					/* 对应回调使能，将回调标识写入FIFO */
					KeyWriteFIFO( (KEY_Flag_enumt)(Key1_R+((G_KEY_NUM+i)*4)));
				}
			}
		}break;
		case keySta_release:
		{	
			/* 单击/短按 释放，双击间隔计时 */
			SG_KeyCombStaList[i].doublePressTime++;
			if (SG_KeyCombStaList[i].doublePressTime < G_KEY_DOUBLEPRESS_TIME)
			{
				/* 双击间隔内 */
				if (tmpCombTrigFlag)
				{
					/* 再次按下，切换为双击按下 */
					SG_KeyCombStaList[i].status = keySta_doublepress;
					SG_KeyCombStaList[i].longPressTime = 0;

					/* 双击按下回调 */
					
				}
			}
			else
			{
				/* 超出双击间隔，切换为空闲 */
				SG_KeyCombStaList[i].status = keySta_dummy;

				/* 空闲回调 */
			}
		}break;
		case keySta_longpress:
		{	
			/* 长按， */
			if (tmpCombTrigFlag)
			{
				/* 继续按下 长按重复触发计数 */
				SG_KeyCombStaList[i].longPressCount++;
				if (SG_KeyCombStaList[i].longPressCount > SG_KeyCombStaList[i]._LPRTime)
				{
					/* 达到重复触发条件 */
					SG_KeyCombStaList[i].longPressCount=0;
	
					/* 长按重复触发回调 */
					if (SG_KeyEnableFlag[G_KEY_NUM+i] & (0x01<<Key1_LP))
					{
						/* 对应回调使能，将回调标识写入FIFO */
						KeyWriteFIFO( (KEY_Flag_enumt)(Key1_LP+((G_KEY_NUM+i)*4)));
					}
				}
			}
			else
			{
				/* 释放，切换为空闲 */
				SG_KeyCombStaList[i].status = keySta_dummy;

				/* 空闲回调 */
			}
		}break;
		case keySta_doublepress:
		{	
			/* 双击按下 */
			if (tmpCombTrigFlag)
			{
				/* 仍然按下，长按计时 */
				SG_KeyCombStaList[i].longPressTime++;
				if (SG_KeyCombStaList[i].longPressTime>SG_KeyCombStaList[i]._LPFTime)
				{
					/* 长按滤波达到，切换为长按状态 */
					SG_KeyCombStaList[i].status = keySta_longpress;
					SG_KeyCombStaList[i].longPressCount = 0;

					/* 长按按下回调 */
					if (SG_KeyEnableFlag[G_KEY_NUM+i] & (0x01<<Key1_LP))
					{
						/* 对应回调使能，将回调标识写入FIFO */
						KeyWriteFIFO( (KEY_Flag_enumt)(Key1_LP+((G_KEY_NUM+i)*4)));
					}
				}
			}
			else
			{
				/* 释放，切换为空闲 */
				SG_KeyCombStaList[i].status = keySta_dummy;

				/* 双击释放回调 */
				if (SG_KeyEnableFlag[G_KEY_NUM+i] & (0x01<<Key1_DPR))
				{
					/* 对应回调使能，将回调标识写入FIFO */
					KeyWriteFIFO( (KEY_Flag_enumt)(Key1_DPR+((G_KEY_NUM+i)*4)));
				}
			}
		}break;
		default:
			break;
		}
	}
	
	return;
}


/**
  * @brief   按键扫描程序，在中断中每10ms执行一次，每次扫描并切换所有按键的状态
  * @param   
  * @retval  
**/
void KeyScan10ms(void)
{
	#if KEY_COMB_ON //
	KeyDetectAnchor();//扫描锚键
	#endif //KEY_COMB_ON

	KeyDetectCommon();//扫描普通按键

	#if KEY_COMB_ON //
	KeyDetectCombination();//扫描组合键
	#endif //KEY_COMB_ON

	return;
}


/**
  * @brief   向按键FIFO里储存器里写入按键标识
  * @param   KeyFlag : 按键回调标识
  * @retval  
**/
void KeyWriteFIFO(KEY_Flag_enumt KeyFlag)
{
	SG_KeyCBFlagFIFO[SG_KeyFifoRW_Obj.WPoint] = KeyFlag;

	SG_KeyFifoRW_Obj.WPoint++;
	if (SG_KeyFifoRW_Obj.WPoint >= KEY_FIFO_CAPACITY)
	{
		/* 超出边界重置指针 */
		SG_KeyFifoRW_Obj.WPoint = 0;
	}
	
	return;
}


/**
  * @brief   从按键FIFO里储存器里读取按键标识
  * @param   
  * @retval  KeyDummy: 无标识 else: 读取的第一个按键回调标识
**/
KEY_Flag_enumt KeyReadFIFO(void)
{
	KEY_Flag_enumt ret = KeyDummy;
	if (SG_KeyFifoRW_Obj.WPoint != SG_KeyFifoRW_Obj.RPoint)
	{
		/* 可读 */
		ret = SG_KeyCBFlagFIFO[SG_KeyFifoRW_Obj.RPoint];
		SG_KeyFifoRW_Obj.RPoint++;
		if (SG_KeyFifoRW_Obj.RPoint >= KEY_FIFO_CAPACITY)
		{
			/* 超出边界重置指针 */
			SG_KeyFifoRW_Obj.RPoint = 0;
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
	SG_KeyFifoRW_Obj.RPoint = 0;
	SG_KeyFifoRW_Obj.WPoint = 0;

	return;
}

/***************************按键状态回调函数***********************/
#if KEY_USE_DEFAULT_CBF //使用默认按键回调函数
/************************KEY_UP******************/

/**
  * @brief   按键KEY_UP 单击/短按释放回调函数
  * @param   
  * @retval  
**/
void KeyUpReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key up release");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键KEY_UP 长按回调函数
  * @param   
  * @retval  
**/
void KeyUpLongPressedCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key up long pressed");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键KEY_UP 双击释放回调函数
  * @param   
  * @retval  
**/
void KeyUpDoubleReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key up double release");
	#endif //KEY_DEBUG_ON

	return;
}


/************************KEY_DOWN******************/

/**
  * @brief   按键 KEY_DOWN 单击/短按释放回调函数
  * @param   
  * @retval  
**/
void KeyDownReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key down release");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键 KEY_DOWN 长按回调函数
  * @param   
  * @retval  
**/
void KeyDownLongPressedCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key down long pressed");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键 KEY_DOWN 双击释放回调函数
  * @param   
  * @retval  
**/
void KeyDownDoubleReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key up double release");
	#endif //KEY_DEBUG_ON

	return;
}


/************************KEY_LEFT******************/

/**
  * @brief   按键 KEY_LEFT 单击/短按释放回调函数
  * @param   
  * @retval  
**/
void KeyLeftReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key left release");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键 KEY_LEFT 长按回调函数
  * @param   
  * @retval  
**/
void KeyLeftLongPressedCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key left long pressed");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键 KEY_LEFT 双击释放回调函数
  * @param   
  * @retval  
**/
void KeyLeftDoubleReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key left double release");
	#endif //KEY_DEBUG_ON

	return;
}


/************************KEY_RIGHT******************/

/**
  * @brief   按键 KEY_RIGHT 单击/短按释放回调函数
  * @param   
  * @retval  
**/
void KeyRightReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key right  release");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键 KEY_RIGHT 长按回调函数
  * @param   
  * @retval  
**/
void KeyRightLongPressedCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key right long pressed");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键 KEY_RIGHT 双击释放回调函数
  * @param   
  * @retval  
**/
void KeyRightDoubleReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key right double release");
	#endif //KEY_DEBUG_ON

	return;
}


/************************KEY_A******************/

/**
  * @brief   按键 KEY_A 单击/短按释放回调函数
  * @param   
  * @retval  
**/
void KeyAReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key a  release");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键 KEY_A 长按回调函数
  * @param   
  * @retval  
**/
void KeyALongPressedCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key a long pressed");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键 KEY_A 双击释放回调函数
  * @param   
  * @retval  
**/
void KeyADoubleReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key a double release");
	#endif //KEY_DEBUG_ON

	return;
}


/************************KEY_B******************/

/**
  * @brief   按键 KEY_B 单击/短按释放回调函数
  * @param   
  * @retval  
**/
void KeyBReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key b  release");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键 KEY_B 长按回调函数
  * @param   
  * @retval  
**/
void KeyBLongPressedCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key b long pressed");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键 KEY_B 双击释放回调函数
  * @param   
  * @retval  
**/
void KeyBDoubleReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key b double release");
	#endif //KEY_DEBUG_ON

	return;
}


/************************KEY_C******************/

/**
  * @brief   按键 KEY_C 单击/短按释放回调函数
  * @param   
  * @retval  
**/
void KeyCReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key c  release");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键 KEY_C 长按回调函数
  * @param   
  * @retval  
**/
void KeyCLongPressedCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key c long pressed");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键 KEY_C 双击释放回调函数
  * @param   
  * @retval  
**/
void KeyCDoubleReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key c double release");
	#endif //KEY_DEBUG_ON

	return;
}


/************************KEY_D******************/

/**
  * @brief   按键 KEY_D 单击/短按释放回调函数
  * @param   
  * @retval  
**/
void KeyDReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key d  release");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键 KEY_D 长按回调函数
  * @param   
  * @retval  
**/
void KeyDLongPressedCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key d long pressed");
	#endif //KEY_DEBUG_ON

	return;
}

/**
  * @brief   按键 KEY_D 双击释放回调函数
  * @param   
  * @retval  
**/
void KeyDDoubleReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key d double release");
	#endif //KEY_DEBUG_ON

	return;
}


/**
  * @brief   组合键1 单击/短按释放回调函数
  * @param   
  * @retval  
**/
void KeyComb1ReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key Comb1 release");
	#endif //KEY_DEBUG_ON

	return;
}
/**
  * @brief   组合键1 双击释放回调函数
  * @param   
  * @retval  
**/
void KeyComb1DoubleReleaseCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key Comb1 double release");
	#endif //KEY_DEBUG_ON

	return;
}
/**
  * @brief   组合键1 长按回调函数
  * @param   
  * @retval  
**/
void KeyComb1LongPressedCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key Comb1 long pressed");
	#endif //KEY_DEBUG_ON

	return;
}



/**
  * @brief   组合键2 单击按下回调函数
  * @param   
  * @retval  
**/
void KeyComb2PressedCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key Comb2 pressed");
	#endif //KEY_DEBUG_ON

	return;
}
/**
  * @brief   3 单击按下回调函数
  * @param   
  * @retval  
**/
void KeyComb3PressedCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key Comb3 pressed");
	#endif //KEY_DEBUG_ON

	return;
}
/**
  * @brief   组合键4 单击按下回调函数
  * @param   
  * @retval  
**/
void KeyComb4PressedCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key Comb4 pressed");
	#endif //KEY_DEBUG_ON

	return;
}
/**
  * @brief   组合键5 单击按下回调函数
  * @param   
  * @retval  
**/
void KeyComb5PressedCBF(void)
{
	#if KEY_DEBUG_ON
	KEY_INFO("key Comb5 pressed");
	#endif //KEY_DEBUG_ON

	return;
}

#endif // KEY_USE_DEFAULT_CBF
/****************************************************************/






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






