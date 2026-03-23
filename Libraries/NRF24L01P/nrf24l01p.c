#include "nrf24l01p.h"//板级支持包board support package\
                      仅支持某一个或一批同样的开发板
					  
#include "bsp_gpio.h"			  
#include "bsp_exti.h"
#include "bsp_spi.h"	


/* The following functions must be defined for this platform:
 *
 * read/write length Bytes into/from RByte/WByte return 0 if successful
 * int NRF_RWBytes(unsigned char* WByte, unsigned char* RByte, unsigned char length);
 * 
 * 
 * 
 * 
 */

#define NRF_RWBytes(A, B, C) SPIx_2Line_RWBytes(NRF24L01_SPIx, A, B, C)


static uint8_t NRF_G_STATE;//NRF中断状态

/*
接收方的某个接收地址应和发送方的发送地址相同，接收方才能接收到数据
若AutoACK，接收方接收到数据后，会按照接收通道的地址发送ACK，即若P0接收到数据，则以P0的接收地址为目标发送ACK
若发送方想接收到ACK，则发送方的P0接收地址应和发送地址相同
*/

//发送方
const uint8_t NRFTX_TX_ADDRESS[TX_ADDRESS_WIDTH] = {0x1A,0x2B,0x3C,0x4D,0x5E};
const uint8_t NRFTX_RX_ADDRESS_0[RX_ADDRESS_WIDTH] = {0x1A,0x2B,0x3C,0x4D,0x5E};//通道0接收地址
const uint8_t NRFTX_RX_ADDRESS_1[RX_ADDRESS_WIDTH] = {0x1F,0x1E,0x1D,0x1C,0x11};//通道1接收地址
/*                                                            ADDRESS_2 = 0x12;
                                                              ADDRESS_3 = 0x13;
                                                              ................
*/

//接收方
const uint8_t NRFRX_TX_ADDRESS[TX_ADDRESS_WIDTH] = {0x1A,0x2B,0x3C,0x4D,0x5E};
const uint8_t NRFRX_RX_ADDRESS_0[RX_ADDRESS_WIDTH] = {0x1A,0x2B,0x3C,0x4D,0x5E};
const uint8_t NRFRX_RX_ADDRESS_1[RX_ADDRESS_WIDTH] = {0x11,0x12,0x13,0x14,0x11};
/*                                                            ADDRESS_2 = 0x12;
                                                              ADDRESS_3 = 0x13;
                                                              ................
*/

/**
  * @brief   初始化NRF24L01+
  * @param   TxMode :是否为发送方  NRF_MODE_TX：是  NRF_MODE_RX：否
  * @retval  1：初始化成功  0：初始化失败
**/
uint8_t NRF24L01P_Init(uint8_t TxMode)
{

	BSP_GPIO_Config(NRF24L01_CE_PORT,  NRF24L01_CE_PIN,  GPIO_Mode_Out_PP);//CE引脚IO配置

	BSP_GPIO_Config(NRF24L01_NSS_PORT,  NRF24L01_NSS_PIN,  GPIO_Mode_Out_PP);//NSS引脚IO配置
	
	#if NRF24L01_IRQ_EXTI
	KEY_EXTI_Register( NRF24L01_IRQ_PORT,  NRF24L01_IRQ_PIN, 
						EXTI_Mode_Interrupt,  EXTI_Trigger_Falling);//IRQ引脚外部中断配置
	#else
	BSP_GPIO_Config(NRF24L01_IRQ_PORT,  NRF24L01_IRQ_PIN,  GPIO_Mode_IPU);//IRQ引脚IO配置
	#endif //NRF24L01_IRQ_EXTI
	
	//spi配置
	#if NRF_InitSPI
	// BSP_SPIx_Init(NRF24L01_SPIx ,  SPI_Mode_Master,  NRF24L01_SPIx_Direction,  SPI_NSS_Soft,  SPIx_SSOE_DISABLE,  NRF24L01_SPIx_GPIO_Remap,
	// 			  NRF24L01_SPIx_CPOL,  NRF24L01_SPIx_CPHA,  NRF24L01_SPIx_FirstBit,  NRF24L01_SPIx_BaudRate_Psc);
	
	// BSP_SPI1_Struct.SPIx = NRF24L01_SPIx;
	// BSP_SPI1_Struct.SPI_GPIO_Remap = NRF24L01_SPIx_GPIO_Remap;
	// BSP_SPI1_Struct.SPI_InitStruct = BSP_SPIxStructInit( SPI_Mode_Master,  NRF24L01_SPIx_Direction);
	// BSP_SPIx_Init( &BSP_SPI1_Struct);
	#endif //NRF_InitSPI

	NRF24L01_TX_ModeConfig();//上电，准备检测

	NRF24L01_CE_LOW();//切换为待机
	if(!NRF24L01_Check())return 0;//通信异常，初始化失败
	NRF24L01_CE_HIGH();//切换为收发
	
	if(TxMode==NRF_MODE_TX)
	{
		NRF24L01_TX_ModeConfig();
	}
	else
	{
		NRF24L01_RX_ModeConfig();
	}
	
	
	uint8_t state;
	state = NRF_R_Reg(STATUS);//读取状态寄存器
	NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, state);//清除所有中断标志
	
	NRF_G_STATE = 0;//清除所有中断标记

	return 1;
}	



/**	
  * @brief   NRF24L01+ 接收模式配置
  * @param   
  * @retval  
  */
void NRF24L01_RX_ModeConfig(void)
{
	NRF24L01_CE_LOW();//切换为待机

	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P0, WRX_PAYLOAD_WIDTH);//设置接收数据管道P0有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |SETUP_AW, Addr_Width5);//设置地址宽度为5字节
	NRF_W_nByte(NRF_WRITE_REG_CMD +TX_ADDR, (uint8_t*)NRFRX_TX_ADDRESS, TX_ADDRESS_WIDTH);//设置数据发送地址
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P0, (uint8_t*)NRFRX_RX_ADDRESS_0, RX_ADDRESS_WIDTH);//设置数据接收管道P0的接收地址
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_AA, 0x01);//使能通道0的自动应答
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_RXADDR, 0x01);//使能通道0的接收地址
	NRF_W_Reg(0x75,SETUP_RETR);//设置自动重发间隔2000us，自动重发次数5次, xy(H)高4位为延迟时间=(x+1)*250us; 低4位为重发次数y
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_CH, 40);//设置射频信道为2.44GHz，0x01为2.401GHz 0x02为2.402GHz...（2.4GHz-2.525GHz）
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_SETUP, 0x0f);//打开低噪声放大器增益，发射功率为0dBm，数据传输率2Mbps
	NRF_W_Reg(NRF_WRITE_REG_CMD |CONFIG, 0x0f);//打开所有中断（产生中断IRQ脚被拉低），使能16位CRC，芯片上电，设置为接收模式
	NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY);//清除TX_FIFO寄存器
	NRF_W_Reg(FLUSH_RX_CMD , NRF_DUMMY);//清除RX_FIFO寄存器
	
	NRF24L01_CE_HIGH();//切换为收发
}

/**	
* @brief   NRF24L01+ 发送模式配置
  * @param   
  * @retval  
  */
void NRF24L01_TX_ModeConfig(void)
{
	NRF24L01_CE_LOW();//切换为待机
	
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P0, WRX_PAYLOAD_WIDTH);//设置接收数据管道P0有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |SETUP_AW, Addr_Width5);//设置地址宽度为5字节
	NRF_W_nByte(NRF_WRITE_REG_CMD +TX_ADDR, (uint8_t*)NRFTX_TX_ADDRESS, TX_ADDRESS_WIDTH);//设置数据发送地址
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P0, (uint8_t*)NRFTX_RX_ADDRESS_0, RX_ADDRESS_WIDTH);//设置数据接收管道P0的接收地址
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_AA, 0x01);//使能通道0的自动应答
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_RXADDR, 0x01);//使能通道0的接收地址
	NRF_W_Reg(0x75,SETUP_RETR);//设置自动重发间隔2000us，自动重发次数5次, xy(H)高4位为延迟时间=(x+1)*250us; 低4位为重发次数y
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_CH, 40);//设置射频信道为2.44GHz，0x01为2.401GHz 0x02为2.402GHz...（2.4GHz-2.525GHz）
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_SETUP, 0x0f);//打开低噪声放大器增益，发射功率为0dBm，数据传输率2Mbps
	NRF_W_Reg(NRF_WRITE_REG_CMD |CONFIG, 0x0e);//打开所有中断（产生中断IRQ脚被拉低），使能16位CRC，芯片上电，设置为发送模式
	NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY);//清除TX_FIFO寄存器
	NRF_W_Reg(FLUSH_RX_CMD , NRF_DUMMY);//清除RX_FIFO寄存器
	
	NRF24L01_CE_HIGH();//切换为收发
}


/**	
* @brief   NRF24L01+ 收发模式切换及发送地址修改
* @param   TxAddr : 发送地址
* @param   SwFlag : 模式切换标志
*	@arg   SWFlag_2RxSwAddr ： 切换为接收并修改发送地址
*	@arg   SWFlag_2TxSwAddr ： 切换为发送并修改发送地址
*	@arg   SWFlag_Only2Rx ： 只切换为接收
*	@arg   SWFlag_Only2Tx ： 只切换为发送
* @retval  
  */
void NRF24L01_TXRX_Mode_Switch(uint8_t* TxAddr, uint8_t SwFlag)
{
	NRF24L01_CE_LOW();//切换为待机
	
	if(SwFlag&0x01)
	{
		NRF_W_nByte(NRF_WRITE_REG_CMD +TX_ADDR, (uint8_t*)TxAddr, TX_ADDRESS_WIDTH);//设置数据发送地址
	}
	
	if(SwFlag&0x10)
	{
		NRF_W_Reg(NRF_WRITE_REG_CMD |CONFIG, 0x0f);//打开所有中断（产生中断IRQ脚被拉低），使能16位CRC，芯片上电，设置为接收模式
	}
	else
	{
		NRF_W_Reg(NRF_WRITE_REG_CMD |CONFIG, 0x0e);//打开所有中断（产生中断IRQ脚被拉低），使能16位CRC，芯片上电，设置为发射模式
	}
	
	NRF24L01_CE_HIGH();//切换为收发
	
	
	uint8_t state;
	state = NRF_R_Reg(STATUS);//读取状态寄存器
	NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, state);//清除所有中断标志
	
	NRF_G_STATE = 0;//清除所有中断标记
}


/**	写入001X XXXX(X为寄存器地址)命令，表示写对应寄存器
  * @brief   写NRF24L01+寄存器或命令
  * @param   Reg ：寄存器
  * @param   WData ：数据
  * @retval  
  */
void NRF_W_Reg(uint8_t Reg, uint8_t WData)
{
	
	uint8_t WByte[2], RByte[2];
	
	WByte[0] = Reg;
	WByte[1] = WData;
	NRF24L01_NSS_LOW();//片选
	NRF_RWBytes(  WByte, RByte,  2);
	NRF24L01_NSS_HIGH();//取消片选
}



/** 写入000X XXXX(X为寄存器地址)命令，表示读对应寄存器，返回对应寄存器的值
  * @brief   读NRF24L01+寄存器 1Byte
  * @param   Reg ：寄存器
  * @retval  读取的数据
  */
uint8_t NRF_R_Reg(uint8_t Reg)
{
	uint8_t WByte,  RByte;

	WByte = Reg;
	NRF24L01_NSS_LOW();//片选
	NRF_RWBytes(  &WByte, &RByte,  1);
	WByte = NRF_DUMMY;
	NRF_RWBytes(  &WByte, &RByte,  1);
	NRF24L01_NSS_HIGH();//取消片选
	return RByte;
}

/** 
  * @brief   写n个Byte的数据
  * @param   Reg ：指令/寄存器
  * @retval  0表示成功
  */
uint8_t NRF_W_nByte(uint8_t Reg, uint8_t* WDataBuf, uint8_t n)
{
	uint8_t  RByte[n];

	uint32_t sta;
	NRF24L01_NSS_LOW();//片选
	sta = NRF_RWBytes(  &Reg, RByte,  1);
	if(!sta) sta = NRF_RWBytes(  WDataBuf, RByte,  n);
	NRF24L01_NSS_HIGH();//取消片选
	return sta;
}


/** 
  * @brief   读n个Byte的数据
  * @param   Reg ：指令/寄存器
  * @retval  0表示读取成功
  */
uint8_t NRF_R_nByte(uint8_t Reg, uint8_t* RDataBuf, uint8_t n)
{
	uint8_t  WByte[n];

	uint32_t sta;

	for(uint8_t i=0;i<n;i++)
	{
		WByte[i] = NRF_DUMMY;
	}
	NRF24L01_NSS_LOW();//片选
	sta = NRF_RWBytes(  &Reg, RDataBuf,  1);
	if(!sta) sta = NRF_RWBytes(  WByte, RDataBuf,  n);
	NRF24L01_NSS_HIGH();//取消片选
	return sta;
}


#if NRF24L01_IRQ_EXTI


/** 
* @brief   NRF IRQ引脚外部中断处理函数；含中断标志清除
* @param  
  * @retval  
**/
void NRF24L01_IRQ_Handler(void)
{	
	if(EXTI_GetITStatus(NRF24L01_IRQ_EXTI_Line) !=RESET )//判断是否进入中断，若进入中断则执行后面的中断程序
	{
		/**********************中断程序*********************/
		NRF_G_STATE = NRF_R_Reg(STATUS);//读取状态寄存器
		NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, NRF_G_STATE);//清除所有中断标志
		/**************************************************/
		EXTI_ClearITPendingBit(NRF24L01_IRQ_EXTI_Line);//完成中断后清除中断标志位
	}
	
}

/** 
* @brief   发送数据包
* @param   TxBuf ：需要发送的数据 最大32Byte
  * @retval  0表示成功
  */
uint8_t NRF24L01_TxPacket(uint8_t* TxBuf)
{
	uint32_t time_out;
	
	NRF24L01_CE_LOW();//切换为待机
	NRF_W_nByte(WR_TX_PLOAD_CMD , TxBuf, WRX_PAYLOAD_WIDTH);//写入数据到发送缓冲区
	NRF24L01_CE_HIGH();//切换为收发，等待发送

	time_out = NRF_LONG_TIMEOUT;
	while(!(NRF_G_STATE&STATUS_TX_DS))//等待发送完成,产生中断
	{
		if((time_out--)==0) 
		{
			NRF_DEBUG("NRF 等待超时!errorCode = %d",NRF_ERRCODE_GET_TX_STA_TIMEOUT);
			break;
		}
	}
	
	if(NRF_G_STATE&STATUS_TX_DS)
	{
		NRF_G_STATE&=~STATUS_TX_DS;//清除发送完成中断标志
		NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY);//清除TX_FIFO寄存器
		return 0;//发送成功
	}
	else if(NRF_G_STATE&STATUS_MAX_RT)//达到最大重发次数
	{
		NRF_G_STATE&=~STATUS_MAX_RT;//清除达到最大重发次数中断标志 
		NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY);//清除TX_FIFO寄存器
		NRF_DEBUG("NRF 达到最大重发次数!errorCode = %d",NRF_ERRCODE_TX_MAX);
		return STATUS_MAX_RT;
	}
	return NRF_ERRCODE_GET_TX_STA_TIMEOUT;
}

/** 
* @brief   接收数据包，反复接受直至超时
* @param   RxBuf ：接收的数据 最大32Byte
  * @retval  0表示成功
  */
uint8_t NRF24L01_RxPacket(uint8_t* RxBuf)
{
	uint32_t time_out;
	time_out = NRF_LONG_TIMEOUT;
	while(!(NRF_G_STATE&STATUS_RX_DR))//等待接收完成,产生中断
	{
		if((time_out--)==0) 
		{
			NRF_DEBUG("NRF 等待超时!errorCode = %d",NRF_ERRCODE_GET_RX_STA_TIMEOUT);
			break;
		}
	}
	
	if(NRF_G_STATE&STATUS_RX_DR)
	{
		NRF_R_nByte(RD_RX_PLOAD_CMD , RxBuf, WRX_PAYLOAD_WIDTH);//读取数据
		NRF_G_STATE&=~STATUS_RX_DR;//清除接收到有效数据标志
		NRF_W_Reg(FLUSH_RX_CMD, NRF_DUMMY);//清除RX_FIFO寄存器
		return 0;//接收成功
	}
	return NRF_ERRCODE_GET_RX_STA_TIMEOUT;//接收失败
}


/** 
* @brief   接收数据包，超时极短
* @param   RxBuf ：接收的数据 最大32Byte
  * @retval  0表示成功
  */
 uint8_t NRF24L01_FastRxPacket(uint8_t* RxBuf)
 {
	 uint32_t time_out;
	 time_out = NRF_FAST_TIMEOUT;
	 while(!(NRF_G_STATE&STATUS_RX_DR))//等待接收完成,产生中断
	 {
		 if((time_out--)==0) 
		 {
			 //NRF_DEBUG("NRF 等待超时!errorCode = %d",NRF_ERRCODE_GET_RX_STA_TIMEOUT);
			 break;
		 }
	 }
	 
	 if(NRF_G_STATE&STATUS_RX_DR)
	 {
		 NRF_R_nByte(RD_RX_PLOAD_CMD , RxBuf, WRX_PAYLOAD_WIDTH);//读取数据
		 NRF_G_STATE&=~STATUS_RX_DR;//清除接收到有效数据标志
		 NRF_W_Reg(FLUSH_RX_CMD, NRF_DUMMY);//清除RX_FIFO寄存器
		 return 0;//接收成功
	 }
	 return NRF_ERRCODE_GET_RX_STA_TIMEOUT;//接收失败
 }
 


#else // #if NRF24L01_IRQ_EXTI

/** 
* @brief   发送数据包
* @param   TxBuf ：需要发送的数据 最大32Byte
  * @retval  0表示成功
  */
uint8_t NRF24L01_TxPacket(uint8_t* TxBuf)
{
	uint32_t time_out;
	
	NRF24L01_CE_LOW();//切换为待机
	NRF_W_nByte(WR_TX_PLOAD_CMD , TxBuf, WRX_PAYLOAD_WIDTH);//写入数据到发送缓冲区
	NRF24L01_CE_HIGH();//切换为收发，等待发送

	time_out = NRF_LONG_TIMEOUT;
	NRF_G_STATE = NRF_R_Reg(STATUS);//读取状态寄存器
	while (!(NRF_G_STATE&STATUS_TX_DS))//等待发送完成
	{
		NRF_G_STATE = NRF_R_Reg(STATUS);//读取状态寄存器
		if((time_out--)==0) 
		{
			NRF_DEBUG("NRF 等待超时!errorCode = %d",NRF_ERRCODE_GET_TX_STA_TIMEOUT);
			break;
		}
	}
	
	NRF_G_STATE = NRF_R_Reg(STATUS);//读取状态寄存器
	if(NRF_G_STATE&STATUS_TX_DS)
	{
		NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY);//清除TX_FIFO寄存器
		NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, STATUS_TX_DS);//清除所有中断标志
		return 0;//发送成功
	}
	else if(NRF_G_STATE&STATUS_MAX_RT)//达到最大重发次数
	{
		NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY);//清除TX_FIFO寄存器
		NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, STATUS_MAX_RT);//清除所有中断标志
		NRF_DEBUG("NRF 达到最大重发次数!errorCode = %d",NRF_ERRCODE_TX_MAX);
		return STATUS_MAX_RT;
	}
	return NRF_ERRCODE_GET_TX_STA_TIMEOUT;
}

/** 
* @brief   接收数据包
* @param   RxBuf ：接收的数据 最大32Byte
  * @retval  0表示成功
  */
uint8_t NRF24L01_RxPacket(uint8_t* RxBuf)
{
	uint32_t time_out;
	time_out = NRF_LONG_TIMEOUT;
	NRF_G_STATE = NRF_R_Reg(STATUS);//读取状态寄存器
	while(!(NRF_G_STATE&STATUS_RX_DR))//等待接收完成,产生中断
	{
		NRF_G_STATE = NRF_R_Reg(STATUS);//读取状态寄存器
		if((time_out--)==0) 
		{
			NRF_DEBUG("NRF 等待超时!errorCode = %d",NRF_ERRCODE_GET_RX_STA_TIMEOUT);
			break;
		}
	}

	NRF_G_STATE = NRF_R_Reg(STATUS);//读取状态寄存器

	if(NRF_G_STATE&STATUS_RX_DR)
	{
		NRF_R_nByte(RD_RX_PLOAD_CMD , RxBuf, WRX_PAYLOAD_WIDTH);//读取数据
		NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, STATUS_RX_DR);//清除所有中断标志
		NRF_W_Reg(FLUSH_RX_CMD, NRF_DUMMY);//清除RX_FIFO寄存器
		return 0;//接收成功
	}
	return NRF_ERRCODE_GET_RX_STA_TIMEOUT;//接收失败
}
#endif //NRF24L01_IRQ_EXTI


/**
  * @brief   NRF超时回调函数
  * @param   errorCode ：错误代码 
  * @retval  default errorCode
**/
uint8_t NRF_TIMEOUT_UserCallback(uint32_t errorCode)
{
  /* Block communication and all processes */
  NRF_DEBUG("NRF 等待超时!errorCode = %d",errorCode);
  
  return errorCode;
}

/**
  * @brief   NRF通信检测
  * @param   
  * @retval  1：设备通信正常， 0：通信异常
**/
#include "bsp_usart.h"
uint8_t NRF24L01_Check(void)
{
	uint8_t checkbuf[] = {0x12,0x13,0x14,0x15,0x16};
	uint8_t Rcheckbuf[5] = {0,0,0,0,0};
	uint8_t i;
	NRF_W_nByte(NRF_WRITE_REG_CMD|TX_ADDR, checkbuf, 5);
	NRF_R_nByte(TX_ADDR, Rcheckbuf, 5);
	
	
	NRF_DEBUG( "NRF24L01_CheckRX:\n");
	for(uint8_t i=0;i<5;i++)
	{
		NRF_DEBUG( "%d, ", Rcheckbuf[i]);
	}
	NRF_DEBUG("\n\n");
	
	for(i = 0;i<5;i++)
	{
		if(!(Rcheckbuf[i]==checkbuf[i]))
		{
			break;
		}
	}
	if(i==5)
		return 1;
	else 
		return 0;
}



#if TEST_NRF24L01


/**
* @brief   NRF发送方测试
  * @param   
  * @retval  
**/
void NRF24L01_TX_TestConfig(void)
{
	NRF24L01_CE_LOW();//切换为待机
	
	
	NRF_W_Reg(NRF_WRITE_REG_CMD |SETUP_AW, Addr_Width5);//设置地址宽度为5字节
	NRF_W_nByte(NRF_WRITE_REG_CMD +TX_ADDR, (uint8_t*)NRFTX_TX_ADDRESS, TX_ADDRESS_WIDTH);//设置数据发送地址
	
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P0, WRX_PAYLOAD_WIDTH);//设置接收数据管道P0有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P1, WRX_PAYLOAD_WIDTH);//设置接收数据管道P1有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P2, WRX_PAYLOAD_WIDTH);//设置接收数据管道P2有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P3, WRX_PAYLOAD_WIDTH);//设置接收数据管道P3有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P4, WRX_PAYLOAD_WIDTH);//设置接收数据管道P4有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P5, WRX_PAYLOAD_WIDTH);//设置接收数据管道P5有效数据宽度
	
	
	uint8_t tmp_addr;
	tmp_addr = NRFTX_RX_ADDRESS_1[RX_ADDRESS_WIDTH-1];
	
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P0, (uint8_t*)NRFTX_RX_ADDRESS_0, RX_ADDRESS_WIDTH);//设置数据接收管道P0的接收地址
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P1, (uint8_t*)NRFTX_RX_ADDRESS_1, RX_ADDRESS_WIDTH);//设置数据接收管道P1的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P2, &tmp_addr, 1);//设置数据接收管道P2的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P3, &tmp_addr, 1);//设置数据接收管道P3的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P4, &tmp_addr, 1);//设置数据接收管道P4的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P5, &tmp_addr, 1);//设置数据接收管道P5的接收地址
	
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_AA, 0x3f);//使能通道012345的自动应答
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_RXADDR, 0x3f);//使能通道012345的接收地址
	
	NRF_W_Reg(0x75,SETUP_RETR);//设置自动重发间隔2000us，自动重发次数5次, xy(H)高4位为延迟时间=(x+1)*250us; 低4位为重发次数y
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_CH, 40);//设置射频信道为2.44GHz，0x01为2.401GHz 0x02为2.402GHz...（2.4GHz-2.525GHz）
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_SETUP, 0x0f);//打开低噪声放大器增益，发射功率为0dBm，数据传输率2Mbps
	NRF_W_Reg(NRF_WRITE_REG_CMD |CONFIG, 0x0e);//打开所有中断（产生中断IRQ脚被拉低），使能16位CRC，芯片上电，设置为发送模式
	NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY);//清除TX_FIFO寄存器
	NRF_W_Reg(FLUSH_RX_CMD , NRF_DUMMY);//清除RX_FIFO寄存器
	
	
	uint8_t state;
	state = NRF_R_Reg(STATUS);//读取状态寄存器
	NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, state);//清除所有中断标志
	
	NRF24L01_CE_HIGH();//切换为收发
	
}


/**
* @brief   NRF接收方测试
  * @param   
  * @retval  
**/
void NRF24L01_RX_TestConfig(void)
{
	NRF24L01_CE_LOW();//切换为待机
	
	
	NRF_W_Reg(NRF_WRITE_REG_CMD |SETUP_AW, Addr_Width5);//设置地址宽度为5字节
	NRF_W_nByte(NRF_WRITE_REG_CMD +TX_ADDR, (uint8_t*)NRFRX_TX_ADDRESS, TX_ADDRESS_WIDTH);//设置数据发送地址
	
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P0, WRX_PAYLOAD_WIDTH);//设置接收数据管道P0有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P1, WRX_PAYLOAD_WIDTH);//设置接收数据管道P1有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P2, WRX_PAYLOAD_WIDTH);//设置接收数据管道P2有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P3, WRX_PAYLOAD_WIDTH);//设置接收数据管道P3有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P4, WRX_PAYLOAD_WIDTH);//设置接收数据管道P4有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P5, WRX_PAYLOAD_WIDTH);//设置接收数据管道P5有效数据宽度
	
	
	uint8_t tmp_addr;
	tmp_addr = NRFRX_RX_ADDRESS_1[RX_ADDRESS_WIDTH-1];
	
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P0, (uint8_t*)NRFRX_RX_ADDRESS_0, RX_ADDRESS_WIDTH);//设置数据接收管道P0的接收地址
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P1, (uint8_t*)NRFRX_RX_ADDRESS_1, RX_ADDRESS_WIDTH);//设置数据接收管道P1的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P2, &tmp_addr, 1);//设置数据接收管道P2的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P3, &tmp_addr, 1);//设置数据接收管道P3的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P4, &tmp_addr, 1);//设置数据接收管道P4的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P5, &tmp_addr, 1);//设置数据接收管道P5的接收地址
	
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_AA, 0x3f);//使能通道012345的自动应答
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_RXADDR, 0x3f);//使能通道012345的接收地址
	
	NRF_W_Reg(0x75,SETUP_RETR);//设置自动重发间隔2000us，自动重发次数5次, xy(H)高4位为延迟时间=(x+1)*250us; 低4位为重发次数y
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_CH, 40);//设置射频信道为2.44GHz，0x01为2.401GHz 0x02为2.402GHz...（2.4GHz-2.525GHz）
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_SETUP, 0x0f);//打开低噪声放大器增益，发射功率为0dBm，数据传输率2Mbps
	NRF_W_Reg(NRF_WRITE_REG_CMD |CONFIG, 0x0f);//打开所有中断（产生中断IRQ脚被拉低），使能16位CRC，芯片上电，设置为接收模式
	NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY);//清除TX_FIFO寄存器
	NRF_W_Reg(FLUSH_RX_CMD , NRF_DUMMY);//清除RX_FIFO寄存器
	
	
	uint8_t state;
	state = NRF_R_Reg(STATUS);//读取状态寄存器
	NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, state);//清除所有中断标志
	
	
	NRF24L01_CE_HIGH();//切换为收发
	
}


/**
* @brief   NRF收发测试
* @param    TxMode :是否为发送方  1：是  0：否
* @retval  1 : 测试成功  0：测试失败  
**/

#include "bsp_usart.h"
uint8_t NRF_TXRX_TEST(uint8_t TxMode)
{
	uint8_t test_Byte = 6;
	uint8_t checkbuf[WRX_PAYLOAD_WIDTH] = {0x10,0x11,0x12,0x13,0x14,0x15};
	uint8_t Rcheckbuf[WRX_PAYLOAD_WIDTH] = {0,0,0,0,0,0};
	
	if(TxMode)
	{
		NRF24L01_TX_TestConfig();
		uint8_t tmp = NRF24L01_TxPacket (checkbuf);//发送测试数据
		if (!tmp)//发送成功
		{
			NRF24L01_TXRX_Mode_Switch( (uint8_t*)NRFTX_TX_ADDRESS,  SWFlag_Only2Rx  );//切换为接收
			
			NRF_DEBUG( "NRF_TXMode_Tx:\n");
			for(uint8_t i=0;i<test_Byte;i++)
			{
				NRF_DEBUG( "%d, ", checkbuf[i]);
			}
			NRF_DEBUG( "\n\n");
			
			uint8_t tmp=0xff ;
			while(tmp)
			{
				tmp=NRF24L01_RxPacket( Rcheckbuf);//接收测试数据
			}
			
			if(!tmp)//接收测试数据
			{
				for (uint8_t i = 0; i < test_Byte; i++)//收到的数据是发出的+1
				{
					checkbuf[i]+=1;
				}
				
				NRF_DEBUG( "NRF_TXMode_Rx:\n");
				for(uint8_t i=0;i<test_Byte;i++)
				{
					NRF_DEBUG( "%d, ", Rcheckbuf[i]);
				}
				NRF_DEBUG( "\n\n");
				
				if(Compare_Array( checkbuf, Rcheckbuf,  test_Byte))//接收数据正确
				{
					return 1;
				}
			}
			else
				NRF_DEBUG( "NRF TXMode Rx Failed！:\n");
		}
		else
			NRF_DEBUG( "NRF TXMode TxFailed!\n");
		return 0;
	}
	else
	{
		NRF24L01_RX_TestConfig();
		uint8_t tmp=0xff ;
		while(tmp)
		{
			tmp=NRF24L01_RxPacket( Rcheckbuf);//接收测试数据
		}
		if (!tmp)//接收成功
		{
			
			NRF_DEBUG( "NRF_RXMode_Rx:\n");
			for(uint8_t i=0;i<test_Byte;i++)
			{
				NRF_DEBUG( "%d, ", Rcheckbuf[i]);
			}
			NRF_DEBUG( "\n\n");
			
			
			if(Compare_Array( checkbuf, Rcheckbuf,  test_Byte))//接收数据正确
			{
				NRF24L01_TXRX_Mode_Switch( (uint8_t*)NRFRX_TX_ADDRESS,  SWFlag_Only2Tx );//切换为发送

				for (uint8_t i = 0; i < test_Byte; i++)//收到的数据+1后发回去
				{
					checkbuf[i]+=1;
				}
				

				if(!NRF24L01_TxPacket (checkbuf))//发送测试数据成功
				{
					
					NRF_DEBUG( "NRF_RXMode_Tx:\n");
					for(uint8_t i=0;i<test_Byte;i++)
					{
						NRF_DEBUG( "%d, ", checkbuf[i]);
					}
					NRF_DEBUG( "\n\n");
					
					return 1;
				}
				else
					NRF_DEBUG( "NRF RXMode Tx Failed!:\n");
			}
		}
		return 0;
	}
	
}



#endif //TEST_NRF24L01

/**
* @brief   比较数组是否相同
* @param    arr1 : 数组1
* @param    arr2 : 数组2
* @param    len : 数组长度
* @retval  1 : 相同  0：不同  
**/
uint8_t Compare_Array(uint8_t* arr1, uint8_t* arr2, uint8_t len)
{

	uint8_t i;
	for (i = 0; i < len; i++)
	{
		if (*(arr1+i)!=*(arr2+i))
		{
			break;
		}
		
	}
	
	if (i==len)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}


