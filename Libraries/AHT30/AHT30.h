#ifndef _BSP_AHT30_H_
#define _BSP_AHT30_H_


#include "stm32f10x.h"
#include "bsp_usart.h"
#include "delay.h"
#include "bsp_iic.h"

/****************AHT30温湿度计连接**********************
AHT30             C8T6 IIC1         C8T6 IIC1
VIN               3v3               3v3
GND               G                 G
SCL               PB6               PB10
SDA               PB7               PB11

*******************************************************/

#define AHT30_I2C_ADDRESS7  		0x38<<1 	//外部模块IIC地址,IIC7位地址最低位为读(1)写(0)标志,1-7位为地址，库函数会自动修改最低位
								  
										  
										  
typedef struct
{
	uint8_t AHT_CRC;//CRC结果
	uint8_t AHT_STATUS;//AHT30当前状态
	float RH;//湿度
	float T;//温度
}AHT30_RH_T_Struct;
							


/*******************************************************************************/
void AHT30Init(uint32_t I2C_ClockSpeed);
_Bool AHT30_get_Temp(AHT30_RH_T_Struct *aht30_rh_t);
_Bool I2C_AHT30_SEND_DATAGETCMD(void);
_Bool I2C_AHT30_READ_T(uint8_t *pBuffer);
unsigned char  AHT30_CheckCrc8(unsigned char *pDat,unsigned char Lenth);
				
/*******************************************************************************/


#endif //_BSP_AHT30_H_

