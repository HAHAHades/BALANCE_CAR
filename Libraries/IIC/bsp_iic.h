#ifndef _BSP_IIC_H_
#define _BSP_IIC_H_


#include "stm32f10x.h"

#include "bsp_SysTick.h"


/*        F103C   IICx_IO_Reamp 对照表
IICx       SCL     SDA             IO_Reamp 
IIC1       B6      B7          IIC1_IO_Reamp0
IIC1       B8      B9          IIC1_IO_Reamp1
IIC2       B10     B11         IIC2_IO_Reamp0

*/

/*        F401R   IICx_IO_Reamp 对照表
IICx       SCL     SDA             IO_Reamp 
IIC1       B6      B7          IIC1_IO_Reamp0
IIC1       B8      B9          IIC1_IO_Reamp1
IIC2       B10     B11         IIC2_IO_Reamp0
IIC3       A8      C9          IIC3_IO_Reamp0

*/

#define BSP_USE_F103

/****************************/
#define BSP_IICx 						I2C1
#define IICx_DEBUG_ON         0

#define I2C_Speed              400000  //
#define BSP_I2Cx_OWN_ADDRESS7       0x10   //本机地址，这个地址只要与STM32外挂的I2C器件地址不一样即可


/** @defgroup IIC_IO_Reamp
  * @{
  */

#define IIC1_IO_Reamp0  ((uint32_t)0x00010000)  //
#define IIC1_IO_Reamp1  ((uint32_t)0x00010001)  //
#define IIC2_IO_Reamp0  ((uint32_t)0x00020000)  //
#define IIC3_IO_Reamp0  ((uint32_t)0x00030000)  //

/**
  * @}
  */


/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))


/* 标志代码 */
#define IICx_RET_Flag_OK  	        ((uint8_t)0x00)	//								  
#define IICx_RET_Flag_Init_Error  	((uint8_t)0x01)	//初始化错误


/*信息输出*/

#include "bsp_usart.h"


#define IICx_INFO(fmt,arg...)           do{\
                                          if(IICx_DEBUG_ON)\
											printf("<<-IIC-INFO->> "fmt"\n",##arg);\
											}while(0)
#define IICx_ERROR(fmt,arg...)          do{\
                                          if(IICx_DEBUG_ON)\
											printf("<<-IIC-ERROR->> "fmt"\n",##arg);\
											}while(0)
#define IICx_DEBUG(fmt,arg...)          do{\
                                          if(IICx_DEBUG_ON)\
                                          printf("<<-IIC-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)



										  
							  
										  
						
/*******************************************************************************/

uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);
uint8_t IICxInit(I2C_TypeDef* I2Cx, I2C_InitTypeDef I2C_InitStruct, uint32_t IICx_GPIO_Remap);
I2C_InitTypeDef BSP_IICxStructInit(uint32_t I2C_ClockSpeed);
#endif //_BSP_IIC_H_

