



//硬件驱动
#include "bsp_iic.h"

//AHT30 读取温度指令
//#define AHT30_SEND_DATAGETCMD_NumByte 3
//uint8_t AHT30_SEND_DATAGETCMD[AHT30_SEND_DATAGETCMD_NumByte] = {0xAC,0x33,0x00};
//#define AHT30_READ_T_ADDR 0x71
//#define AHT30_READ_T_NumByte 7


static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT; 

/**
  * @brief   IIC IO初始化
  * @param   
  *		@arg I2Cx ：要使用的IIC
  *		@arg IICx_GPIO_Remap ：IIC引脚是否进行重映射，0x00:不重映射; 否则重映射(GPIO_Remap_I2C1...)
  * @retval  
  */
static void I2C_GPIO_Config(I2C_TypeDef* I2Cx, uint32_t IICx_GPIO_Remap)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 

	uint32_t IICx_RCC_APB1Periph = RCC_APB1Periph_I2C1;
	GPIO_TypeDef* IICx_GPIO_PORTx = GPIOB;
	uint32_t IICx_Pin_RCC_APB2Periph = RCC_APB2Periph_GPIOB;
	uint16_t IICx_SCL_GPIO_Pin = GPIO_Pin_6; 
	uint16_t IICx_SDA_GPIO_Pin = GPIO_Pin_7; 
	
	if(I2Cx==I2C1)
	{
		if(IICx_GPIO_Remap==GPIO_Remap_I2C1)
		{
			RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE);
			GPIO_PinRemapConfig( GPIO_Remap_I2C1, ENABLE);
			
	
			IICx_SCL_GPIO_Pin = GPIO_Pin_8; 
			IICx_SDA_GPIO_Pin = GPIO_Pin_9; 
			
		}

	}
	else if(I2Cx==I2C2)
	{
		IICx_RCC_APB1Periph = RCC_APB1Periph_I2C2;
		

		IICx_SCL_GPIO_Pin = GPIO_Pin_10; 
		IICx_SDA_GPIO_Pin = GPIO_Pin_11; 
	}
	
	/* 使能与 I2C 有关的时钟 */
	RCC_APB1PeriphClockCmd( IICx_RCC_APB1Periph,  ENABLE);
	//使能引脚时钟
	RCC_APB2PeriphClockCmd(IICx_Pin_RCC_APB2Periph, ENABLE);

    
	/* I2C_SCL、I2C_SDA*/
	GPIO_InitStructure.GPIO_Pin = IICx_SCL_GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // 开漏输出
	GPIO_Init(IICx_GPIO_PORTx, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IICx_SDA_GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // 开漏输出
	GPIO_Init(IICx_GPIO_PORTx, &GPIO_InitStructure);	

}



static void I2C_Mode_Config(I2C_TypeDef* I2Cx, uint32_t I2C_ClockSpeed)
{
	I2C_InitTypeDef  I2C_InitStructure; 

	/* I2C 配置 */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;

	/* 高电平数据稳定，低电平数据变化 SCL 时钟线的占空比 */
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;

	I2C_InitStructure.I2C_OwnAddress1 = BSP_I2Cx_OWN_ADDRESS7; 
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
	 
	/* I2C的寻址模式 */
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

	/* 通信速率 */
	I2C_InitStructure.I2C_ClockSpeed = I2C_ClockSpeed;

	/* I2C 初始化 */
	I2C_Init(I2Cx, &I2C_InitStructure);

	/* 使能 I2C */
	I2C_Cmd(I2Cx, ENABLE);  

}


uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  IICx_ERROR("I2C 等待超时!errorCode = %d",errorCode);
  
  return 0;
}



/**
  * @brief   IIC 初始化
  * @param   
  *		@arg I2Cx ：要使用的IIC
  *		@arg IICx_GPIO_Remap ：IIC引脚是否进行重映射，0x00:不重映射; 否则重映射(GPIO_Remap_I2C1...)
  *		@arg I2C_ClockSpeed ：IIC速率
  * @retval  
  */
void IICxInit(I2C_TypeDef* I2Cx, uint32_t IICx_GPIO_Remap, uint32_t I2C_ClockSpeed)
{	
	I2C_Cmd(I2Cx, DISABLE);//先关再配置
	
	
	I2C_GPIO_Config(I2Cx, IICx_GPIO_Remap);
	I2C_Mode_Config(I2Cx, I2C_ClockSpeed);
	
}











