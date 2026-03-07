/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
//#include "bsp_exit.h"


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}



/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
#include "bsp_SysTick.h"
#include "mpu6050.h"
#include "mpu6050_SL.h"

// #define TASK_DELAY_NUM  2       //总任务个数，可以自己根据实际情况修改
// #define TASK_DELAY_0    1000    //任务0延时 1000*1 毫秒后执行：翻转LED
// #define TASK_DELAY_1    500     //任务1延时 500*1 毫秒后执行：MPU6050任务
// #define NumOfTask 2


// unsigned int Task_Delay[NumOfTask]={0};
extern void TimingDelay_Decrement(void);
extern void TimeStamp_Increment(void);
extern void gyro_data_ready_cb(void);

// uint32_t Task_Delay_Group[TASK_DELAY_NUM];  //任务数组，用来计时、并判断是否执行对应任务


/* 执行任务标志：读取MPU6050数据 */
// - 标志置 1表示读取MPU6050数据完成，需要在主循环处理MPU6050数据
// - 标志置 0表示未完成读取MPU6050数据，需要在中断中读取MPU6050数据
int task_readdata_finish;


// 声明外部变量
extern short Accel[3];
extern short Gyro[3];
extern float Temp;

//nrf_controller.c/h
#define NRF_CTRL_ON 0 //是否使用NRF遥控器
extern void NRF_CTRL_TickIncrease(void);
#include "bsp_key.h"
void SysTick_Handler(void)
{

  TimingDelay_Decrement();
  TimeStamp_Increment();
#if KEY_TEST_ON
  KeyTesks_SysTick_Handler();

#endif //KEY_TEST_ON
#if NRF_CTRL_ON
  NRF_CTRL_TickIncrease();
#endif //NRF_CTRL_ON
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/


#include "bsp_tim.h"

void TIM3_IRQHandler(void)
{
  #if TIMx_IT_ON
	TIMx_10us_IRQHandler();
  #endif //TIMx_IT_ON
}



#include "bsp_USART.h"

void USART1_IRQHandler(void)
{
	vUSART1_IRQHandler();
}

void USART2_IRQHandler(void)
{
 
	vUSART2_IRQHandler();
	
}


#include "nrf24l01p.h"
#include "nrf_controller.h"
void EXTI0_IRQHandler(void)
{

#if Use_Default_Pin_Config
	NRF24L01_IRQ_Handler();
#endif //!Use_Default_Pin_Config

	if(EXTI_GetITStatus(EXTI_Line0) !=RESET )//判断是否进入中断，若进入中断则执行后面的中断程序
	{
		/**********************中断程序*********************/

		/**************************************************/
	}
	EXTI_ClearITPendingBit(EXTI_Line0);//完成中断后清除中断标志位
}




#include "bsp_hc_sr04.h"

void EXTI2_IRQHandler(void)
{
	
	HC_SR04_IRQHandler();
	
}


extern void Control_Car_IRQHandler(void);

void EXTI9_5_IRQHandler(void)
{


      //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(MPU6050_INT_EXTI_LINE) != RESET) 
	{
       
    //回调函数
    MPU6050_data_ready_cb();
		Control_Car_IRQHandler();
    //清除中断标志位
    EXTI_ClearITPendingBit(MPU6050_INT_EXTI_LINE);     
	} 
	
}





void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line13) !=RESET )//判断是否进入中断，若进入中断则执行后面的中断程序
	{
		/**********************中断程序*********************/

		/**************************************************/
	}
	EXTI_ClearITPendingBit(EXTI_Line13);//完成中断后清除中断标志位
	

	
      //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(MPU6050_INT_EXTI_LINE) != RESET) 
	{
       
    //回调函数
    MPU6050_data_ready_cb();
		Control_Car_IRQHandler();
    //清除中断标志位
    EXTI_ClearITPendingBit(MPU6050_INT_EXTI_LINE);     
	} 
	

}








#include "bsp_spi.h"


void DMA1_Channel2_IRQHandler(void)
{
	if( (DMA1->ISR & DMA1_IT_TC2) != RESET )//获取中断标志位
	{
		if(SPI1_DMA_REQ_Rx)//
		{
			SPI1->CR2 &= (uint16_t)~SPI_I2S_DMAReq_Rx;//关闭SPI1的Tx DMA
		}
		DMA1_Channel2->CCR  &= (uint16_t)(~DMA_CCR1_EN);//关闭DMA1_Channel2
		DMA1->IFCR = DMA1_IT_TC2; //清楚中断标志位
	}
}

void DMA1_Channel3_IRQHandler(void)
{
	if( (DMA1->ISR & DMA1_IT_TC3) != RESET )//获取中断标志位
	{
		if(SPI1_DMA_REQ_Tx)//
		{
			SPI1->CR2 &= (uint16_t)~SPI_I2S_DMAReq_Tx;//关闭SPI1的Tx DMA
		}
		
		DMA1_Channel3->CCR  &= (uint16_t)(~DMA_CCR1_EN);//关闭DMA1_Channel3
		DMA1->IFCR = DMA1_IT_TC3; //清楚中断标志位
	}
	
}



/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
