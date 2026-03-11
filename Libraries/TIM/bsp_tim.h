

#ifndef _BSP_TIM_H_
#define _BSP_TIM_H_


#include "stm32f10x.h"



/*******注意，如果使用TIM3的CH1或CH3等的映射引脚，GPIO需要重映射,使用GPIO_PinRemapConfig()********/
//注意，C6T6没有TIM4
//库函数SYSCLK为72M,AHB分频为1,APB1为36M分频为2,TIM2-TIM7为36*2=72M

/*            F103C      TIMx_IO_Reamp 对照表
TIMx    BKIN    CH1N    CH2N    CH3N    CH4N    CH1    CH2    CH3    CH4    ETR    IO_Reamp 
TIM1    B12     B13     B14     B15             A8     A9     A10    A11    A12    TIM1_IO_Reamp0
TIM1    A6      A7      B0      B1              A8     A9     A10    A11    A12    TIM1_IO_Reamp1
TIM2                                            A0     A1     A2     A3     A0     TIM2_IO_Reamp0
TIM2                                            A15    B3     A2     A3     A15    TIM2_IO_Reamp1
TIM2                                            A0     A1     B10    B11    A0     TIM2_IO_Reamp2
TIM2                                            A15    B3     B10    B11    A15    TIM2_IO_Reamp3
TIM3                                            A6     A7     B0     B1            TIM3_IO_Reamp0
TIM3                                            B4     B5     B0     B1            TIM3_IO_Reamp1
TIM4                                            B6     B7     B8     B9            TIM4_IO_Reamp0
*/




//是否开启定时器中断

// #define BSP_TIM1_IT_ON  0  //
// #define BSP_TIM2_IT_ON  0  //
// #define BSP_TIM3_IT_ON  0  //
// #define BSP_TIM4_IT_ON  0  //





/** @defgroup TIMx_IO_Reamp
  * @{
  */
#define TIM1_IO_Reamp0  ((uint32_t)0x00010000)  //
#define TIM1_IO_Reamp1  ((uint32_t)0x00010001)  //
#define TIM2_IO_Reamp0  ((uint32_t)0x00020000)  //
#define TIM2_IO_Reamp1  ((uint32_t)0x00020001)  //
#define TIM2_IO_Reamp2  ((uint32_t)0x00020002)  //
#define TIM2_IO_Reamp3  ((uint32_t)0x00020003)  //
#define TIM3_IO_Reamp0  ((uint32_t)0x00030000)  //
#define TIM3_IO_Reamp1  ((uint32_t)0x00030001)  //
#define TIM4_IO_Reamp0  ((uint32_t)0x00040000)  //
/**
  * @}
  */


typedef struct BSP_timx_TypeDef
{

	  TIM_TypeDef* TIMx;
    uint8_t WorkMode;//@ref TIMx_WorkMode
	  uint32_t TIM_GPIO_Remap;//@ref TIMx_IO_Reamp
    uint16_t TIM_Channel_x;//@ref TIM_OCIC_Channel_x
	  TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef* TIM_OCInitStruct;
    TIM_ICInitTypeDef* TIM_ICInitStruct;
    uint32_t TIMx_Delay_Count;//延时计数值
    uint32_t TIMx_DelayT_Count;//TIMx_DelayT(bsp_time.h)的计数值,最多计数1小时
    uint16_t TIMx_Prescaler;
    uint16_t TIMx_Period;
    uint32_t TIMx_DelayIF;//TIMx的计数器中断频率Hz  =  TIMxCLK/( (Psc+1)*(Period+1) )
}BSP_TIMx_TypeDef;



/** @defgroup TIM_OCIC_Channel_x 
  * @{
  */
#define TIM_OCIC_Channel_1 TIM_Channel_1 
#define TIM_OCIC_Channel_2 TIM_Channel_2 
#define TIM_OCIC_Channel_3 TIM_Channel_3 
#define TIM_OCIC_Channel_4 TIM_Channel_4 
#define TIM_OCIC_Channel_12 (TIM_Channel_1 | TIM_Channel_2) 
#define TIM_OCIC_Channel_13 (TIM_Channel_1 | TIM_Channel_3) 
#define TIM_OCIC_Channel_14 (TIM_Channel_1 | TIM_Channel_4) 
#define TIM_OCIC_Channel_23 (TIM_Channel_2 | TIM_Channel_3) 
#define TIM_OCIC_Channel_24 (TIM_Channel_2 | TIM_Channel_4) 
#define TIM_OCIC_Channel_34 (TIM_Channel_3 | TIM_Channel_4) 
#define TIM_OCIC_Channel_123 (TIM_Channel_1 | TIM_Channel_2 | TIM_Channel_3) 
#define TIM_OCIC_Channel_124 (TIM_Channel_1 | TIM_Channel_2 | TIM_Channel_4) 
#define TIM_OCIC_Channel_134 (TIM_Channel_1 | TIM_Channel_3 | TIM_Channel_4) 
#define TIM_OCIC_Channel_234 (TIM_Channel_2 | TIM_Channel_3 | TIM_Channel_4) 
#define TIM_OCIC_Channel_1234 (TIM_Channel_1 |TIM_Channel_2 | TIM_Channel_3 | TIM_Channel_4) 

/**
  * @}
  */ 

/** @defgroup TIMx_WorkMode 
  * @{
  */
#define BSP_TIMx_TIME       ((uint8_t)0x01) //计时模式
#define BSP_TIMx_PWM        ((uint8_t)0x02) //PWM模式
#define BSP_TIMx_ENCODER    ((uint8_t)0x03) //编码器模式
/**
  * @}
  */ 



/** @defgroup TIMx_TIMEBASE_CountOverFlow 计数器中断频率对应的psc和reload
  * @{
  */

#define TIMx72M_1000k_Prescaler 72-1
#define TIMx72M_1000k_Period 1-1 // TIMx_DelayT = 1 / ( 72000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx72M_100k_Prescaler 72-1
#define TIMx72M_100k_Period 10-1 // TIMx_DelayT = 1 / ( 72000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx72M_10k_Prescaler 72-1
#define TIMx72M_10k_Period 100-1 // TIMx_DelayT = 1 / ( 72000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx72M_1k_Prescaler 72-1
#define TIMx72M_1k_Period 1000-1 // TIMx_DelayT = 1 / ( 72000000 / ( (Psc+1)*(Period+1) ) )


#define TIMx36M_1000k_Prescaler 36-1
#define TIMx36M_1000k_Period 1-1 // TIMx_DelayT = 1 / ( 36000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx36M_100k_Prescaler 36-1
#define TIMx36M_100k_Period 10-1 // TIMx_DelayT = 1 / ( 36000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx36M_10k_Prescaler 36-1
#define TIMx36M_10k_Period 100-1 // TIMx_DelayT = 1 / ( 36000000 / ( (Psc+1)*(Period+1) ) )

#define TIMx36M_1k_Prescaler 36-1
#define TIMx36M_1k_Period 1000-1 // TIMx_DelayT = 1 / ( 36000000 / ( (Psc+1)*(Period+1) ) )

/**
  * @}
  */



#define ARRAY_LENGTH(arr) sizeof(arr)/sizeof(arr[0]) //获取数据长度

/* 标志代码 */
#define TIMx_RET_Flag_OK  	        ((uint8_t)0x00)	//								  
#define TIMx_RET_Flag_Init_Error  	((uint8_t)0x01)	//初始化错误




uint8_t BSP_TIMx_Init(BSP_TIMx_TypeDef* BSP_TIMx_struct);
#endif //_BSP_TIM_H_
