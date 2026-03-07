#ifndef _BSP_KEY_H
#define _BSP_KEY_H

#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_adc.h"


/************************************************************* 

*************************************************************/
#define KEY_TEST_ON 0 //仅测试按键

#define KEY_IOscan10_ON 1 // 是否开启按键IO扫描
#define KEY_DEBUG_ON 1 //通过串口输出调试信息
#define KEY_USE_DEFAULT_CBF 1 //是否使用默认按键回调函数，若使用自定义回调请将此置为0，并添加回调函数
#define KEY_COMMON_ON 1 //是否使用普通按键
#define KEY_POT_ON 1 //是否使用电位计按键
#define KEY_COMB_ON 0 //是否使用组合键


//普通按键
#define G_KEY_NUM 4  //所使用的按键数量(不包括电位计按键，不包括组合键，不包括组合键的锚键)
/*定义按键按下电平*/
#define Pressed_Level_Low 0  //低电平接通
#define Pressed_Level_High (!Pressed_Level_Low) //高电平接通


//电位计按键
#define G_KEYPOT_NUM 4  //所使用电位计按键数量
#define G_KeyPot_PHL (0xfff)//电位计正向上限
#define G_KeyPot_PLL ((0xfff>>1)+(0xff<<1)) //电位计正向下限
#define G_KeyPot_NHL ((0xfff>>1)-(0xff<<1))//电位计逆向上限
#define G_KeyPot_NLL (0)//电位计逆向下限
#define G_KeyPot_RTime 100  //电位计重复触发周期数


//组合键
#define G_KEYCOMBANCHOR_NUM 2  //所使用的组合键锚键按键数量(锚键只在组合键中起作用)
#define G_KEYCOMB_NUM 5 //所使用的组合键组数量 (组合键一组表示一个按键)
#define G_KEYCOMBANCHOR_FLAG (uint8_t)(0x80) //组合键锚键标识


#define G_KEYCBFLIST_NUM 4 //使用回调函数列表执行回调的按键个数(包括独立按键与组合键)

#define KEY_TESKS_PERIOD (uint8_t)(10) //按键任务运行周期 单位/1ms 
#define KEY_FIFO_CAPACITY 8 //定义按键回调标识FIFO储存容量


#define G_KEY_FILTER_TIME 1 // 按下/释放 软件消抖/滤波时间 单位/KEY_TESKS_PERIOD
#define G_KEY_LP_FT 100 // 长按判断时间 单位/KEY_TESKS_PERIOD
#define G_KEY_LP_RT 30 // 长按重复触发时间 单位/KEY_TESKS_PERIOD
#define G_KEY_DOUBLEPRESS_TIME 50 // 双击间隔时间 单位/KEY_TESKS_PERIOD


/*定义按键状态*/
typedef enum
{
    keySta_dummy=0,//空闲
    keySta_pressed,//按下/单击按下
    keySta_release,//短按释放/单击释放
    keySta_longpress,//长按
    keySta_doublepress//双击/双击按下
}KEY_STA_enumt;


/* 定义按键GPIO {GPIOx; GPIO_Pin_x; PressedLevel...} */
typedef struct 
{
    GPIO_TypeDef* gpio;
    uint16_t pin;
    uint8_t pressedLevel;// SET(!RESET)/RESET(0)
    uint8_t keyCombShareNum; //该键共用于几个组合键
    const uint8_t *keyCombIndexList; //该键所属的组合键索引列表(仅在 keyCombShareNum 非零时有效)
    const uint8_t *keyCombFlagList; //该键在某组合键中的标志列表
}KEY_GPIO_t;


/* 定义按键实时状态及每个状态的参数 */
typedef struct 
{
    
    KEY_STA_enumt status;
    uint8_t pressFilteringTime;
    uint8_t releaseFilteringTime;
    uint8_t longPressTime;
    uint8_t _LPFTime;//长按触发滤波
    uint8_t longPressCount;
    uint8_t _LPRTime;//长按重复触发周期
    uint8_t doublePressTime;
}KEY_STA_t;



/*电位计型按键*/
typedef struct KEY_Pot_t
{
    GPIO_TypeDef* gpio;
    uint16_t pin;
    ADC_TypeDef* ADCx;
    uint8_t ADC_Channel_x;
    u16 keyPot_PHLevel;//电位计正向上限
    u16 keyPot_PLLevel;//电位计正向下限
    u16 keyPot_NHLevel;//电位计逆向上限
    u16 keyPot_NLLevel;//电位计逆向下限
    u16 keyPot_Level;//电位计当前位置
    uint8_t longPressCount;//当前周期
    uint8_t _LPRTime;//重复触发周期
    int current_value;//当前状态值
    int sum_value;//累计状态值
    void (*pCBF)(struct KEY_Pot_t*) ;//单击回调/正向回调
    void (*dpCBF)(struct KEY_Pot_t*) ;//双击回调/逆向回调
    void (*rCBF)(struct KEY_Pot_t*) ;//释放回调/中位回调
    void (*lpCBF)(struct KEY_Pot_t*) ;//长按回调/重复回调
}KEY_Pot_TypeDef;

/* 定义按键回调标志 */
#define G_KEY_R_FLAG             (uint8_t)(0x01<<0) //按下/单击释放
#define G_KEY_LP_FLAG            (uint8_t)(0x01<<1) //长按
#define G_KEY_DPR_FLAG           (uint8_t)(0x01<<2) //双击/双击释放

/* 定义FIFO读写对象 */
typedef struct 
{
    /* data */
    uint8_t RPoint;//读指针位置
    uint8_t WPoint;//写指针位置
}KEY_FIFORW_t;


#define KEY_FIFOFLAG_NUM 4 //定义每个按键回调标识个数
/* 定义按键状态回调函数 */
typedef struct 
{
    /* data */
    void (*pressedFun)(void);       //按下(单击)      /正向 (序号0)
    void (*releaseFun)(void);       //释放(单击释放)  /逆向 (序号1)
    void (*doubleReleaseFun)(void); //双击(双击释放)  /中位 (序号2)
    void (*longPressedFun)(void);   //长按()         /重复 (序号3)
}KEY_CALLBACK_FUN_t;

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
    Key3_P,     //Key3 单击按下回调标识
    Key3_R,     //Key3 单击释放回调标识
    Key3_DPR,   //Key3 双击释放回调标识
    Key3_LP,    //Key3 长按/长按重复触发回调标识
    Key4_P,     //Key4 单击按下回调标识
    Key4_R,     //Key4 单击释放回调标识
    Key4_DPR,   //Key4 双击释放回调标识
    Key4_LP,    //Key4 长按/长按重复触发回调标识
    Key5_P,     //Key5 单击按下回调标识
    Key5_R,     //Key5 单击释放回调标识
    Key5_DPR,   //Key5 双击释放回调标识
    Key5_LP,    //Key5 长按/长按重复触发回调标识
    Key6_P,     //Key6 单击按下回调标识
    Key6_R,     //Key6 单击释放回调标识
    Key6_DPR,   //Key6 双击释放回调标识
    Key6_LP,    //Key6 长按/长按重复触发回调标识
    Key7_P,     //Key7 单击按下回调标识
    Key7_R,     //Key7 单击释放回调标识
    Key7_DPR,   //Key7 双击释放回调标识
    Key7_LP,    //Key7 长按/长按重复触发回调标识
    Key8_P,     //Key8 单击按下回调标识
    Key8_R,     //Key8 单击释放回调标识
    Key8_DPR,   //Key8 双击释放回调标识
    Key8_LP,    //Key8 长按/长按重复触发回调标识
    Key9_P,     //Key9 单击按下回调标识
    Key9_R,     //Key9 单击释放回调标识
    Key9_DPR,   //Key9 双击释放回调标识
    Key9_LP,    //Key9 长按/长按重复触发回调标识
    Key10_P,    //Key10 单击按下回调标识
    Key10_R,    //Key10 单击释放回调标识
    Key10_DPR,  //Key10 双击释放回调标识
    Key10_LP,   //Key10 长按/长按重复触发回调标识
    Key11_P,    //Key11 单击按下回调标识
    Key11_R,    //Key11 单击释放回调标识
    Key11_DPR,  //Key11 双击释放回调标识
    Key11_LP,   //Key11 长按/长按重复触发回调标识
    Key12_P,    //Key12 单击按下回调标识
    Key12_R,    //Key12 单击释放回调标识
    Key12_DPR,  //Key12 双击释放回调标识
    Key12_LP,   //Key12 长按/长按重复触发回调标识
    Key13_P,    //Key13 单击按下回调标识
    Key13_R,    //Key13 单击释放回调标识
    Key13_DPR,  //Key13 双击释放回调标识
    Key13_LP,   //Key13 长按/长按重复触发回调标识
    KeyDummy=0xff, //空闲
}KEY_Flag_enumt;


/* 定义组合键 */
typedef struct 
{
    /* data */
    uint8_t keyCount; //组合键数量
    KEY_GPIO_t* keyObj; //按键对象列表
}KEY_COMBINATION_t;


/*信息输出*/
#define KEY_INFO(fmt,arg...)           printf("<<-KEY-INFO->> "fmt"\n",##arg)
#define KEY_ERROR(fmt,arg...)          printf("<<-KEY-ERROR->> "fmt"\n",##arg)
#define KEY_DEBUG(fmt,arg...)          do{\
                                          if(KEY_DEBUG_ON)\
                                          UsartPrintf(USART_DEBUG,"<<-KEY-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)


/******************函数声明************************/

void KeyTest(void);
void KeyCallBackListExe(KEY_CALLBACK_FUN_t *CBFList, uint8_t i);
void KeyCallBackSwitchExe(KEY_Flag_enumt keyFlag );
uint8_t KeyGetFIFOFlag(uint8_t *Flag, uint8_t maxFlagNum);
void KeyInit(void);
void KeyGpioConfig(void);
void KeySetParam(KEY_STA_t *KeyStaObj , uint8_t LPFTime, uint8_t LPRTime);
void KEY_Pot_t_structInit( KEY_Pot_TypeDef* key);
void KeyTesks_SysTick_Handler(void);
void KeyDetectAnchor(void);
void KeyDetectCommon(void);
void KeyDetectPot(void);
void KeyDetectCombination(void);
void KeyScan10ms(void);
void KeyWriteFIFO(KEY_Flag_enumt KeyFlag);
KEY_Flag_enumt KeyReadFIFO(void);
void KeyClearFIFO(void);
KEY_Pot_TypeDef* KeyPot_FindByFlag(uint8_t Flag);
void KeyPot_CalPercentage(int *p, KEY_Pot_TypeDef* keyPot);
#if KEY_USE_DEFAULT_CBF //使用默认按键回调函数
void KeyPot_CBF_List(KEY_Flag_enumt kflag);
void KeyPot1_lpCBF(KEY_Pot_TypeDef* keyPot);
void KeyPot2_lpCBF( KEY_Pot_TypeDef* keyPot);
void KeyPot3_lpCBF( KEY_Pot_TypeDef* keyPot);
void KeyPot4_lpCBF( KEY_Pot_TypeDef* keyPot);
void KeyUpReleaseCBF(void);
void KeyUpLongPressedCBF(void);
void KeyUpDoubleReleaseCBF(void);
void KeyDownReleaseCBF(void);
void KeyDownLongPressedCBF(void);
void KeyDownDoubleReleaseCBF(void);
void KeyLeftReleaseCBF(void);
void KeyLeftLongPressedCBF(void);
void KeyLeftDoubleReleaseCBF(void);
void KeyRightReleaseCBF(void);
void KeyRightLongPressedCBF(void);
void KeyRightDoubleReleaseCBF(void);
void KeyAReleaseCBF(void);
void KeyALongPressedCBF(void);
void KeyADoubleReleaseCBF(void);
void KeyBReleaseCBF(void);
void KeyBLongPressedCBF(void);
void KeyBDoubleReleaseCBF(void);
void KeyCReleaseCBF(void);
void KeyCLongPressedCBF(void);
void KeyCDoubleReleaseCBF(void);
void KeyDReleaseCBF(void);
void KeyDLongPressedCBF(void);
void KeyDDoubleReleaseCBF(void);
void KeyComb1ReleaseCBF(void);
void KeyComb1DoubleReleaseCBF(void);
void KeyComb1LongPressedCBF(void);
void KeyComb2PressedCBF(void);
void KeyComb3PressedCBF(void);
void KeyComb4PressedCBF(void);
void KeyComb5PressedCBF(void);
#endif //KEY_USE_DEFAULT_CBF
/****************************************************/




#define MAX_KEY_NUM  3 //最多几个按键  LKEY从1开始编号

#define KEYx_ON 1 //注：按键按下时默认接入高电平

#define KEYx_OFF 0 


void KEY_Register(uint8_t KEY_num, GPIO_TypeDef* KEY_GPIO_POTRx, uint16_t KEY_GPIO_PINx);
_Bool KEY_Scan(uint8_t KEY_num);




#endif /* _BSP_KEY_H */
