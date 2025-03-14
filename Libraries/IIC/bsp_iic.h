#ifndef _BSP_IIC_H_
#define _BSP_IIC_H_


#include "stm32f10x.h"
#include "bsp_usart.h"




/************使用哪个IIC****************/
//#define IICNUM  0  //  0:使用IIC1  1：使用IIC2 


///*************定义IIC端口***************/
//#if IICNUM
//	#define IICx 						I2C2
//	#define IICx_CLK					RCC_APB1Periph_I2C2
//	#define IICx_APBxClock_FUN  		RCC_APB1PeriphClockCmd
//	#define IICx_GPIO_CLK				RCC_APB2Periph_GPIOB
//	#define IICx_GPIO_APBxClock_FUN 	RCC_APB2PeriphClockCmd
//	
//	#define IICx_SDA_PORT 				GPIOB
//	#define IICx_SDA_PIN 				GPIO_Pin_11
//	#define IICx_SDA_APBxClock_FUN  	RCC_APB2PeriphClockCmd
//	
//	#define IICx_SCL_PORT 				GPIOB
//	#define IICx_SCL_PIN 				GPIO_Pin_10
//	#define IICx_SCL_APBxClock_FUN  	RCC_APB2PeriphClockCmd
//#else
//	#define IICx 						I2C1
//	#define IICx_CLK					RCC_APB1Periph_I2C1
//	#define IICx_APBxClock_FUN  		RCC_APB1PeriphClockCmd
//	#define IICx_GPIO_CLK				RCC_APB2Periph_GPIOB
//	#define IICx_GPIO_APBxClock_FUN 	RCC_APB2PeriphClockCmd
//	
//	#define IICx_SDA_PORT 				GPIOB
//	#define IICx_SDA_PIN 				GPIO_Pin_7
//	#define IICx_SDA_APBxClock_FUN  	RCC_APB2PeriphClockCmd
//	
//	#define IICx_SCL_PORT 				GPIOB
//	#define IICx_SCL_PIN 				GPIO_Pin_6
//	#define IICx_SCL_APBxClock_FUN  	RCC_APB2PeriphClockCmd
//	
//#endif

/********************************/




/* STM32 I2C 快速模式 */
#define I2C_Speed              400000  //*

/*********************************************/
#define BSP_I2Cx_OWN_ADDRESS7       0x10   //本机地址，这个地址只要与STM32外挂的I2C器件地址不一样即可




/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))


/*信息输出*/
#define IICx_DEBUG_ON         0

#define IICx_INFO(fmt,arg...)           printf("<<-EEPROM-INFO->> "fmt"\n",##arg)
#define IICx_ERROR(fmt,arg...)          printf("<<-EEPROM-ERROR->> "fmt"\n",##arg)
#define IICx_DEBUG(fmt,arg...)          do{\
                                          if(EEPROM_DEBUG_ON)\
                                          printf("<<-EEPROM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)


										  

										  
										  
										  
										  
						
/*******************************************************************************/
void IICxInit(I2C_TypeDef* I2Cx, uint32_t IICx_GPIO_Remap, uint32_t I2C_ClockSpeed);
uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);

#endif //_BSP_IIC_H_

