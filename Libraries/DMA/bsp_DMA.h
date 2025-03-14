#ifndef _BSP_DMA_H
#define _BSP_DMA_H

#include "stm32f10x.h"



/*         注：DMA2控制器及相关请求仅存在于大容量产品和互联型产品中。        */


void DMA_M2M_Init(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t DMA_PeripheralBaseAddr, 
				  uint32_t DMA_MemoryBaseAddr, uint32_t DMA_DIR, uint32_t DMA_BufferSize, 
				  uint32_t DMA_PeripheralDataSize, uint32_t DMA_PeripheralInc, uint32_t DMA_MemoryInc);


void DMA_USART_Init(USART_TypeDef *USARTx, uint16_t USART_DMAReq, uint32_t DMA_MemoryBaseAddr, 
	                uint32_t DMA_DIR, uint32_t DMA_BufferSize, uint32_t DMA_MemoryInc);

void DMAy_Channelx_Config(DMA_Channel_TypeDef* DMAy_Channelx, DMA_InitTypeDef DMA_InitStruct);

uint8_t 	BufferCMP			(const uint32_t *SentBuffer, uint32_t *ReBuffer,uint16_t BSize);
#endif /*_BSP_DMA_H*/
