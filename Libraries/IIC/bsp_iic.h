#ifndef _BSP_IIC_H_
#define _BSP_IIC_H_


#include "stm32f10x.h"
#include "bsp_usart.h"




/************使用哪个IIC****************/


#define IICx_DEBUG_ON         0

/********************************/




/* STM32 I2C 快速模式 */
#define I2C_Speed              400000  //*

/*********************************************/
#define BSP_I2Cx_OWN_ADDRESS7       0x10   //本机地址，这个地址只要与STM32外挂的I2C器件地址不一样即可




/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))


/*信息输出*/


#define IICx_INFO(fmt,arg...)           do{\
                                            if(IICx_DEBUG_ON)\
                                            printf("<<-EEPROM-INFO->> "fmt"\n",##arg);\
                                        }while(0)
#define IICx_ERROR(fmt,arg...)          do{\
                                            if(IICx_DEBUG_ON)\
                                            printf("<<-EEPROM-ERROR->> "fmt"\n",##arg);\
                                        }while(0)
#define IICx_DEBUG(fmt,arg...)          do{\
                                          if(IICx_DEBUG_ON)\
                                          printf("<<-EEPROM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)


										  

										  
										  
										  
										  
						
/*******************************************************************************/
void IICxInit(I2C_TypeDef* I2Cx, uint32_t IICx_GPIO_Remap, uint32_t I2C_ClockSpeed);
uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);

#endif //_BSP_IIC_H_

