/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   MPU6050驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-MINI STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */


#include "mpu6050.h"
#include "bsp_SysTick.h"


float  G_Euler_RPY[3];//欧拉角
float  G_GYRO_XYZ[3];//角速度
float  G_ACCEL_XYZ[3];//加速度


#ifdef   soft_IIC
#include "bsp_soft_i2c.h"
static void I2Cx_Error(uint8_t Addr);
int MPU6050_ReadData(unsigned char slave_addr, unsigned char reg_addr, unsigned short len,
                             unsigned char *data_ptr)
{
	unsigned char i;

    slave_addr <<= 1;
	
	i2c_Start();
	i2c_SendByte(slave_addr);
    
	if(i2c_WaitAck())
        I2Cx_Error(slave_addr);
    
	i2c_SendByte(reg_addr);
	
    if(i2c_WaitAck())
        I2Cx_Error(slave_addr);
	
	i2c_Start();
	i2c_SendByte(slave_addr+1);
	
    if(i2c_WaitAck())
        I2Cx_Error(slave_addr);
	
	for(i=0;i<(len-1);i++){
		*data_ptr=i2c_ReadByte(1);
		data_ptr++;
	}
	*data_ptr=i2c_ReadByte(0);
	i2c_Stop();
    return 0;
}

/**
 * @brief  写寄存器，这是提供给上层的接口
 * @param  slave_addr: 从机地址
 * @param 	reg_addr:寄存器地址
 * @param len：写入的长度
 *	@param data_ptr:指向要写入的数据
 * @retval 正常为0，不正常为非0
 */
int MPU6050_WriteReg(unsigned char slave_addr, unsigned char reg_addr, unsigned short len,
                              unsigned char *data_ptr)
{
    unsigned short i = 0;
    slave_addr <<= 1;
    i2c_Start();
    i2c_SendByte(slave_addr);
    
    if(i2c_WaitAck())
        I2Cx_Error(slave_addr);
    
    i2c_SendByte(reg_addr);
    
    if(i2c_WaitAck())
        I2Cx_Error(slave_addr);
    
    for(i=0;i<len;i++)
    {
        i2c_SendByte(data_ptr[i]);
        if(i2c_WaitAck())
            I2Cx_Error(slave_addr);
    }
    i2c_Stop();
    return 0;
}

/**
 * @brief  Manages error callback by re-initializing I2C.
 * @param  Addr: I2C Address
 * @retval None
 */
static void I2Cx_Error(uint8_t Addr)
{
    i2c_GPIO_Config();
    i2c_Stop();
    MPU6050_Init();
}

#else
#include "bsp_hard_i2c.h"



/**
 * @brief  读寄存器，这是提供给上层的接口
 * @param  slave_addr: 从机地址
 * @param 	reg_addr:寄存器地址
 * @param len：要读取的长度
 *	@param data_ptr:指向要存储数据的指针
 * @retval 正常为0，不正常为非0
 */
int MPU6050_ReadData(unsigned char slave_addr, unsigned char reg_addr, unsigned short len,
                             unsigned char *data_ptr)
{
    I2C_BufferRead(data_ptr,reg_addr,len);
    return 0;
}


/**
 * @brief  写寄存器，这是提供给上层的接口
 * @param  slave_addr: 从机地址
 * @param 	reg_addr:寄存器地址
 * @param len：写入的长度
 *	@param data_ptr:指向要写入的数据
 * @retval 正常为0，不正常为非0
 */
 
int MPU6050_WriteReg(unsigned char slave_addr, unsigned char reg_addr, unsigned short len,
                              unsigned char *data_ptr)
{
    I2C_ByteWrite(data_ptr,reg_addr,len);
    return 0;
}



#endif



static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    
    NVIC_InitStructure.NVIC_IRQChannel = MPU6050_INT_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    
    NVIC_Init(&NVIC_InitStructure);
}

void MPU_INT_Init(void)
{
    GPIO_InitTypeDef Init;
    EXTI_InitTypeDef EXTI_InitStructure;
    
    NVIC_Configuration();
    
    RCC_APB2PeriphClockCmd(RCC_INT_GPIO_PORT, ENABLE);   //开启GPIOA时钟
    
    Init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    Init.GPIO_Pin  = MPU6050_INT_Pin;
    Init.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(MPU6050_INT_GPIO_Port,&Init);
    
    /* 选择EXTI的信号源 */
    GPIO_EXTILineConfig(MPU6050_INT_EXTI_PORTSOURCE,MPU6050_INT_EXTI_PINSOURCE);
    
    EXTI_InitStructure.EXTI_Line = MPU6050_INT_EXTI_LINE;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    
    EXTI_Init(&EXTI_InitStructure);
    
}


/**
  * @brief   初始化MPU6050芯片
  * @param   
  * @retval  
  */
void MPU6050_Init(void)
{
  int i=0,j=0;
  uint8_t reg[5]={0x00,0x07,0x06,0x01,0x18};
  
  
  //在初始化之前要延时一段时间，若没有延时，则断电后再上电数据可能会出错
  for(i=0;i<1000;i++)
  {
    for(j=0;j<1000;j++)
    {
      ;
    }
  }
	MPU6050_WriteReg(MPU6050_SLAVE_ADDRESS,MPU6050_RA_PWR_MGMT_1,1, &reg[0]);	     //解除休眠状态
	MPU6050_WriteReg(MPU6050_SLAVE_ADDRESS,MPU6050_RA_SMPLRT_DIV ,1, &reg[1]);	    //陀螺仪采样率
	MPU6050_WriteReg(MPU6050_SLAVE_ADDRESS,MPU6050_RA_CONFIG , 1,&reg[2]);	
	MPU6050_WriteReg(MPU6050_SLAVE_ADDRESS,MPU6050_RA_ACCEL_CONFIG ,1, &reg[3]);	  //配置加速度传感器工作在4G模式
	MPU6050_WriteReg(MPU6050_SLAVE_ADDRESS,MPU6050_RA_GYRO_CONFIG,1, &reg[4]);     //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
}

/**
  * @brief   读取MPU6050的ID
  * @param   
  * @retval  
  */
uint8_t MPU6050ReadID(void)
{
	unsigned char Re = 0;
    MPU6050_ReadData(MPU6050_SLAVE_ADDRESS,MPU6050_RA_WHO_AM_I,1,&Re);
	if((Re != MPU_6500ID)&&(Re != MPU_6050ID))
	{
        #if USART_Print_ON
		UsartPrintf(USART_DEBUG,"MPU6050 dectected error!\r\n检测不到MPU6050模块，请检查模块与开发板的接线");
        UsartPrintf(USART_DEBUG,"MPU6050 ID = %d\r\n",Re);
        #endif //USART_Print_ON
		return 0;
	}
	else
	{
        #if USART_Print_ON
		UsartPrintf(USART_DEBUG,"MPU6050 ID = %d\r\n",Re);
        #endif //USART_Print_ON
		return 1;
	}
		
}
/**
  * @brief   读取MPU6050的加速度数据
  * @param   
  * @retval  
  */
void MPU6050ReadAcc(short *accData)
{
    u8 buf[6];
    MPU6050_ReadData(MPU6050_SLAVE_ADDRESS,MPU6050_ACC_OUT,6,buf);
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
}

/**
  * @brief   读取MPU6050的角加速度数据
  * @param   
  * @retval  
  */
void MPU6050ReadGyro(short *gyroData)
{
    u8 buf[6];
    MPU6050_ReadData(MPU6050_SLAVE_ADDRESS,MPU6050_GYRO_OUT,6,buf);
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5];
}


/**
  * @brief   读取MPU6050的原始温度数据
  * @param   
  * @retval  
  */
void MPU6050ReadTemp(short *tempData)
{
	u8 buf[2];
    MPU6050_ReadData(MPU6050_SLAVE_ADDRESS,MPU6050_RA_TEMP_OUT_H,2,buf);
    *tempData = (buf[0] << 8) | buf[1];
}


/**
  * @brief   读取MPU6050的温度数据，转化成摄氏度
  * @param   
  * @retval  
  */
void MPU6050_ReturnTemp(float *Temperature)
{
	short temp3;
	u8 buf[2];
	
    MPU6050_ReadData(MPU6050_SLAVE_ADDRESS,MPU6050_RA_TEMP_OUT_H,2,buf);
  temp3= (buf[0] << 8) | buf[1];	
	*Temperature=((double) temp3/340.0)+36.53;

}

/**
  * @brief   将读取的数据转为欧拉角、加速度、角速度
  * @param   Euler_RPY[3] 欧拉角
  * @param   ACCEL[3] 加速度
  * @param   GYRO_XYZ[3] 角速度
  * @retval  
  */
void MPU_GetEuler(float *Euler_RPY, float *ACCEL, float *GYRO_XYZ)
{
    unsigned char new_temp = 0;
    unsigned long timestamp;
    unsigned long sensor_timestamp;
    int new_data = 0;

    /* 接收到INT传来的中断信息后继续往下 */
    if (!hal.sensors || !hal.new_gyro)
    {
        return;
    }

    /* 每过500ms读取一次温度 */
    get_tick_count(&timestamp);
    if (timestamp > hal.next_temp_ms)
    {
        hal.next_temp_ms = timestamp + TEMP_READ_MS;
        new_temp = 1;
    }

    /* 接收到新数据 并且 开启DMP */
    if (hal.new_gyro && hal.dmp_on)
    {
        short gyro[3], accel_short[3], sensors;
        unsigned char more;
        long accel[3], quat[4], temperature;
        /* 当DMP正在使用时，该函数从FIFO获取新数据 */
        dmp_read_fifo(gyro, accel_short, quat, &sensor_timestamp, &sensors, &more);
        /* 如果more=0，则数据被获取完毕 */
        if (!more)
            hal.new_gyro = 0;

        /* 读取相对应的新数据，推入MPL */
        //四元数数据
        if (sensors & INV_WXYZ_QUAT)
        {
            inv_build_quat(quat, 0, sensor_timestamp);
            new_data = 1;
        }

        //陀螺仪数据
        if (sensors & INV_XYZ_GYRO)
        {
            inv_build_gyro(gyro, sensor_timestamp);
            new_data = 1;
            if (new_temp)
            {
                new_temp = 0;
                /* 获取温度，仅用于陀螺温度比较 */
                mpu_get_temperature(&temperature, &sensor_timestamp);
                inv_build_temp(temperature, sensor_timestamp);
            }
        }
        
        //加速度计数据
        if (sensors & INV_XYZ_ACCEL)
        {
            accel[0] = (long)accel_short[0];
            accel[1] = (long)accel_short[1];
            accel[2] = (long)accel_short[2];
            inv_build_accel(accel, 0, sensor_timestamp);
            new_data = 1;
        }
    }//end if (hal.new_gyro && hal.dmp_on)

    if (new_data)
    {
        long data[9];
        int8_t accuracy;
        /* 处理接收到的数据 */
        if (inv_execute_on_data())
        {
            #if USART_Print_IN_IT_ON
            UsartPrintf(USART_DEBUG,"数据错误\n");
            #endif //USART_Print_IN_IT_ON
        }

        if (inv_get_sensor_type_euler(data, &accuracy, (inv_time_t *)&timestamp))
        {
            Euler_RPY[0] = data[0] * 1.0 / (1 << 16);
            Euler_RPY[1] = data[1] * 1.0 / (1 << 16);
            Euler_RPY[2] = data[2] * 1.0 / (1 << 16);
            
            #if USART_Print_IN_IT_ON
            UsartPrintf(USART_DEBUG,"\r\n欧拉角(rad)\t\t: %7.5f\t %7.5f\t %7.5f\t", Euler_RPY[0], Euler_RPY[1], Euler_RPY[2]);
            #endif //USART_Print_IN_IT_ON
            
            #if OLED_SHOW_MPU
            OLED_Display_XxX_ASCII( 0, 0,  8, 16, 1, "euler:R,P,Y\n", Euler_RPY[0], Euler_RPY[1], Euler_RPY[2]);
            OLED_Display_XxX_ASCII( 0, 16,  8, 16, 1, "%5.2f %5.2f %5.2f\n", Euler_RPY[0], Euler_RPY[1], Euler_RPY[2]);
            OLED_Refresh();
            #endif //OLED_SHOW_MPU
        }
           if (inv_get_sensor_type_accel(data, &accuracy, (inv_time_t *)&timestamp))
           {
               ACCEL[0] = data[0] * 1.0 / (1 << 16);
               ACCEL[1] = data[1] * 1.0 / (1 << 16);
               ACCEL[2] = data[2] * 1.0 / (1 << 16);
				#if USART_Print_IN_IT_ON
               UsartPrintf(USART_DEBUG,"\r加速度(g/s)\t\t: %7.5f\t %7.5f\t %7.5f\t\r", ACCEL[0], ACCEL[1], ACCEL[2]);
				#endif //USART_Print_IN_IT_ON
			}

            if (inv_get_sensor_type_gyro(data, &accuracy, (inv_time_t *)&timestamp))
            {
                GYRO_XYZ[0] = data[0] * 1.0 / (1 << 16);
                GYRO_XYZ[1] = data[1] * 1.0 / (1 << 16);
                GYRO_XYZ[2] = data[2] * 1.0 / (1 << 16);
				#if USART_Print_IN_IT_ON
                UsartPrintf(USART_DEBUG,"角速度(rad/s)\t\t: %7.5f\t %7.5f\t %7.5f\t\r\n", GYRO_XYZ[0], GYRO_XYZ[1], GYRO_XYZ[2]);
				#endif //USART_Print_IN_IT_ON+
            }		

        }//end if (new_data)

}


