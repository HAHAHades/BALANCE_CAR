#ifndef _BSP_ADC_H
#define _BSP_ADC_H

#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_SysTick.h"


extern uint16_t ADCx_Independent_DMA_MemoryBaseAddr[];
extern uint32_t ADCx_RegSimult_DMA_MemoryBaseAddr[];


/**/
#define ADC_NumOfChannel 4  //多通道模式下的通道使用个数
#define ADC_SOFTWARE_START 1 //是否软件触发转换

/**********ADC1 ADC2的通道引脚*******************/
#define ADC12_CH0_PORT  GPIOA
#define ADC12_CH0_PIN  GPIO_Pin_0
#define ADC12_CH1_PORT  GPIOA
#define ADC12_CH1_PIN  GPIO_Pin_1
#define ADC12_CH2_PORT  GPIOA
#define ADC12_CH2_PIN  GPIO_Pin_2
#define ADC12_CH3_PORT  GPIOA
#define ADC12_CH3_PIN  GPIO_Pin_3
#define ADC12_CH4_PORT  GPIOA
#define ADC12_CH4_PIN  GPIO_Pin_4
#define ADC12_CH5_PORT  GPIOA
#define ADC12_CH5_PIN  GPIO_Pin_5
#define ADC12_CH6_PORT  GPIOA
#define ADC12_CH6_PIN  GPIO_Pin_6
#define ADC12_CH7_PORT  GPIOA
#define ADC12_CH7_PIN  GPIO_Pin_7
#define ADC12_CH8_PORT  GPIOB
#define ADC12_CH8_PIN  GPIO_Pin_0
#define ADC12_CH9_PORT  GPIOB
#define ADC12_CH9_PIN  GPIO_Pin_1




					
/***************函数申明***************/
static  void ADC1_DMA_Config(uint32_t DMA_PeripheralDataSize, uint32_t DMA_MemoryDataSize ,uint32_t DMA_MemoryBaseAddr);

void ADC_START(ADC_TypeDef* ADCx, uint32_t ADC_Mode);

void ADCx_CHx_IndependentCongfig( ADC_TypeDef* ADCx, uint8_t* ADC_Channel_xList );
void ADCx_CHx_RegSimultCongfig( uint8_t* ADC_Channel_xList );
void ADC_Get_CHx_Data(uint16_t* data, uint8_t i);
/********************************/


#endif //_BSP_ADC_H



