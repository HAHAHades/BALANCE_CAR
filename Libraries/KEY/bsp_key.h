#ifndef _BSP_KEY_H
#define _BSP_KEY_H

#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_adc.h"


/************************************************************* 

*************************************************************/

#define KEY_DEBUG_ON 1 //通过串口输出调试信息

#define KEY_POT_ON 1 //是否使用电位计按键


//电位计按键
#define G_KeyPot_MaxLevel  0xfff //电位计最大数值 12位
#define G_KeyPot_PR (G_KeyPot_MaxLevel - G_KeyPot_PLL)//电位计正向范围
#define G_KeyPot_PLL ((G_KeyPot_MaxLevel>>1)+(0x0ff<<1)) //电位计正向下限
#define G_KeyPot_NHL ((G_KeyPot_MaxLevel>>1)-(0x0ff<<1))//电位计逆向上限
#define G_KeyPot_NR (G_KeyPot_NHL - 0)//电位计逆向范围
#define G_KeyPot_RTime 100  //电位计重复触发周期数
#define G_KeyPot_PctStep 10  //电位计步进值


#define KEY_TESKS_PERIOD ((uint8_t)10) //按键任务运行周期 单位/ms 
#define KEY_FIFO_CAPACITY 8 //定义按键回调标识FIFO储存容量
#define G_KEY_FILTER_TIME 30 // 按下/释放 软件消抖/滤波时间 单位/ms
#define G_KEY_LP_FT 800 // 长按判断时间 单位/ms
#define G_KEY_LP_RT 500 // 长按重复触发时间 单位/ms
#define G_KEY_DOUBLEPRESS_TIME 200 // 双击间隔时间 单位/ms
#define G_KEY_OUT_COMB_TIME 400 // 触发组合键后触发单键时间 单位/ms


#define KEY_FLAG_NUM 4  //每个按钮有几种可用的回调标识

#define KEY_FIFO_DUMMY 0
#define KEY_BUTTON_FIFO_MASK  ((uint32_t)0x0000ffff) //电位计和按键共用FIFO，按键FIFO只有低16位有效
#define KEY_POT_FIFO_FLAG  ((uint32_t)0x40000000) //电位计FIFO bit30为1,
#define KEY_POT_FIFO_MASK  ((uint32_t)0x0000ffff) //电位计FIFO bit30为1,bit0-bit15为int16数据
#define KEY_POT_FIFO_I_MASK  ((uint32_t)0x00ff0000) //电位计FIFO bit30为1,bit0-bit15为int16数据,bit16-bit23为u8索引
#define KEY_POT_FIFO_I_BIT_S  16 //电位计索引起始bit位


typedef  uint32_t KEY_FIFO_t;
typedef  uint16_t KEY_POT_Level_t;
typedef  int16_t KEY_POT_PCT_t;



/*定义按键状态*/
typedef enum
{
    keySta_dummy=0,//空闲
    keySta_pressed,//按下/单击按下
    keySta_release,//短按释放/单击释放
    keySta_longpress,//长按
    keySta_doublepress,//双击/双击释放
}KEY_STA_enumt;

/* 定义写入按键FIFO储存中的按键回调标识，独立按键在前，电位计在中间，组合键在后  */
typedef enum
{
 
    Key1_P = 0, //KEY1 单击按下回调标识
    Key1_R,     //KEY1 单击释放回调标识
    Key1_DPR,   //KEY1 双击释放回调标识
    Key1_LP,    //KEY1 长按/长按重复触发回调标识
    Key2_P,     //Key2 单击按下回调标识
    Key2_R,     //Key2 单击释放回调标识
    Key2_DPR,   //Key2 双击释放回调标识
    Key2_LP,    //Key2 长按/长按重复触发回调标识
    //..........
    Key_dummy=0xffff,
}KEY_Flag_enumt;




/* 定义按键GPIO {GPIOx; GPIO_Pin_x; PressedLevel...} */
typedef struct 
{
    GPIO_TypeDef* gpio;
    uint16_t pin;
    uint8_t pressedLevel;// SET(!RESET)/RESET(0) @ref BSP_KEY_PressedLevel
    uint8_t currentLevel;//当前电平
    uint32_t updataTime;//电平切换时间
}KEY_GPIO_TypeDef;


/* 定义按键实时状态及每个状态的参数 */
typedef struct 
{
    KEY_STA_enumt status;
    uint16_t _LPFTime;//长按触发时间
    uint16_t _LPRPeriod;//长按重复触发周期
    uint32_t updataTime;//状态更新时间
    uint8_t keyEnableFlag;//使能的检测状态 @ref KEY_Enable_Flag
}KEY_STA_TypeDef;


/* 普通按钮型按键/实体按键 */
typedef struct 
{
    KEY_GPIO_TypeDef IO_Struct;
    KEY_STA_TypeDef Sta_Struct;
    uint32_t intoCombTime;//触发组合键状态的时间
}KEY_Button_TypeDef;


/* 组合键 */
typedef struct 
{
    uint8_t keyCount; //组合键所包含实体按键数量
    uint8_t* keyIndexList; //所包含按键在按键列表中的索引
    uint32_t keyPressFlag;//组合键触发标识(keyPressFlag==keyNowFlagSta时按键按下)
    uint8_t keyNowFlagSta;//组合键当前状态标识
    KEY_STA_TypeDef Sta_Struct;
}KEY_COMBINATION_t;


/*电位计型按键*/
typedef struct KEY_Pot_t
{
    GPIO_TypeDef* gpio;
    uint16_t pin;
    ADC_TypeDef* ADCx;
    uint8_t ADC_Channel_x;
    //+<******************************************************* -
    //           ^              ^    ^             ^  
    //           PH             PL   NL            NH    
    // PR=PH-PL  NR=NH-NL
    KEY_POT_Level_t _PRange;//电位计正向范围
    KEY_POT_Level_t _PLLevel;//电位计正向下限
    KEY_POT_Level_t _NHLevel;//电位计逆向上限
    KEY_POT_Level_t _NRange;//电位计逆向范围
    uint16_t _RTime;//重复采样周期
    uint16_t _PctStep;//百分比状态步进值
    KEY_POT_PCT_t pct_value;//当前百分比状态值
    uint32_t updataTime;//更新时间
}KEY_Pot_TypeDef;


/* 定义FIFO读写对象 */
typedef struct 
{
    uint8_t RP;//读指针位置
    uint8_t WP;//写指针位置
    uint8_t DataLen;//可读数据个数  
}KEY_FIFORW_t;


/* 定义按键状态回调函数 */
typedef struct 
{
    /* data */
    void (*pressedFun)(void);       //按下(单击)      /正向 (序号0)
    void (*releaseFun)(void);       //释放(单击释放)  /逆向 (序号1)
    void (*doubleReleaseFun)(void); //双击(双击释放)  /中位 (序号2)
    void (*longPressedFun)(void);   //长按()         /重复 (序号3)
}KEY_CALLBACK_FUN_t;



/** @defgroup BSP_KEY_PressedLevel 按键按下电平/状态
  * @{
    */
 #define KEY_PL_RESET  0 
 #define KEY_PL_SET    (!KEY_PL_RESET)

/**
  * @}
  */


/** @defgroup KEY_Enable_Flag 按键按下电平/状态
  * @{
    */
#define KEY_Flag_D      (0x01<<keySta_dummy)//空闲标识
#define KEY_Flag_P      (0x01<<keySta_pressed)//按下标识
#define KEY_Flag_R      (0x01<<keySta_release)//释放标识
#define KEY_Flag_LP     (0x01<<keySta_longpress)//长按标识
#define KEY_Flag_DPR     (0x01<<keySta_doublepress)//双击标识

/**
  * @}
  */


/******************函数声明************************/

/*信息输出*/
#define KEY_INFO(fmt,arg...)           printf("<<-KEY-INFO->> "fmt"\n",##arg)
#define KEY_ERROR(fmt,arg...)          printf("<<-KEY-ERROR->> "fmt"\n",##arg)
#define KEY_DEBUG(fmt,arg...)          do{\
                                          if(KEY_DEBUG_ON)\
                                          UsartPrintf(USART_DEBUG,"<<-KEY-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

#define KEY_GET_IO_STA(A,B)  (A->IDR & B)
#define KEY_GET_POT_VAL(A,B)  ADC_Get_CHx_Data(A,B)

void BSP_KEY_UsageDemo(void);

uint32_t Key_GetTime(void);
void BSP_KEY_GpioConfig(void);
void BSP_KEY_Button_StructInit(KEY_Button_TypeDef* Button, GPIO_TypeDef* gpio, uint16_t pin, 
								uint16_t pressedLevel,uint8_t keyEnableFlag);
void BSP_KEY_COMB_StructInit(KEY_COMBINATION_t* Comb, uint8_t keyCount, 
						uint8_t* keyIndexList, uint8_t keyEnableFlag);
uint8_t BSP_KEY_Button_Register(uint8_t Index, KEY_Button_TypeDef Button_Struct);
uint8_t BSP_KEY_Comb_Register(uint8_t Index, KEY_COMBINATION_t Comb_Struct);
uint8_t BSP_KEY_InitParam(uint8_t Button_Count, uint8_t Comb_Count, uint8_t Pot_Count);
void BSP_KEY_ClearButton(void);
uint8_t KeyGetFIFOFlag(KEY_FIFO_t *Flag, uint8_t maxFlagNum);
void KeyWriteFIFO(KEY_FIFO_t KeyFlag);
void KeyClearFIFO(void);
KEY_FIFO_t KeyReadFIFO(void);
void KeyDetectButton(void);
void Key_TickTask(void);
static void UpdataButtonIOLevel(KEY_Button_TypeDef* Button_Struct, uint8_t keyIndexBias);
static void KeyUpdataButtonSta(KEY_STA_TypeDef* Sta_Struct, uint8_t isPressed, uint8_t keyIndexBias);

void KeyDetectPot(void);
static KEY_POT_Level_t KEY_getPotVal(uint8_t index);
static KEY_POT_PCT_t KEY_calPotPct(KEY_POT_Level_t level ,uint8_t index);
static uint8_t BSP_KEY_CmpDiff(int16_t A, int16_t B, uint16_t diff);
void KEY_Pot_StructInit(KEY_Pot_TypeDef* key, ADC_TypeDef* ADCx, uint8_t ADC_Channel_x);
uint8_t BSP_KEY_Pot_Register(uint8_t Index, KEY_Pot_TypeDef potStruct);

/****************************************************/


#endif /* _BSP_KEY_H */
