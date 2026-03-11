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


#include "main.h"
#include "bsp_SysTick.h"


/* 执行任务标志：读取MPU6050数据 */
// - 标志置 1表示读取MPU6050数据完成，需要在主循环处理MPU6050数据
// - 标志置 0表示未完成读取MPU6050数据，需要在中断中读取MPU6050数据
int task_readdata_finish;
// 声明外部变量
// extern short Accel[3];
// extern short Gyro[3];
// extern float Temp;
//nrf_controller.c/h
#define NRF_CTRL_ON 0 //是否使用NRF遥控器
extern void NRF_CTRL_TickIncrease(void);

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
  TimeStamp_Increment();
// #if KEY_TEST_ON
//   KeyTesks_SysTick_Handler();

// #endif //KEY_TEST_ON
// #if NRF_CTRL_ON
//   NRF_CTRL_TickIncrease();
// #endif //NRF_CTRL_ON
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/




void TIM3_IRQHandler(void)
{
  #if BSP_TIM3_TIME_ON
  TIMx_DelayT_IRQHandler(&BSP_TIM3_Struct);
  #endif //#if BSP_TIM3_TIME_ON
}



#if USART_Print_ON

void USART1_IRQHandler(void)
{
	vUSART1_IRQHandler();
}

void USART2_IRQHandler(void)
{
 
	vUSART2_IRQHandler();
	
}

#endif //USART_Print_ON


void EXTI0_IRQHandler(void)
{

// #if Use_Default_Pin_Config
// 	NRF24L01_IRQ_Handler();
// #endif //!Use_Default_Pin_Config

	if(EXTI_GetITStatus(EXTI_Line0) !=RESET )//判断是否进入中断，若进入中断则执行后面的中断程序
	{
		/**********************中断程序*********************/

		/**************************************************/
	}
	EXTI_ClearITPendingBit(EXTI_Line0);//完成中断后清除中断标志位
}






void EXTI2_IRQHandler(void)
{
	
	// HC_SR04_IRQHandler();
	
}


// extern void Control_Car_IRQHandler(void);

void EXTI9_5_IRQHandler(void)
{

#if MPU_INT_PB5
      //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(MPU6050_INT_EXTI_LINE) != RESET) 
	{
       
    //回调函数
    MPU6050_data_ready_cb();
    #if BANLANCE_CAR_ON
		Control_Car_IRQHandler();
    #endif //#if BANLANCE_CAR_ON
    //清除中断标志位
    EXTI_ClearITPendingBit(MPU6050_INT_EXTI_LINE);     
	} 
#endif //MPU_INT_PB5
}





void EXTI15_10_IRQHandler(void)
{
	// if(EXTI_GetITStatus(EXTI_Line13) !=RESET )//判断是否进入中断，若进入中断则执行后面的中断程序
	// {
	// 	/**********************中断程序*********************/

	// 	/**************************************************/
  //   EXTI_ClearITPendingBit(EXTI_Line13);//完成中断后清除中断标志位
	// }
	
#if MPU_INT_PA15
  // //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(MPU6050_INT_EXTI_LINE) != RESET) 
	{
    //回调函数
    MPU6050_data_ready_cb();
#if MPU_GetEuler_IN_IT
    MPU_GetEuler(G_Euler_RPY, G_ACCEL_XYZ, G_GYRO_XYZ);
#endif //#if MPU_GetEuler_IN_IT

#if BANLANCE_CAR_ON
	Control_Car_IRQHandler();
#endif //#if BANLANCE_CAR_ON
    //清除中断标志位
    EXTI_ClearITPendingBit(MPU6050_INT_EXTI_LINE);     
	} 
#endif //MPU_INT_PA15

}




void DMA1_Channel2_IRQHandler(void)
{
	// if( (DMA1->ISR & DMA1_IT_TC2) != RESET )//获取中断标志位
	// {
	// 	if(SPI1_DMA_REQ_Rx)//
	// 	{
	// 		SPI1->CR2 &= (uint16_t)~SPI_I2S_DMAReq_Rx;//关闭SPI1的Tx DMA
	// 	}
	// 	DMA1_Channel2->CCR  &= (uint16_t)(~DMA_CCR1_EN);//关闭DMA1_Channel2
	// 	DMA1->IFCR = DMA1_IT_TC2; //清楚中断标志位
	// }
}

void DMA1_Channel3_IRQHandler(void)
{
	// if( (DMA1->ISR & DMA1_IT_TC3) != RESET )//获取中断标志位
	// {
	// 	if(SPI1_DMA_REQ_Tx)//
	// 	{
	// 		SPI1->CR2 &= (uint16_t)~SPI_I2S_DMAReq_Tx;//关闭SPI1的Tx DMA
	// 	}
		
	// 	DMA1_Channel3->CCR  &= (uint16_t)(~DMA_CCR1_EN);//关闭DMA1_Channel3
	// 	DMA1->IFCR = DMA1_IT_TC3; //清楚中断标志位
	// }
	
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
