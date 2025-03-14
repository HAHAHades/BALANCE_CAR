

#include "AHT30.h"



//AHT30 读取温度指令
#define AHT30_SEND_DATAGETCMD_NumByte 3
uint8_t AHT30_SEND_DATAGETCMD[AHT30_SEND_DATAGETCMD_NumByte] = {0xAC,0x33,0x00};
#define AHT30_READ_T_ADDR 0x71
#define AHT30_READ_T_NumByte 7


static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT; 


/**
  * @brief   初始化AHT30
  * @param   
  *		@arg 
  *		 
  * @retval  
  */
void AHT30Init( uint32_t I2C_ClockSpeed)
{	
	
	IICxInit( I2C_ClockSpeed);
	
	SisTic_Delay_ms(5);//等待AHT30
	
	
}


/**
  * @brief   读取并转换温湿度
  * @param   
  *		@arg 
  *		 
  * @retval  0:失败  1:成功
  */
_Bool AHT30_get_Temp(AHT30_RH_T_Struct *aht30_rh_t)
{
	/****************读取******************/
	if(!I2C_AHT30_SEND_DATAGETCMD())
	{return 0;}
	SisTic_Delay_ms(80);//等待AHT30
	
	
	uint8_t pBuff[AHT30_READ_T_NumByte];
	
	
	if(!I2C_AHT30_READ_T(pBuff))
	{return 0;}
	
	
	/*****************判断传感器状态*********************/
	uint8_t aht30_status = pBuff[0];
	if(!((aht30_status & 0x98)==0x18)) //  0xx1 1xxx  传感器空闲，CRC完整，校准功能启动
	{
		UsartPrintf(DEBUG_USART,"AHT30 status err! status:0x%x\r\n",aht30_status);
	}
	
	
	
	/*******************CRC校验***********************/
	uint8_t aht30_crc = pBuff[6];
	uint8_t cal_CRC = AHT30_CheckCrc8(pBuff,6);
	if(cal_CRC!=pBuff[6])
	{
		UsartPrintf(DEBUG_USART,"AHT30 CRC err! read_CRC:0x%x ;  cal_CRC:0x%x\r\n",pBuff[6], cal_CRC);
	}
	

	/************转换****************/

	int rh_data = 0;
	
	rh_data |= ((int)pBuff[1])<<12;
	rh_data |= ((int)pBuff[2])<<4;
	rh_data |= ((int)pBuff[3]>>4);
	
	int t_data = 0;
	
	t_data |= ((int)(pBuff[3]&0x0f))<<16;
	t_data |= ((int)pBuff[4])<<8;
	t_data |= ((int)pBuff[5]);
	
	
	
	aht30_rh_t->RH = rh_data/10485.76; //RH=(rh/2^20)*100%
	aht30_rh_t->T = (t_data/1048576.0)*200-50; //T=(t/2^20)*200-50
	aht30_rh_t->AHT_CRC = aht30_crc;
	aht30_rh_t->AHT_STATUS = aht30_status;
	
	return 1;
}


/**
  * @brief   向 AHT30发送读取温湿度的指令
  * @param   
  *		@arg 
  *		 
  * @retval  0:失败  1:成功
  */
_Bool I2C_AHT30_SEND_DATAGETCMD(void) 
{

  /* Send STRAT condition */
  I2C_GenerateSTART(IICx, ENABLE);

  I2CTimeout = I2CT_FLAG_TIMEOUT;  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_MODE_SELECT))  
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(0);
  } 
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Send AHT30 address for write */
  I2C_Send7bitAddress(IICx, AHT30_I2C_ADDRESS7, I2C_Direction_Transmitter);
  SisTic_Delay_ms(10);
  /* Test on EV6 and clear it */

  while(!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(1);
	
  }  


  /* Send the byte to be written */
  uint8_t NumByteToWrite = AHT30_SEND_DATAGETCMD_NumByte;
  uint8_t *pBuff = AHT30_SEND_DATAGETCMD;
  while(NumByteToWrite--)
  {
	I2C_SendData(IICx, *pBuff); 
	pBuff++;
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	/* Test on EV8 and clear it */
	while(!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(3);
	} 
	  
  }
  

	/* Send STOP condition */
	I2C_GenerateSTOP(IICx, ENABLE);

	return 1;
  
}


/**
  * @brief   从AHT30读取温湿度
  * @param   
  *		@arg 读取的数据
  *		 
  * @retval  0:失败  1:成功
  */
_Bool I2C_AHT30_READ_T(uint8_t *pBuffer) 
{

	I2CTimeout = I2CT_LONG_TIMEOUT;

	while(I2C_GetFlagStatus(IICx, I2C_FLAG_BUSY))
	{
	if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(9);
	}
	
	/* Send START condition */
	I2C_GenerateSTART(IICx, ENABLE);

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_MODE_SELECT))
	{
	if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(10);
	}
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	/* Send AHT30 address for write */
	I2C_Send7bitAddress(IICx, AHT30_I2C_ADDRESS7, I2C_Direction_Receiver);

	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(IICx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
	if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(1);
	}  

    
	uint8_t read_NumByte = AHT30_READ_T_NumByte;
	
	while(read_NumByte--)
	{
		
		/* Test on EV7 and clear it */    
		I2CTimeout = I2CT_LONG_TIMEOUT;

		while(I2C_CheckEvent(IICx, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)  
		{
			if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(3);
		} 

		*pBuffer = I2C_ReceiveData(IICx);
		pBuffer++;
		
		
	}
	
	/* Send STOP Condition */
	I2C_GenerateSTOP(IICx, ENABLE);
	return 1;
}



/**
  * @brief   AHT30数据CRC校验
  * @param   
  *		@arg 读取的数据
 *		@arg 数据长度
  * @retval  计算后的CRC值
  */
unsigned char  AHT30_CheckCrc8(unsigned char *pDat,unsigned char Lenth)
{
	unsigned char crc = 0xff, i, j;

    for (i = 0; i < Lenth ; i++)
    {
        crc = crc ^ *pDat;
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x80) crc = (crc << 1) ^ 0x31;
            else crc <<= 1;
        }
				pDat++;
    }
    return crc;
}



