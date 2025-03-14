#include "bsp_can.h"//板级支持包board support package\
                      仅支持某一个或一批同样的开发板
					  
					  



/**
  * @brief   初始化CAN
  * @param   
  *		@arg CANx ：需要配置的CAN
  *		@arg CAN_Mode ：CAN_Mode_Normal/CAN_Mode_LoopBack/CAN_Mode_Silent/CAN_Mode_Silent_LoopBack
注：C8T6只有CAN1
  * @retval  
  */

void CAN_Config(CAN_TypeDef* CANx, uint8_t CAN_Mode)
{

	GPIO_TypeDef* CANx_Tx_GPIO_PORTx;
	GPIO_TypeDef* CANx_Rx_GPIO_PORTx;
	
	uint16_t CANx_Tx_GPIO_PINx;
	uint16_t CANx_Rx_GPIO_PINx;
	
	if(CANx==CAN1)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);//打开时钟
		if(CAN1_GPIO_Remapping==0x00)
		{
			CANx_Tx_GPIO_PORTx = GPIOA;
			CANx_Rx_GPIO_PORTx = GPIOA;
			CANx_Rx_GPIO_PINx = GPIO_Pin_11;
			CANx_Tx_GPIO_PINx = GPIO_Pin_12;
		}
		else if(CAN1_GPIO_Remapping==0x10)
		{
			CANx_Tx_GPIO_PORTx = GPIOB;
			CANx_Rx_GPIO_PORTx = GPIOB;
			CANx_Rx_GPIO_PINx = GPIO_Pin_8;
			CANx_Tx_GPIO_PINx = GPIO_Pin_9;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//打开时钟
			GPIO_PinRemapConfig( GPIO_Remap1_CAN1,  ENABLE);
		}
		else
			return;
	}
	else
		return;
	
	uint32_t CANx_Tx_RCC_APB2Periph_GPIOx  = RCC_APB2Periph_GPIOA << \
											(((uint32_t)CANx_Tx_GPIO_PORTx - (uint32_t)GPIOA) / 0x0400 );
	uint32_t CANx_Rx_RCC_APB2Periph_GPIOx  = RCC_APB2Periph_GPIOA << \
											(((uint32_t)CANx_Rx_GPIO_PORTx - (uint32_t)GPIOA) / 0x0400 );
	
	/***********************GPIO Config**********************/
	
	RCC_APB2PeriphClockCmd(CANx_Tx_RCC_APB2Periph_GPIOx|CANx_Rx_RCC_APB2Periph_GPIOx, ENABLE);//打开时钟
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//CAN Tx
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = CANx_Tx_GPIO_PINx;
	GPIO_Init(CANx_Tx_GPIO_PORTx, &GPIO_InitStruct);
	
	//CAN Rx
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = CANx_Rx_GPIO_PINx;
	GPIO_Init(CANx_Rx_GPIO_PORTx, &GPIO_InitStruct);
	
	/********************************************************/
	
	

	
	
	/**********************CAN Config************************/
	CAN_InitTypeDef CAN_InitStruct;
	
	CAN_InitStruct.CAN_Mode = CAN_Mode;
	CAN_InitStruct.CAN_Prescaler = 4;//波特率分频器，设置Tq=(CAN_Prescaler+1)*tPCLK(APB1)
	CAN_InitStruct.CAN_BS1 = CAN_BS1_4tq;
	CAN_InitStruct.CAN_BS2 = CAN_BS2_4tq;
	CAN_InitStruct.CAN_SJW = CAN_SJW_2tq;//重新同步跳跃宽度
	CAN_InitStruct.CAN_ABOM = ENABLE;//是否自动离线
	CAN_InitStruct.CAN_AWUM = ENABLE;//是否自动唤醒
	CAN_InitStruct.CAN_NART = DISABLE;//是否禁止自动重传
	CAN_InitStruct.CAN_RFLM = ENABLE;//是否锁定接收FIFO，锁定：FIFO满后忽略新消息，不锁定：新消息覆盖旧消息
	CAN_InitStruct.CAN_TTCM = DISABLE;//时间触发通信
	CAN_InitStruct.CAN_TXFP = DISABLE;//消息发送按照存入FIFO顺序(ENABLE)，按照标识符优先级(DISABLE)
	
	CAN_Init( CANx, &CAN_InitStruct);
	
	/*******************************************************/
	
	
	/******************CAN Filter Config********************/
	CAN_FilterInitTypeDef CAN_FilterInitStruct;
	
	CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;//使能过滤器
	CAN_FilterInitStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//使用FIFO0储存过滤到的报文
	CAN_FilterInitStruct.CAN_FilterNumber = 0;
	CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;
	/*注：ID和Mask的前三位默认为0 IDE RTR
	其中 IDE为ID扩展标识符，为1表示使用扩展标识符为0表示使用标准标识符
	RTR为远程数据请求标志，为1表示该报文向远端请求对应ID的数据，为0表示该报文发送对应ID的数据给远端
	*/
	CAN_FilterInitStruct.CAN_FilterIdHigh = (((uint32_t)CAN_PASS_ID<<3)|CAN_ID_EXT|CAN_RTR_DATA)>>16;//过滤报文为 扩展ID，且向远端发出的数据
	CAN_FilterInitStruct.CAN_FilterIdLow = (((uint32_t)CAN_PASS_ID<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;
	CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0xFFFF;//ID完全匹配，等同于标识符列表
	CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0xFFFF;
	
	CAN_FilterInit( &CAN_FilterInitStruct);
	/*******************************************************/


	CAN_ITConfig(CANx,  CAN_IT_FMP0,  ENABLE);//使能FIFO0的接收中断

	/******************NVIC Config**************************/

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//选择中断优先级分组
	NVIC_InitTypeDef NVIC_InitStruct;

	NVIC_InitStruct.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStruct);

	/*******************************************************/


}	



