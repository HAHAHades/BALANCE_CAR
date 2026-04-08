

//硬件驱动
#include "bsp_iic.h"
#include "bsp_gpio.h"


uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  IICx_ERROR("I2C 等待超时!errorCode = %d",errorCode);
  
  return 0;
}


#ifdef BSP_USE_F103

void IICx_UsageDemo()
{
	//Usage1
	IICxInit(I2C1, BSP_IICxStructInit(I2C_Speed),  IIC1_IO_Reamp0);


	//Usage2
	I2C_InitTypeDef I2C_InitStruct;
	/* I2C 配置 */
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;

	/* 高电平数据稳定，低电平数据变化 SCL 时钟线的占空比 */
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;

	I2C_InitStruct.I2C_OwnAddress1 = BSP_I2Cx_OWN_ADDRESS7; 
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable ;
	 
	/* I2C的寻址模式 */
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

	/* 通信速率 */
	I2C_InitStruct.I2C_ClockSpeed = I2C_Speed;

	IICxInit(I2C1, I2C_InitStruct,  IIC1_IO_Reamp0);


}


/**
  * @brief   IIC 初始化
  * @param   I2Cx:  要使用的IIC
  * @param   I2C_InitStruct:  IICx的配置
  * @param   IICx_GPIO_Remap:  IO重映射 @ref USART_IO_Reamp
  * @retval  
  *		@arg IICx_RET_Flag_OK:初始化成功
  *		@arg other:初始化失败
  */
uint8_t IICxInit(I2C_TypeDef* I2Cx, I2C_InitTypeDef I2C_InitStruct, uint32_t IICx_GPIO_Remap)
{
	uint8_t retVal = IICx_RET_Flag_OK;
	I2C_Cmd(I2Cx, DISABLE);//先关再配置

	void (*RCC_APBxPeriphClockCmd)(uint32_t RCC_APBxPeriph, FunctionalState NewState);
	uint32_t RCC_APBxPeriph_IICx;


	GPIO_TypeDef* IICx_SCL_PORT;
	uint16_t IICx_SCL_PIN;
	GPIO_TypeDef* IICx_SDA_PORT;
	uint16_t IICx_SDA_PIN;

	if (I2Cx==I2C1)
	{
		/* code */
		RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		RCC_APBxPeriph_IICx = RCC_APB1Periph_I2C1;

		IICx_SCL_PORT = GPIOB;
		IICx_SCL_PIN = GPIO_Pin_6;
		IICx_SDA_PORT = GPIOB;
		IICx_SDA_PIN = GPIO_Pin_7;

		if (IICx_GPIO_Remap == IIC1_IO_Reamp1)
		{
			/* code */
			IICx_SCL_PORT = GPIOB;
			IICx_SCL_PIN = GPIO_Pin_8;
			IICx_SDA_PORT = GPIOB;
			IICx_SDA_PIN = GPIO_Pin_9;

			RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,  ENABLE);
			GPIO_PinRemapConfig( GPIO_Remap_I2C1,  ENABLE);
		}

	}
	else if (I2Cx==I2C2)
	{
		/* code */
		RCC_APBxPeriphClockCmd = RCC_APB1PeriphClockCmd;
		RCC_APBxPeriph_IICx = RCC_APB1Periph_I2C2;

		IICx_SCL_PORT = GPIOB;
		IICx_SCL_PIN = GPIO_Pin_10;
		IICx_SDA_PORT = GPIOB;
		IICx_SDA_PIN = GPIO_Pin_11;

	}
	else
	{
		retVal = IICx_RET_Flag_Init_Error;
	}

	RCC_APBxPeriphClockCmd(RCC_APBxPeriph_IICx, ENABLE);//必须放在IO配置前
	BSP_GPIO_Config(IICx_SCL_PORT, IICx_SCL_PIN,  GPIO_Mode_AF_OD);
	BSP_GPIO_Config(IICx_SDA_PORT, IICx_SDA_PIN,  GPIO_Mode_AF_OD);

	/* I2C 初始化 */
	
	I2C_Init(I2Cx, &I2C_InitStruct);

	/* 使能 I2C */
	I2C_Cmd(I2Cx, ENABLE);  

	return retVal;
}


/**
  * @brief   按默认方式配置IIC结构体
  * @param   I2C_ClockSpeed ：IIC速率
  * @retval 
**/
I2C_InitTypeDef BSP_IICxStructInit(uint32_t I2C_ClockSpeed)
{
	I2C_InitTypeDef I2C_InitStruct;

	/* I2C 配置 */
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;

	/* 高电平数据稳定，低电平数据变化 SCL 时钟线的占空比 */
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;

	I2C_InitStruct.I2C_OwnAddress1 = BSP_I2Cx_OWN_ADDRESS7; 
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable ;
	 
	/* I2C的寻址模式 */
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

	/* 通信速率 */
	I2C_InitStruct.I2C_ClockSpeed = I2C_ClockSpeed;

	return I2C_InitStruct;
}


#endif //BSP_USE_F103



