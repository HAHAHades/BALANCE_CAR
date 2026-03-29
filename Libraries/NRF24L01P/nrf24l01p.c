#include "nrf24l01p.h"//板级支持包board support package\
                      仅支持某一个或一批同样的开发板
					  
#include "bsp_gpio.h"			  
#include "bsp_exti.h"


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

NRF24L01P_Hard_Typedef* G_NRF_HardStruct_ForEXTI0;//全局指针，仅为中断函数使用


void NRF24L01P_UseageDemo(void)
{
	
	uint8_t tmpR;
	NRF24L01P_Hard_Typedef NRF_HardStruct;
	NRF_WorkMode_enumt WorkMode = NRF_Mode_Tx;
	NRF_HardStruct_Init(&NRF_HardStruct, WorkMode, NRF_ACK_PAY_DIS,  SPI1,  SPI1_IO_Reamp0, GPIOB,  GPIO_Pin_1,  GPIOB,  GPIO_Pin_0);
	
	//全局指针方便调用中断函数
	G_NRF_HardStruct_ForEXTI0 = &NRF_HardStruct;
	tmpR = NRF24L01P_Init(&NRF_HardStruct);
	if (tmpR)
	{
		NRF_DEBUG("NRF24L01P_Init Failed!");
		return;
	}
	
	//等待 中断 调用NRF24L01_IRQ_Handler 更新 NRF_G_STATE
	uint8_t TxBuf[WRX_PAYLOAD_WIDTH];
	uint8_t RxBuf[WRX_PAYLOAD_WIDTH];
	uint8_t rVal;
	uint32_t time_out = 10;
	while (1)
	{
		//TxBuf=.....
		
		//发送方
		rVal = NRF24L01_TxPacket(TxBuf, time_out, &NRF_HardStruct);
		if (rVal)
		{
			NRF_DEBUG("err code:", rVal);
		}
		 
		//接收方
		rVal = NRF24L01_RxPacket( RxBuf, time_out, &NRF_HardStruct);
		if (rVal)
		{
			NRF_DEBUG("err code:", rVal);
		}
		else
		{
			//接受成功， 处理数据
			
		}
	}
}	


/**
  * @brief   获取时间戳/ms
  * @param   
  * @retval  
**/
uint32_t NRF24L01_GetTime(void) 
{
	unsigned long count;
	get_tick_count(&count);
	return count;
}


/**	
  * @brief   NRF24L01+ 硬件配置，默认全双工SPI,NSS_Soft,CPOL_High,CPHA_2Edge,8b,MSB,2分频,无校验，默认开启外部中断
  * @param   NRF_HardStruct[IN/OUT]: 需配置的NRF
  * @param   WorkMode[IN]: NRF工作模式  @ref NRF24L01P_WorkMode
  * @param   AckPay[IN]: ACK是否携带数据 @ref NRF_ACK_PAY_enumt
  * @param   SPIx[IN]: NRF使用的SPI
  * @param   SPI_GPIO_Remap[IN]: @ref SPIx_IO_Reamp
  * @param   NRF_CE_PORTx[IN]: 使能引脚
  * @param   NRF_CE_PINx[IN]: 
  * @param   NRF_EXTI_PORTx[IN]: 外部中断引脚
  * @param   NRF_EXTI_PINx[IN]: 
  * @retval  0: successful   else: failed
  */
uint8_t NRF_HardStruct_Init(NRF24L01P_Hard_Typedef* NRF_HardStruct, NRF_WorkMode_enumt WorkMode, NRF_ACK_PAY_enumt AckPay, SPI_TypeDef* SPIx, uint32_t SPI_GPIO_Remap,
							GPIO_TypeDef* NRF_CE_PORTx, uint16_t NRF_CE_PINx, GPIO_TypeDef* NRF_EXTI_PORTx, uint16_t NRF_EXTI_PINx)
{
	uint8_t rVal = 0;

	NRF_HardStruct->WorkMode = WorkMode;
	NRF_HardStruct->BSP_SPIX_Struct.SPIx = SPIx;
	NRF_HardStruct->BSP_SPIX_Struct.SPI_GPIO_Remap = SPI_GPIO_Remap;
	NRF_HardStruct->BSP_SPIX_Struct.SPI_InitStruct = BSP_SPIxStructInit( SPI_Mode_Master,  SPI_Direction_2Lines_FullDuplex);;
	NRF_HardStruct->NRF_CE_PORTx = NRF_CE_PORTx;
	NRF_HardStruct->NRF_CE_PINx = NRF_CE_PINx;
	NRF_HardStruct->NRF_EXTI_PORTx = NRF_EXTI_PORTx;
	NRF_HardStruct->NRF_EXTI_PINx = NRF_EXTI_PINx;
	NRF_HardStruct->NRF_IRQ_EXTI_Line = (uint32_t)NRF_EXTI_PINx;
	NRF_HardStruct->NRF_UseEXTI = 1;
	NRF_HardStruct->NRF_G_STATE = 0;

	return rVal;
}


/**	
  * @brief   NRF24L01+ 配置
  * @param   NRF_HardStruct[IN/OUT]: 
  * @retval  0: successful   else: failed
  */
uint8_t NRF24L01P_Init(NRF24L01P_Hard_Typedef* NRF_HardStruct)
{
	uint8_t rVal = 0;


	BSP_GPIO_Config(NRF_HardStruct->NRF_CE_PORTx,  NRF_HardStruct->NRF_CE_PINx,  GPIO_Mode_Out_PP);//CE引脚IO配置

	if (NRF_HardStruct->NRF_UseEXTI)
	{
		KEY_EXTI_Register( NRF_HardStruct->NRF_EXTI_PORTx,  NRF_HardStruct->NRF_EXTI_PINx, EXTI_Mode_Interrupt,  EXTI_Trigger_Falling);//IRQ引脚外部中断配置
	}
	else
	{
		BSP_GPIO_Config(NRF_HardStruct->NRF_EXTI_PORTx,   NRF_HardStruct->NRF_EXTI_PINx,  GPIO_Mode_IPU);//IRQ引脚IO配置
	}

	
	//spi配置
	if(BSP_SPIx_Init( &(NRF_HardStruct->BSP_SPIX_Struct)) != SPIx_RET_Flag_OK)
	{
		rVal = 1;
		NRF_DEBUG("NRF SPI Init Failed!");
		return rVal;
	}

	NRF24L01_TX_ModeConfig(NRF_DefaultFrequency, NRF_HardStruct);//上电，准备检测
	
	NRF24L01_NSS_LOW(NRF_HardStruct);//切换为待机
	if(NRF24L01_Check(*NRF_HardStruct))
	{
		NRF_DEBUG("NRF Check Failed!");
		rVal = 1;
		return rVal;
		//通信异常，初始化失败
	}
	NRF24L01_NSS_HIGH(NRF_HardStruct);//切换为收发

	if(NRF_HardStruct->WorkMode == NRF_Mode_Tx)
	{
		NRF24L01_TX_ModeConfig(NRF_DefaultFrequency, NRF_HardStruct);
	}
	else
	{
		NRF24L01_RX_ModeConfig(NRF_DefaultFrequency, NRF_HardStruct);
	}
	
	uint8_t state;
	state = NRF_R_Reg(STATUS, NRF_HardStruct);//读取状态寄存器
	NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, state, NRF_HardStruct);//清除所有中断标志
	
	NRF_HardStruct->NRF_G_STATE = 0;//清除所有中断标记


	return rVal;

}


/**	
* @brief   NRF24L01+ 接收模式配置
* @param   NRF_HardStruct[IN]: 配置的NRF
* @param   f : 通信频率  (2.4+0.001*f)/GHz f<125
* @retval  
*/
void NRF24L01_RX_ModeConfig(uint8_t f, NRF24L01P_Hard_Typedef* NRF_HardStruct)
{
	NRF24L01_CE_LOW(NRF_HardStruct);//切换为待机

	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P0, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//设置接收数据管道P0有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |SETUP_AW, Addr_Width5, NRF_HardStruct);//设置地址宽度为5字节
	NRF_W_nByte(NRF_WRITE_REG_CMD +TX_ADDR, (uint8_t*)NRFRX_TX_ADDRESS, TX_ADDRESS_WIDTH, NRF_HardStruct);//设置数据发送地址
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P0, (uint8_t*)NRFRX_RX_ADDRESS_0, RX_ADDRESS_WIDTH, NRF_HardStruct);//设置数据接收管道P0的接收地址
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_AA, 0x01, NRF_HardStruct);//使能通道0的自动应答
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_RXADDR, 0x01, NRF_HardStruct);//使能通道0的接收地址
	NRF_W_Reg(0x75,SETUP_RETR, NRF_HardStruct);//设置自动重发间隔2000us，自动重发次数5次, xy(H)高4位为延迟时间=(x+1)*250us; 低4位为重发次数y
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_CH, f, NRF_HardStruct);//设置射频信道为2.4+0.001*fGHz，0x01为2.401GHz 0x02为2.402GHz...（2.4GHz-2.525GHz）
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_SETUP, 0x0f, NRF_HardStruct);//打开低噪声放大器增益，发射功率为0dBm，数据传输率2Mbps
	NRF_W_Reg(NRF_WRITE_REG_CMD |CONFIG, 0x0f, NRF_HardStruct);//打开所有中断（产生中断IRQ脚被拉低），使能16位CRC，芯片上电，设置为接收模式
	NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY, NRF_HardStruct);//清除TX_FIFO寄存器
	NRF_W_Reg(FLUSH_RX_CMD , NRF_DUMMY, NRF_HardStruct);//清除RX_FIFO寄存器
	
	if (NRF_HardStruct->AckPay==NRF_ACK_PAY_EN)
	{
		NRF_W_Reg(NRF_WRITE_REG_CMD|FEATURE , 0x06, NRF_HardStruct);//使能动态负载长度，使能自动应答携带负载
		NRF_W_Reg(NRF_WRITE_REG_CMD|DYNPD , 0x01, NRF_HardStruct);//使能通道0动态负载长度
	}

	uint8_t state;
	state = NRF_R_Reg(STATUS, NRF_HardStruct);//读取状态寄存器
	NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, state, NRF_HardStruct);//清除所有中断标志

	NRF24L01_CE_HIGH(NRF_HardStruct);//切换为收发
}

/**	
* @brief   NRF24L01+ 发送模式配置
* @param   NRF_HardStruct[IN]: 配置的NRF
* @param   f : 通信频率  (2.4+0.001*f)/GHz f<125
* @retval  
*/
void NRF24L01_TX_ModeConfig(uint8_t f, NRF24L01P_Hard_Typedef* NRF_HardStruct)
{
	NRF24L01_CE_LOW(NRF_HardStruct);//切换为待机
	
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P0, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//设置接收数据管道P0有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |SETUP_AW, Addr_Width5, NRF_HardStruct);//设置地址宽度为5字节
	NRF_W_nByte(NRF_WRITE_REG_CMD +TX_ADDR, (uint8_t*)NRFTX_TX_ADDRESS, TX_ADDRESS_WIDTH, NRF_HardStruct);//设置数据发送地址
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P0, (uint8_t*)NRFTX_RX_ADDRESS_0, RX_ADDRESS_WIDTH, NRF_HardStruct);//设置数据接收管道P0的接收地址
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_AA, 0x01, NRF_HardStruct);//使能通道0的自动应答
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_RXADDR, 0x01, NRF_HardStruct);//使能通道0的接收地址
	NRF_W_Reg(0x75,SETUP_RETR, NRF_HardStruct);//设置自动重发间隔2000us，自动重发次数5次, xy(H)高4位为延迟时间=(x+1)*250us; 低4位为重发次数y
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_CH, f, NRF_HardStruct);//设置射频信道为2.4+0.001*fGHz，0x01为2.401GHz 0x02为2.402GHz...（2.4GHz-2.525GHz）
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_SETUP, 0x0f, NRF_HardStruct);//打开低噪声放大器增益，发射功率为0dBm，数据传输率2Mbps
	NRF_W_Reg(NRF_WRITE_REG_CMD |CONFIG, 0x0e, NRF_HardStruct);//打开所有中断（产生中断IRQ脚被拉低），使能16位CRC，芯片上电，设置为发送模式
	NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY, NRF_HardStruct);//清除TX_FIFO寄存器
	NRF_W_Reg(FLUSH_RX_CMD , NRF_DUMMY, NRF_HardStruct);//清除RX_FIFO寄存器
	
	if (NRF_HardStruct->AckPay==NRF_ACK_PAY_EN)
	{	
		NRF_W_Reg(NRF_WRITE_REG_CMD|FEATURE , 0x06, NRF_HardStruct);//使能动态负载长度，使能自动应答携带负载
		NRF_W_Reg(NRF_WRITE_REG_CMD|DYNPD , 0x01, NRF_HardStruct);//使能通道0动态负载长度
	}
	
	uint8_t state;
	state = NRF_R_Reg(STATUS, NRF_HardStruct);//读取状态寄存器
	NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, state, NRF_HardStruct);//清除所有中断标志

	NRF24L01_CE_HIGH(NRF_HardStruct);//切换为收发
}


/**	
* @brief   NRF24L01+ 收发模式切换及发送地址修改
* @param   TxAddr : 发送地址
* @param   SwFlag : 模式切换标志
*	@arg   SWFlag_2RxSwAddr ： 切换为接收并修改发送地址
*	@arg   SWFlag_2TxSwAddr ： 切换为发送并修改发送地址
*	@arg   SWFlag_Only2Rx ： 只切换为接收
*	@arg   SWFlag_Only2Tx ： 只切换为发送
* @param   NRF_HardStruct[IN/OUT] : 待切换模式的NRF
* @retval  
  */
void NRF24L01_TXRX_Mode_Switch(uint8_t* TxAddr, uint8_t SwFlag, NRF24L01P_Hard_Typedef* NRF_HardStruct)
{
	NRF24L01_CE_LOW(NRF_HardStruct);//切换为待机
	
	if(SwFlag&0x01)
	{
		NRF_W_nByte(NRF_WRITE_REG_CMD +TX_ADDR, (uint8_t*)TxAddr, TX_ADDRESS_WIDTH, NRF_HardStruct);//设置数据发送地址
	}
	
	if(SwFlag&0x10)
	{
		NRF_W_Reg(NRF_WRITE_REG_CMD |CONFIG, 0x0f, NRF_HardStruct);//打开所有中断（产生中断IRQ脚被拉低），使能16位CRC，芯片上电，设置为接收模式
	}
	else
	{
		NRF_W_Reg(NRF_WRITE_REG_CMD |CONFIG, 0x0e, NRF_HardStruct);//打开所有中断（产生中断IRQ脚被拉低），使能16位CRC，芯片上电，设置为发射模式
	}
	
	NRF24L01_CE_HIGH(NRF_HardStruct);//切换为收发
	
	
	uint8_t state;
	state = NRF_R_Reg(STATUS, NRF_HardStruct);//读取状态寄存器
	NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, state, NRF_HardStruct);//清除所有中断标志
	
	NRF_HardStruct->NRF_G_STATE = 0;//清除所有中断标记
}


/**	写入001X XXXX(X为寄存器地址)命令，表示写对应寄存器
  * @brief   写NRF24L01+寄存器或命令
  * @param   Reg ：寄存器
  * @param   WData ：数据
  * @param   NRF_HardStruct[IN] ：待写入的NRF
  * @retval  
  */
void NRF_W_Reg(uint8_t Reg, uint8_t WData, NRF24L01P_Hard_Typedef* NRF_HardStruct)
{
	
	uint8_t WByte[2], RByte[2];
	
	WByte[0] = Reg;
	WByte[1] = WData;
	NRF24L01_NSS_LOW(NRF_HardStruct);//片选
	NRF_RWBytes(  WByte, RByte,  2, NRF_HardStruct);
	NRF24L01_NSS_HIGH(NRF_HardStruct);//取消片选
}



/** 写入000X XXXX(X为寄存器地址)命令，表示读对应寄存器，返回对应寄存器的值
  * @brief   读NRF24L01+寄存器 1Byte
  * @param   Reg ：寄存器
  * @param   NRF_HardStruct[IN] ：待读取的NRF
  * @retval  读取的数据
  */
uint8_t NRF_R_Reg(uint8_t Reg, NRF24L01P_Hard_Typedef* NRF_HardStruct)
{
	uint8_t WByte,  RByte;

	WByte = Reg;
	NRF24L01_NSS_LOW(NRF_HardStruct);//片选
	NRF_RWBytes(  &WByte, &RByte,  1, NRF_HardStruct);
	WByte = NRF_DUMMY;
	NRF_RWBytes(  &WByte, &RByte,  1, NRF_HardStruct);
	NRF24L01_NSS_HIGH(NRF_HardStruct);//取消片选
	return RByte;
}

/** 
  * @brief   写n个Byte的数据
  * @param   Reg ：指令/寄存器
  * @param   NRF_HardStruct[IN] ：待写入的NRF
  * @retval  0表示成功
  */
uint8_t NRF_W_nByte(uint8_t Reg, uint8_t* WDataBuf, uint8_t n, NRF24L01P_Hard_Typedef* NRF_HardStruct)
{
	uint8_t  RByte[n];

	uint32_t sta;
	NRF24L01_NSS_LOW(NRF_HardStruct);//片选
	sta = NRF_RWBytes(  &Reg, RByte,  1, NRF_HardStruct);
	if(!sta) sta = NRF_RWBytes(  WDataBuf, RByte,  n, NRF_HardStruct);
	NRF24L01_NSS_HIGH(NRF_HardStruct);//取消片选
	return sta;
}


/** 
  * @brief   读n个Byte的数据
  * @param   Reg ：指令/寄存器
  * @param   NRF_HardStruct[IN] ：待读取的NRF
  * @retval  0表示读取成功
  */
uint8_t NRF_R_nByte(uint8_t Reg, uint8_t* RDataBuf, uint8_t n, NRF24L01P_Hard_Typedef* NRF_HardStruct)
{
	uint8_t  WByte[n];

	uint32_t sta;

	for(uint8_t i=0;i<n;i++)
	{
		WByte[i] = NRF_DUMMY;
	}
	NRF24L01_NSS_LOW(NRF_HardStruct);//片选
	sta = NRF_RWBytes(  &Reg, RDataBuf,  1, NRF_HardStruct);
	if(!sta) sta = NRF_RWBytes(  WByte, RDataBuf,  n, NRF_HardStruct);
	NRF24L01_NSS_HIGH(NRF_HardStruct);//取消片选
	return sta;
}


/** 
* @brief   NRF IRQ引脚外部中断处理函数；含中断标志清除
* @param   NRF_HardStruct[IN/OUT]:待处理的NRF
* @retval  
**/
void NRF24L01_IRQ_Handler(NRF24L01P_Hard_Typedef* NRF_HardStruct)
{	
	if(EXTI_GetITStatus(NRF_HardStruct->NRF_IRQ_EXTI_Line) !=RESET )//判断是否进入中断，若进入中断则执行后面的中断程序
	{
		/**********************中断程序*********************/
		NRF_HardStruct->NRF_G_STATE = NRF_R_Reg(STATUS, NRF_HardStruct);//读取状态寄存器
		NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, NRF_HardStruct->NRF_G_STATE, NRF_HardStruct);//清除所有中断标志
		/**************************************************/
		EXTI_ClearITPendingBit(NRF_HardStruct->NRF_IRQ_EXTI_Line);//完成中断后清除中断标志位
	}
	
}

/** 
* @brief   发送数据包
* @param   TxBuf ：需要发送的数据 最大32Byte
* @param   time_out ：超时时间ms
* @param   NRF_HardStruct[IN/OUT] ：待发送的NRF
* @retval  0表示成功
*/
uint8_t NRF24L01_TxPacket(uint8_t* TxBuf, uint32_t time_out, NRF24L01P_Hard_Typedef* NRF_HardStruct)
{

	if (NRF_HardStruct->NRF_UseEXTI)
	{
		//使用外部中断读取NRF状态
		uint32_t nowTime = NRF24L01_GetTime();
		uint32_t diffTime;
		
		NRF24L01_CE_LOW(NRF_HardStruct);//切换为待机
		NRF_W_nByte(WR_TX_PLOAD_CMD , TxBuf, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//写入数据到发送缓冲区
		NRF24L01_CE_HIGH(NRF_HardStruct);//切换为收发，等待发送

	
		while(!(NRF_HardStruct->NRF_G_STATE&STATUS_TX_DS))//等待发送完成,产生中断
		{
			diffTime = NRF24L01_GetTime() - nowTime;
			if(diffTime > time_out) 
			{
				// NRF_DEBUG("NRF 等待超时!errorCode = %d",NRF_ERRCODE_GET_TX_STA_TIMEOUT);
				break;
			}
		}
		
		if(NRF_HardStruct->NRF_G_STATE&STATUS_TX_DS)
		{
			NRF_HardStruct->NRF_G_STATE&=~STATUS_TX_DS;//清除发送完成中断标志
			NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY, NRF_HardStruct);//清除TX_FIFO寄存器
			return 0;//发送成功
		}
		else if(NRF_HardStruct->NRF_G_STATE&STATUS_MAX_RT)//达到最大重发次数
		{
			NRF_HardStruct->NRF_G_STATE&=~STATUS_MAX_RT;//清除达到最大重发次数中断标志 
			NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY, NRF_HardStruct);//清除TX_FIFO寄存器
			// NRF_DEBUG("NRF 达到最大重发次数!errorCode = %d",NRF_ERRCODE_TX_MAX);
			return STATUS_MAX_RT;
		}
		return NRF_ERRCODE_GET_TX_STA_TIMEOUT;
	}
	else
	{

		uint32_t nowTime = NRF24L01_GetTime();
		uint32_t diffTime;
		
		NRF24L01_CE_LOW(NRF_HardStruct);//切换为待机
		NRF_W_nByte(WR_TX_PLOAD_CMD , TxBuf, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//写入数据到发送缓冲区
		NRF24L01_CE_HIGH(NRF_HardStruct);//切换为收发，等待发送


		NRF_HardStruct->NRF_G_STATE = NRF_R_Reg(STATUS, NRF_HardStruct);//读取状态寄存器
		while (!(NRF_HardStruct->NRF_G_STATE&STATUS_TX_DS))//等待发送完成
		{
			NRF_HardStruct->NRF_G_STATE = NRF_R_Reg(STATUS, NRF_HardStruct);//读取状态寄存器
			diffTime = NRF24L01_GetTime() - nowTime;
			if(diffTime > time_out) 
			{
				NRF_DEBUG("NRF 等待超时!errorCode = %d",NRF_ERRCODE_GET_TX_STA_TIMEOUT);
				break;
			}
		}
		
		NRF_HardStruct->NRF_G_STATE = NRF_R_Reg(STATUS, NRF_HardStruct);//读取状态寄存器
		if(NRF_HardStruct->NRF_G_STATE&STATUS_TX_DS)
		{
			NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY, NRF_HardStruct);//清除TX_FIFO寄存器
			NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, STATUS_TX_DS, NRF_HardStruct);//清除所有中断标志
			return 0;//发送成功
		}
		else if(NRF_HardStruct->NRF_G_STATE&STATUS_MAX_RT)//达到最大重发次数
		{
			NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY, NRF_HardStruct);//清除TX_FIFO寄存器
			NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, STATUS_MAX_RT, NRF_HardStruct);//清除所有中断标志
			NRF_DEBUG("NRF 达到最大重发次数!errorCode = %d",NRF_ERRCODE_TX_MAX);
			return STATUS_MAX_RT;
		}
		return NRF_ERRCODE_GET_TX_STA_TIMEOUT;

	}


}

/** 
* @brief   接收数据包，反复接受直至超时
* @param   RxBuf ：接收的数据 最大32Byte
* @param   time_out ：超时时间ms
* @param   NRF_HardStruct[IN/OUT] ：待接收的NRF
* @retval  0表示成功
*/
uint8_t NRF24L01_RxPacket(uint8_t* RxBuf, uint32_t time_out, NRF24L01P_Hard_Typedef* NRF_HardStruct)
{

	if (NRF_HardStruct->NRF_UseEXTI)
	{
		//使用外部中断读取NRF状态
		uint32_t nowTime = NRF24L01_GetTime();
		uint32_t diffTime;
		while(!(NRF_HardStruct->NRF_G_STATE&STATUS_RX_DR))//等待接收完成,产生中断
		{
			diffTime = NRF24L01_GetTime() - nowTime;
			if(diffTime > time_out) 
			{
				// NRF_DEBUG("NRF 等待超时!errorCode = %d",NRF_ERRCODE_GET_RX_STA_TIMEOUT);
				break;
			}
		}
		
		if(NRF_HardStruct->NRF_G_STATE&STATUS_RX_DR)
		{
			NRF_R_nByte(RD_RX_PLOAD_CMD , RxBuf, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//读取数据
			NRF_HardStruct->NRF_G_STATE&=~STATUS_RX_DR;//清除接收到有效数据标志
			NRF_W_Reg(FLUSH_RX_CMD, NRF_DUMMY, NRF_HardStruct);//清除RX_FIFO寄存器
			return 0;//接收成功
		}
		return NRF_ERRCODE_GET_RX_STA_TIMEOUT;//接收失败
	}
	else
	{
		uint32_t nowTime = NRF24L01_GetTime();
		uint32_t diffTime;
		NRF_HardStruct->NRF_G_STATE = NRF_R_Reg(STATUS, NRF_HardStruct);//读取状态寄存器
		while(!(NRF_HardStruct->NRF_G_STATE&STATUS_RX_DR))//等待接收完成,产生中断
		{
			NRF_HardStruct->NRF_G_STATE = NRF_R_Reg(STATUS, NRF_HardStruct);//读取状态寄存器
			diffTime = NRF24L01_GetTime() - nowTime;
			if(diffTime > time_out) 
			{
				NRF_DEBUG("NRF 等待超时!errorCode = %d",NRF_ERRCODE_GET_RX_STA_TIMEOUT);
				break;
			}
		}

		NRF_HardStruct->NRF_G_STATE = NRF_R_Reg(STATUS, NRF_HardStruct);//读取状态寄存器

		if(NRF_HardStruct->NRF_G_STATE&STATUS_RX_DR)
		{
			NRF_R_nByte(RD_RX_PLOAD_CMD , RxBuf, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//读取数据
			NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, STATUS_RX_DR, NRF_HardStruct);//清除所有中断标志
			NRF_W_Reg(FLUSH_RX_CMD, NRF_DUMMY, NRF_HardStruct);//清除RX_FIFO寄存器
			return 0;//接收成功
		}
		return NRF_ERRCODE_GET_RX_STA_TIMEOUT;//接收失败
	}

}


/** 
* @brief   发送数据包
* @param   TxBuf ：需要发送的数据 最大32Byte
* @param   RxBuf ：ACK携带的数据 最大32Byte
* @param   time_out ：超时时间ms
* @param   NRF_HardStruct[IN/OUT] ：待发送的NRF
* @retval  0:TxRx successful  else:failed @ref NRF24L01P_ReturnCode
*/
uint8_t NRF24L01_TxPacketWithAckData(uint8_t* TxBuf, uint8_t* RxBuf, uint32_t time_out, NRF24L01P_Hard_Typedef* NRF_HardStruct)
{
	uint8_t rVal = 0;

	if (NRF_HardStruct->NRF_UseEXTI)
	{
		//使用外部中断读取NRF状态
		uint32_t nowTime = NRF24L01_GetTime();
		uint32_t diffTime;
		
		NRF24L01_CE_LOW(NRF_HardStruct);//切换为待机
		NRF_W_nByte(WR_TX_PLOAD_CMD , TxBuf, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//写入数据到发送缓冲区
		NRF24L01_CE_HIGH(NRF_HardStruct);//切换为收发，等待发送

	
		while(!(NRF_HardStruct->NRF_G_STATE&STATUS_TX_DS))//等待发送完成,产生中断
		{
			diffTime = NRF24L01_GetTime() - nowTime;
			if(diffTime > time_out) 
			{
				// NRF_DEBUG("NRF 等待超时!errorCode = %d",NRF_ERRCODE_GET_TX_STA_TIMEOUT);
				break;
			}
		}
		
		if(NRF_HardStruct->NRF_G_STATE&STATUS_TX_DS)
		{
			NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY, NRF_HardStruct);//清除TX_FIFO寄存器
			rVal = 0;//发送成功
		}
		else if(NRF_HardStruct->NRF_G_STATE&STATUS_MAX_RT)//达到最大重发次数
		{
			NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY, NRF_HardStruct);//清除TX_FIFO寄存器
			// NRF_DEBUG("NRF 达到最大重发次数!errorCode = %d",NRF_ERRCODE_TX_MAX);
			rVal |= STATUS_MAX_RT|NRF_ERRCODE_TX_Err;
		}
		else
		{
			rVal |= NRF_ERRCODE_GET_TX_STA_TIMEOUT|NRF_ERRCODE_TX_Err;
		}
		
		if (NRF_HardStruct->NRF_G_STATE&STATUS_RX_DR)
		{
			NRF_GetACKData(RxBuf,  NRF_HardStruct);
		}
		else
		{
			rVal |= NRF_ERRCODE_RX_Err;
		}
		NRF_HardStruct->NRF_G_STATE&=~(NRF_HardStruct->NRF_G_STATE);//清除中断标志
	}
	return rVal;
}


/** 
* @brief   接收数据包，反复接受直至超时
* @param   TxBuf ：下次需要发送的数据 最大32Byte
* @param   RxBuf ：接收的数据 最大32Byte
* @param   time_out ：超时时间ms
* @param   NRF_HardStruct[IN/OUT] ：待接收的NRF
* @retval  0表示成功 @ref NRF24L01P_ReturnCode
*/
uint8_t NRF24L01_RxPacketWithAckData(uint8_t* TxBuf, uint8_t* RxBuf, uint32_t time_out, NRF24L01P_Hard_Typedef* NRF_HardStruct)
{
	uint8_t rVal = 0;
	if (NRF_HardStruct->NRF_UseEXTI)
	{
		//使用外部中断读取NRF状态
		uint32_t nowTime = NRF24L01_GetTime();
		uint32_t diffTime;
		while(!(NRF_HardStruct->NRF_G_STATE&STATUS_RX_DR))//等待接收完成,产生中断
		{
			diffTime = NRF24L01_GetTime() - nowTime;
			if(diffTime > time_out) 
			{
				// NRF_DEBUG("NRF 等待超时!errorCode = %d",NRF_ERRCODE_GET_RX_STA_TIMEOUT);
				break;
			}
		}
		
		if(NRF_HardStruct->NRF_G_STATE&STATUS_RX_DR)
		{
			NRF_W_nByte(W_ACK_PAYLOAD_P0 , TxBuf, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//写入数据
			NRF_R_nByte(RD_RX_PLOAD_CMD , RxBuf, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//读取数据
			NRF_W_Reg(FLUSH_RX_CMD, NRF_DUMMY, NRF_HardStruct);//清除RX_FIFO寄存器
			rVal = 0;//接收成功
		}
		else
		{
			rVal = NRF_ERRCODE_GET_RX_STA_TIMEOUT|NRF_ERRCODE_RX_Err;//接收失败
		}
		if (NRF_HardStruct->NRF_G_STATE&STATUS_TX_FULL)
		{
			NRF_W_Reg(FLUSH_TX_CMD, NRF_DUMMY, NRF_HardStruct);//清除TX_FIFO寄存器
		}
		
	}

	NRF_HardStruct->NRF_G_STATE&=~(NRF_HardStruct->NRF_G_STATE);//清除数据标志
	return rVal;
}


/**
  * @brief   NRF通信自检测
  * @param   NRF_HardStruct[IN]: 待检测的NRF
  * @retval  0：设备通信正常， else：通信异常
**/
uint8_t NRF24L01_Check(NRF24L01P_Hard_Typedef NRF_HardStruct)
{
	uint8_t checkbuf[] = {0x12,0x13,0x14,0x15,0x16};
	uint8_t Rcheckbuf[5] = {0,0,0,0,0};
	uint8_t i;
	NRF_W_nByte(NRF_WRITE_REG_CMD|TX_ADDR, checkbuf, 5, &NRF_HardStruct);
	NRF_R_nByte(TX_ADDR, Rcheckbuf, 5, &NRF_HardStruct);
	
	
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
		return 0;
	else 
		return 1;
}


/**
* @brief   NRF发送方测试
* @param   NRF_HardStruct[IN]: 配置的NRF
* @param   f : 通信频率  (2.4+0.001*f)/GHz f<125
* @retval  
*/
void NRF24L01_TX_TestConfig(uint8_t f, NRF24L01P_Hard_Typedef* NRF_HardStruct)
{
	NRF24L01_CE_LOW(NRF_HardStruct);//切换为待机
	
	NRF_W_Reg(NRF_WRITE_REG_CMD |SETUP_AW, Addr_Width5, NRF_HardStruct);//设置地址宽度为5字节
	NRF_W_nByte(NRF_WRITE_REG_CMD +TX_ADDR, (uint8_t*)NRFTX_TX_ADDRESS, TX_ADDRESS_WIDTH, NRF_HardStruct);//设置数据发送地址
	
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P0, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//设置接收数据管道P0有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P1, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//设置接收数据管道P1有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P2, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//设置接收数据管道P2有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P3, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//设置接收数据管道P3有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P4, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//设置接收数据管道P4有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P5, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//设置接收数据管道P5有效数据宽度
	
	
	uint8_t tmp_addr;
	tmp_addr = NRFTX_RX_ADDRESS_1[RX_ADDRESS_WIDTH-1];
	
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P0, (uint8_t*)NRFTX_RX_ADDRESS_0, RX_ADDRESS_WIDTH, NRF_HardStruct);//设置数据接收管道P0的接收地址
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P1, (uint8_t*)NRFTX_RX_ADDRESS_1, RX_ADDRESS_WIDTH, NRF_HardStruct);//设置数据接收管道P1的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P2, &tmp_addr, 1, NRF_HardStruct);//设置数据接收管道P2的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P3, &tmp_addr, 1, NRF_HardStruct);//设置数据接收管道P3的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P4, &tmp_addr, 1, NRF_HardStruct);//设置数据接收管道P4的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P5, &tmp_addr, 1, NRF_HardStruct);//设置数据接收管道P5的接收地址
	
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_AA, 0x3f, NRF_HardStruct);//使能通道012345的自动应答
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_RXADDR, 0x3f, NRF_HardStruct);//使能通道012345的接收地址
	
	NRF_W_Reg(0x75,SETUP_RETR, NRF_HardStruct);//设置自动重发间隔2000us，自动重发次数5次, xy(H)高4位为延迟时间=(x+1)*250us; 低4位为重发次数y
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_CH, f, NRF_HardStruct);//设置射频信道为2.44GHz，0x01为2.401GHz 0x02为2.402GHz...（2.4GHz-2.525GHz）
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_SETUP, 0x0f, NRF_HardStruct);//打开低噪声放大器增益，发射功率为0dBm，数据传输率2Mbps
	NRF_W_Reg(NRF_WRITE_REG_CMD |CONFIG, 0x0e, NRF_HardStruct);//打开所有中断（产生中断IRQ脚被拉低），使能16位CRC，芯片上电，设置为发送模式
	NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY, NRF_HardStruct);//清除TX_FIFO寄存器
	NRF_W_Reg(FLUSH_RX_CMD , NRF_DUMMY, NRF_HardStruct);//清除RX_FIFO寄存器
	
	
	uint8_t state;
	state = NRF_R_Reg(STATUS, NRF_HardStruct);//读取状态寄存器
	NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, state, NRF_HardStruct);//清除所有中断标志
	
	NRF24L01_CE_HIGH(NRF_HardStruct);//切换为收发
	
}


/**
* @brief   NRF接收方测试
* @param   NRF_HardStruct[IN]: 配置的NRF
* @param   f : 通信频率  (2.4+0.001*f)/GHz f<125
* @retval  
*/
void NRF24L01_RX_TestConfig(uint8_t f, NRF24L01P_Hard_Typedef* NRF_HardStruct)
{
	NRF24L01_CE_LOW(NRF_HardStruct);//切换为待机
	
	
	NRF_W_Reg(NRF_WRITE_REG_CMD |SETUP_AW, Addr_Width5, NRF_HardStruct);//设置地址宽度为5字节
	NRF_W_nByte(NRF_WRITE_REG_CMD +TX_ADDR, (uint8_t*)NRFRX_TX_ADDRESS, TX_ADDRESS_WIDTH, NRF_HardStruct);//设置数据发送地址
	
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P0, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//设置接收数据管道P0有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P1, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//设置接收数据管道P1有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P2, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//设置接收数据管道P2有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P3, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//设置接收数据管道P3有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P4, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//设置接收数据管道P4有效数据宽度
	NRF_W_Reg(NRF_WRITE_REG_CMD |RX_PW_P5, WRX_PAYLOAD_WIDTH, NRF_HardStruct);//设置接收数据管道P5有效数据宽度
	
	
	uint8_t tmp_addr;
	tmp_addr = NRFRX_RX_ADDRESS_1[RX_ADDRESS_WIDTH-1];
	
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P0, (uint8_t*)NRFRX_RX_ADDRESS_0, RX_ADDRESS_WIDTH, NRF_HardStruct);//设置数据接收管道P0的接收地址
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P1, (uint8_t*)NRFRX_RX_ADDRESS_1, RX_ADDRESS_WIDTH, NRF_HardStruct);//设置数据接收管道P1的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P2, &tmp_addr, 1, NRF_HardStruct);//设置数据接收管道P2的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P3, &tmp_addr, 1, NRF_HardStruct);//设置数据接收管道P3的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P4, &tmp_addr, 1, NRF_HardStruct);//设置数据接收管道P4的接收地址
	tmp_addr+=0x01;
	NRF_W_nByte(NRF_WRITE_REG_CMD +RX_ADDR_P5, &tmp_addr, 1, NRF_HardStruct);//设置数据接收管道P5的接收地址
	
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_AA, 0x3f, NRF_HardStruct);//使能通道012345的自动应答
	NRF_W_Reg(NRF_WRITE_REG_CMD |EN_RXADDR, 0x3f, NRF_HardStruct);//使能通道012345的接收地址
	
	NRF_W_Reg(0x75,SETUP_RETR, NRF_HardStruct);//设置自动重发间隔2000us，自动重发次数5次, xy(H)高4位为延迟时间=(x+1)*250us; 低4位为重发次数y
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_CH, f, NRF_HardStruct);//设置射频信道为2.44GHz，0x01为2.401GHz 0x02为2.402GHz...（2.4GHz-2.525GHz）
	NRF_W_Reg(NRF_WRITE_REG_CMD |RF_SETUP, 0x0f, NRF_HardStruct);//打开低噪声放大器增益，发射功率为0dBm，数据传输率2Mbps
	NRF_W_Reg(NRF_WRITE_REG_CMD |CONFIG, 0x0f, NRF_HardStruct);//打开所有中断（产生中断IRQ脚被拉低），使能16位CRC，芯片上电，设置为接收模式
	NRF_W_Reg(FLUSH_TX_CMD , NRF_DUMMY, NRF_HardStruct);//清除TX_FIFO寄存器
	NRF_W_Reg(FLUSH_RX_CMD , NRF_DUMMY, NRF_HardStruct);//清除RX_FIFO寄存器
	
	
	uint8_t state;
	state = NRF_R_Reg(STATUS, NRF_HardStruct);//读取状态寄存器
	NRF_W_Reg(NRF_WRITE_REG_CMD |STATUS, state, NRF_HardStruct);//清除所有中断标志
	
	
	NRF24L01_CE_HIGH(NRF_HardStruct);//切换为收发
	
}



/**
* @brief   NRF收发测试
* @param    NRF_HardStruct：测试对象
* @param    time_out：测试读写超时时间ms
* @retval  1 : 测试成功  0：测试失败  
**/
uint8_t NRF_TXRX_TEST(NRF24L01P_Hard_Typedef* NRF_HardStruct, uint32_t time_out)
{

	uint8_t test_Byte = 6;
	uint8_t checkbuf[WRX_PAYLOAD_WIDTH] = {0x10,0x11,0x12,0x13,0x14,0x15};
	uint8_t Rcheckbuf[WRX_PAYLOAD_WIDTH] = {0,0,0,0,0,0};
	uint32_t nowTime = NRF24L01_GetTime();
	uint32_t diffTime;
	if(NRF_HardStruct->WorkMode == NRF_Mode_Tx)
	{
		uint8_t tmp = NRF24L01_TxPacket (checkbuf, NRF_PackTimeOut, NRF_HardStruct);//发送测试数据
		if (!tmp)//发送成功
		{
			NRF24L01_TXRX_Mode_Switch( (uint8_t*)NRFTX_TX_ADDRESS,  SWFlag_Only2Rx , NRF_HardStruct );//切换为接收
			
			NRF_DEBUG( "NRF_TXMode_Tx:\n");
			for(uint8_t i=0;i<test_Byte;i++)
			{
				NRF_DEBUG( "%d, ", checkbuf[i]);
			}
			NRF_DEBUG( "\n\n");
			
			uint8_t tmp=0xff ;
			nowTime = NRF24L01_GetTime();

			while(tmp)
			{
				diffTime = NRF24L01_GetTime() - nowTime;
				tmp=NRF24L01_RxPacket( Rcheckbuf, NRF_PackTimeOut, NRF_HardStruct);//接收测试数据
				if (diffTime>time_out)
				{
					NRF_DEBUG( "NRF TXMode Rx time out\n");
					return 0;
				}
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
					NRF24L01_TXRX_Mode_Switch( (uint8_t*)NRFRX_TX_ADDRESS,  SWFlag_Only2Tx , NRF_HardStruct);//切换为发送
					return 1;
				}
			}
			else
				NRF_DEBUG( "NRF TXMode Rx Failed！\n");
		}
		else
			NRF_DEBUG( "NRF TXMode TxFailed!\n");
		return 0;
	}
	else
	{
		uint8_t tmp=0xff ;
		nowTime = NRF24L01_GetTime();
		while(tmp)
		{
			diffTime = NRF24L01_GetTime() - nowTime;
			tmp=NRF24L01_RxPacket( Rcheckbuf, NRF_PackTimeOut, NRF_HardStruct);//接收测试数据
			if (diffTime>time_out)
			{
				NRF_DEBUG( "NRF RXMode Rx time out\n");
				return 0;
			}
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
				NRF24L01_TXRX_Mode_Switch( (uint8_t*)NRFRX_TX_ADDRESS,  SWFlag_Only2Tx , NRF_HardStruct);//切换为发送

				for (uint8_t i = 0; i < test_Byte; i++)//收到的数据+1后发回去
				{
					checkbuf[i]+=1;
				}
				

				if(!NRF24L01_TxPacket(checkbuf, NRF_PackTimeOut, NRF_HardStruct))//发送测试数据成功
				{
					
					NRF_DEBUG( "NRF_RXMode_Tx:\n");
					for(uint8_t i=0;i<test_Byte;i++)
					{
						NRF_DEBUG( "%d, ", checkbuf[i]);
					}
					NRF_DEBUG( "\n\n");
					NRF24L01_TXRX_Mode_Switch( (uint8_t*)NRFTX_TX_ADDRESS,  SWFlag_Only2Rx , NRF_HardStruct );//切换为接收
					return 1;
				}
				else
				{
					NRF_DEBUG( "NRF RXMode Tx Failed!:\n");
				}
			}
		}
		return 0;
	}
	
}


/** 
* @brief   接收ACK携带的数据包
* @param   RxBuf ：接收的数据 最大32Byte
* @param   NRF_HardStruct[IN/OUT] ：待接收的NRF
* @retval  读取数据的长度
*/
uint8_t NRF_GetACKData(uint8_t* RxBuf, NRF24L01P_Hard_Typedef* NRF_HardStruct)
{
	uint8_t dataCount = NRF_R_Reg(R_RX_PL_WID, NRF_HardStruct);
	if (dataCount&&(dataCount<=WRX_PAYLOAD_MAX_WIDTH))
	{
		NRF_R_nByte(RD_RX_PLOAD_CMD , RxBuf, dataCount, NRF_HardStruct);//读取数据
		NRF_W_Reg(FLUSH_RX_CMD, NRF_DUMMY, NRF_HardStruct);//清除RX_FIFO寄存器
	}
	else
	{
		dataCount=0;
	}
	return dataCount;
}


/**
* @brief   NRF收发测试
* @param    NRF_HardStruct：测试对象
* @param    time_out：测试读写超时时间ms
* @retval  1 : 测试成功  0：测试失败  
**/
uint8_t NRF_TXRX_TEST_WithAckData(NRF24L01P_Hard_Typedef* NRF_HardStruct, uint32_t time_out)
{
	uint8_t test_Byte = 6;
	uint8_t sendbuf[WRX_PAYLOAD_WIDTH] = {0x10,0x11,0x12,0x13,0x14,0x15};
	uint8_t Ackbuf[WRX_PAYLOAD_WIDTH] = {0x01,0x02,0x03,0x04,0x05,0x06};
	uint8_t Rcheckbuf[WRX_PAYLOAD_WIDTH] = {0,0,0,0,0,0};
	
	uint8_t rpt=10;//重复次数

	if(NRF_HardStruct->WorkMode == NRF_Mode_Tx)
	{
		NRF24L01_TX_ModeConfig(NRF_DefaultFrequency, NRF_HardStruct);
		uint8_t tmp_rpt = rpt;
		while (tmp_rpt--)
		{
			uint8_t tmp = NRF24L01_TxPacketWithAckData(sendbuf, Rcheckbuf, NRF_PackTimeOut, NRF_HardStruct);//发送测试数据
			if (tmp&NRF_ERRCODE_TX_Err)
			{
				NRF_DEBUG( "NRF_TXMode_Tx err code:%d\n", tmp);
			}
			else
			{
				NRF_DEBUG( "NRF_TXMode_Tx:\n");
				for(uint8_t i=0;i<test_Byte;i++)
				{
					NRF_DEBUG( "%d, ", sendbuf[i]);
				}
				NRF_DEBUG( "\n\n");
			}
			if (tmp&NRF_ERRCODE_RX_Err)
			{
				NRF_DEBUG( "NRF_TXMode_RxACK err code:%d\n", tmp);
			}
			else
			{
				NRF_DEBUG( "NRF_TXMode_RxACK data:\n");
				for(uint8_t i=0;i<test_Byte;i++)
				{
					NRF_DEBUG( "%d, ", Rcheckbuf[i]);
				}
				NRF_DEBUG( "\n\n");
				if(Compare_Array( Ackbuf, Rcheckbuf,  test_Byte))//接收数据正确
				{
					NRF_DEBUG( "NRF TXMode TEST OK！\n");
				}
				else
				{
					NRF_DEBUG( "NRF TXMode TEST Failed！\n");
				}
			}
		
			SysTick_delay(500);
		}
		
	}
	else
	{
		NRF24L01_RX_ModeConfig(NRF_DefaultFrequency, NRF_HardStruct);
		if (NRF_HardStruct->AckPay)
		{
			NRF_W_nByte(W_ACK_PAYLOAD_P0, Ackbuf, test_Byte, NRF_HardStruct);//写入ACK回复数据到P0
		}
		while (1)
		{
			uint8_t tmp=0xff ;
			while(tmp)
			{
				tmp=NRF24L01_RxPacketWithAckData(Ackbuf ,Rcheckbuf, NRF_PackTimeOut, NRF_HardStruct);//接收测试数据
			}
			if (tmp&(NRF_ERRCODE_RX_Err|NRF_ERRCODE_GET_RX_STA_TIMEOUT))
			{
				NRF_DEBUG( "NRF_RXMode_Rx err code:%d\n", tmp);
			}
			else
			{
				NRF_DEBUG( "NRF_RXMode_Rx:\n");
				for(uint8_t i=0;i<test_Byte;i++)
				{
					NRF_DEBUG( "%d, ", Rcheckbuf[i]);
				}
				NRF_DEBUG( "\n\n");
				
				if(Compare_Array( sendbuf, Rcheckbuf,  test_Byte))//接收数据正确
				{
					NRF_DEBUG( "NRF RXMode TEST OK！\n");
				}
				else
				{
					NRF_DEBUG( "NRF RXMode Rx data err!:\n");
				}
				
			}
		}
	}

	return 0;
}


/**
* @brief   NRF查找匹配频率，发送方从2.400GHz开始至2.525GHz，逐一检测是否匹配(2.4+0.001*Freq)/GHz的接收方，NO payload with ACK
* @param   NRF_HardStruct ：测试对象
* @param   Freq : 接收方频率 (2.4+0.001*Freq)/GHz Freq<125
* @retval     
**/
void NRF_FinedFrequency( NRF24L01P_Hard_Typedef* NRF_HardStruct, uint8_t Freq)
{
	uint8_t test_Byte = 6;
	uint8_t sendbuf[WRX_PAYLOAD_WIDTH] = {0x10,0x11,0x12,0x13,0x14,0x15};
	uint8_t checkbuf[WRX_PAYLOAD_WIDTH] = {0x11,0x12,0x13,0x14,0x15,0x16};
	uint8_t Rcheckbuf[WRX_PAYLOAD_WIDTH] = {0,0,0,0,0,0};
	
	uint8_t rpt=3;//重复次数
	uint32_t nowTime = NRF24L01_GetTime();
	uint32_t diffTime;
	if(NRF_HardStruct->WorkMode == NRF_Mode_Tx)
	{
		for (uint8_t f = 0; f < 125; f++)
		{
			NRF_DEBUG( "NRF_TXMode_TxF:%fGHz\n", 0.001*f+2.4);
			uint8_t tmp_rpt = rpt;
			while (tmp_rpt--)
			{
				NRF24L01_TX_ModeConfig(f, NRF_HardStruct);
				uint8_t tmp = NRF24L01_TxPacket(sendbuf, NRF_PackTimeOut, NRF_HardStruct);//发送测试数据
				if (!tmp)//发送成功
				{
					NRF24L01_TXRX_Mode_Switch( (uint8_t*)NRFTX_TX_ADDRESS,  SWFlag_Only2Rx , NRF_HardStruct );//切换为接收
					if (NRF_HardStruct->AckPay)
					{
						NRF_W_nByte(W_ACK_PAYLOAD_P0, checkbuf, test_Byte, NRF_HardStruct);//写入ACK回复数据到P0
					}

					NRF_DEBUG( "NRF_TXMode_Tx:\n");
					for(uint8_t i=0;i<test_Byte;i++)
					{
						NRF_DEBUG( "%d, ", sendbuf[i]);
					}
					NRF_DEBUG( "\n\n");

					uint8_t tmp=0xff ;
					nowTime = NRF24L01_GetTime();
					while(tmp)
					{
						diffTime = NRF24L01_GetTime() - nowTime;
						if(diffTime > NRF_PackTimeOut*20) break;
						tmp=NRF24L01_RxPacket( Rcheckbuf, NRF_PackTimeOut, NRF_HardStruct);//接收测试数据
					}
					
					if(!tmp)//接收测试数据
					{
						//收到的数据是发出的+1
						NRF_DEBUG( "NRF_TXMode_Rx:\n");
						for(uint8_t i=0;i<test_Byte;i++)
						{
							NRF_DEBUG( "%d, ", Rcheckbuf[i]);
						}
						NRF_DEBUG( "\n\n");
						
						if(Compare_Array( checkbuf, Rcheckbuf,  test_Byte))//接收数据正确
						{
							NRF_DEBUG( "NRF TXMode TEST OK！\n");
						}
						else
						{
							NRF_DEBUG( "NRF TXMode TEST Failed！\n");
						}
						
					}
					else
						NRF_DEBUG( "NRF TXMode Rx Failed！\n");
				}
				else
					NRF_DEBUG( "NRF TXMode TxFailed!\n");

				SysTick_delay(500);
			}
		
		}
	}
	else
	{
		while (1)
		{
			NRF24L01_RX_ModeConfig(Freq, NRF_HardStruct);
			// if (NRF_HardStruct->AckPay)
			// {
			// 	NRF_W_nByte(W_ACK_PAYLOAD_P0, checkbuf, test_Byte, NRF_HardStruct);//写入ACK回复数据到P0
			// }
			uint8_t tmp=0xff ;
			while(tmp)
			{
				tmp=NRF24L01_RxPacket( Rcheckbuf, NRF_PackTimeOut, NRF_HardStruct);//接收测试数据
			}
			if (!tmp)//接收成功
			{
				NRF_DEBUG( "NRF_RXMode_Rx:\n");
				for(uint8_t i=0;i<test_Byte;i++)
				{
					NRF_DEBUG( "%d, ", Rcheckbuf[i]);
				}
				NRF_DEBUG( "\n\n");
				
				if(Compare_Array( sendbuf, Rcheckbuf,  test_Byte))//接收数据正确
				{
					NRF24L01_TXRX_Mode_Switch( (uint8_t*)NRFRX_TX_ADDRESS,  SWFlag_Only2Tx ,NRF_HardStruct );//切换为发送

					for (uint8_t i = 0; i < test_Byte; i++)//收到的数据+1后发回去
					{
						Rcheckbuf[i]+=1;
					}
					
					if(!NRF24L01_TxPacket (Rcheckbuf, NRF_PackTimeOut, NRF_HardStruct))//发送测试数据成功
					{
						NRF_DEBUG( "NRF_RXMode_Tx:\n");
						for(uint8_t i=0;i<test_Byte;i++)
						{
							NRF_DEBUG( "%d, ", Rcheckbuf[i]);
						}
						NRF_DEBUG( "\n");
					}
					else
						NRF_DEBUG( "NRF RXMode Tx Failed!:\n");

				}
				else
				{
					NRF_DEBUG( "NRF RXMode Rx data err!:\n");
				}
				
			}
			else
			{
				NRF_DEBUG( "NRF RXMode Rx Failed!:\n");
			}
			SysTick_delay(500);
		}
		
	}
	return ;

}



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


