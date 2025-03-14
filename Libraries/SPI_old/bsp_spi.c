
#include "bsp_spi.h"

									  
void BSP_SPI_Init(void)
{
	SPI_InitTypeDef SPI_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/******************************SPI配置************************************/
	SPI_CLKCMD_FUNC(SPI_CLK_PORT,ENABLE);//打开SPI外设时钟
	
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//定义SPI数据收发方向
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//选择SPI主从
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//选择单次收发数据大小
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;//选择时钟极性(空闲时钟电平)
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;//选择时钟相位(数据奇边沿采样还是偶边沿采样)
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//选择NSS为硬件控制还是软件控制
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//设置波特率分频因子
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//设置先发低字节还是高字节
	SPI_InitStruct.SPI_CRCPolynomial = 0;//指定CRC校验的多项式，本实验不校验数据，多项式随便写
	
	
	SPI_Init(SPI_NUM, &SPI_InitStruct);//将配置的SPI参数写入寄存器
	
	
	/******************************GPIO配置***********************************/
	//模式配置见参考手册（通用复用功能I/O）
	/*NSS引脚*/
	SPI_NSS_CLKCMD_FUNC(SPI_NSS_CLK,ENABLE);//打开NSS引脚端口时钟
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = SPI_NSS_PIN;
	GPIO_Init(SPI_NSS_PORT, &GPIO_InitStruct);//将配置的NSS引脚参数写入寄存器
	
	/*CK引脚*/
	//SPI_CK_CLKCMD_FUNC(SPI_CK_CLK,ENABLE);//打开CK引脚端口时钟(和NSS引脚一样,不重复打开了)
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = SPI_CK_PIN;
	GPIO_Init(SPI_CK_PORT, &GPIO_InitStruct);//将配置的NSS引脚参数写入寄存器
	
	/*MISO引脚*/
	//SPI_MISO_CLKCMD_FUNC(SPI_MISO_CLK,ENABLE);//打开MISO引脚端口时钟(和NSS引脚一样,不重复打开了)
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = SPI_MISO_PIN;
	GPIO_Init(SPI_MISO_PORT, &GPIO_InitStruct);//将配置的MISO引脚参数写入寄存器
	
	/*MOSI引脚*/
	//SPI_MOSI_CLKCMD_FUNC(SPI_MOSI_CLK,ENABLE);//打开MOSI引脚端口时钟(和NSS引脚一样,不重复打开了)
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = SPI_MOSI_PIN;
	GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStruct);//将配置的MISO引脚参数写入寄存器
	
	SPI_NSS_HIGH();//停止信号传输 
}	

/*
收发一体
*/
uint8_t Spi_W_RByte(uint8_t Byte)
{

	if(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE)!=SET)//检测SPI发送缓存器是否为空(可写入要发送的数据)
	{
		SPI_I2S_SendData(SPI_NUM, Byte);//向发送缓存器DR写入数据
	}
	
	/*因为全双工收发同步，所以收到数据表明已发送完成*/
	if(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE)!=SET)//检测SPI接收缓存器是否不为空(已接收完数据数据)
	{
		return SPI_I2S_ReceiveData(SPI_NUM);//返回收到的数据
	}
	
	return 0x00;
}

/*
发送数据
*/
void Spi_WriteByte(uint8_t Byte)
{
	Spi_W_RByte(Byte);
}

/*
接收数据
*/
uint8_t Spi_ReadByte(uint8_t Byte)
{
	return Spi_W_RByte(DUMMY);//发送无关数据
}


/*
spi测试程序
此为主机
主机向从机发送0x15
从机将接收的数据加一再发回来
*/
void SPI_Test(void)
{
	SPI_Cmd(SPI_NUM, ENABLE);
	SPI_NSS_LOW();//选择从机
	__nop();
	__nop();//等待从机收到被选中信号
	SPI_I2S_ITConfig(SPI_NUM, SPI_I2S_IT_RXNE, ENABLE);//打开SPI接收中断
	SPI_I2S_ITConfig(SPI_NUM, SPI_I2S_IT_TXE, ENABLE);//打开SPI发送中断，系统产生中断开始发送数据
	
}

