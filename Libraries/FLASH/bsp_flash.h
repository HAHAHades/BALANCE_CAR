#ifndef _BSP_FLASH_H_
#define _BSP_FLASH_H_


#include "stm32f10x.h"
#include "bsp_usart.h"


#define STM32F103C8Tx //使用的芯片
#define BSP_FLASH_DEBUG_ON         0 //是否输出调试信息
#define BSP_FLASH_TEST_ONLY  0 //是否仅测试FLASH



#ifdef STM32F103C6Tx
#define FLASH_PAGE_SIZE 1024 //FLASH每页大小/Bytes
#define FALSH_MAX_SIZE 0x8000 //FLASH大小/32kBytes
#define SRAM_MAX_SIZE  0x2800 //SRAM大小/10kBytes
#endif //F103C8Tx

#ifdef STM32F103C8Tx
#define FLASH_PAGE_SIZE 1024 //FLASH每页大小/Bytes
#define FALSH_MAX_SIZE 0x10000 //FLASH大小/64kBytes
#define SRAM_MAX_SIZE  0x5000 //SRAM大小/20kBytes
#endif //F103C8Tx


/*flash第n页基地址*/
#define FLASH_PAGE_BASE_ADDR(n) (uint32_t)(0x08000000+(FLASH_PAGE_SIZE*(n))) 

#define FLASH_START_ADDR FLASH_PAGE_BASE_ADDR(0) //FLASH起始地址
#define FLASH_END_ADDR (FLASH_PAGE_BASE_ADDR(0)+FALSH_MAX_SIZE) //FLASH结束地址(可访问的最后地址+1)

#define SRAM_START_ADDR ((uint32_t)0x20000000) //SRAM起始地址
#define SRAM_END_ADDR ((uint32_t)(SRAM_START_ADDR+SRAM_MAX_SIZE)) //SRAM结束地址(可访问的最后地址+1)



/*信息输出*/
#define BSP_FLASH_INFO(fmt,arg...)          do{\
										if(BSP_FLASH_DEBUG_ON)\
										printf("<<-FLASH-INFO->> "fmt"\n",##arg);\
										}while(0)
#define BSP_FLASH_ERROR(fmt,arg...)          printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
#define BSP_FLASH_DEBUG(fmt,arg...)          do{\
                                          if(BSP_FLASH_DEBUG_ON)\
                                          printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

/****************copy from stm32f10x_flash.c******************/
/* Flash Control Register bits */
#define ProgramTimeout        ((uint32_t)0x00002000)
#define EraseTimeout          ((uint32_t)0x000B0000)
#define CR_PG_Set                ((uint32_t)0x00000001)
#define CR_PG_Reset              ((uint32_t)0x00001FFE) 
#define CR_PER_Set               ((uint32_t)0x00000002)
#define CR_PER_Reset             ((uint32_t)0x00001FFD)
#define CR_MER_Set               ((uint32_t)0x00000004)
#define CR_MER_Reset             ((uint32_t)0x00001FFB)
#define CR_OPTPG_Set             ((uint32_t)0x00000010)
#define CR_OPTPG_Reset           ((uint32_t)0x00001FEF)
#define CR_OPTER_Set             ((uint32_t)0x00000020)
#define CR_OPTER_Reset           ((uint32_t)0x00001FDF)
#define CR_STRT_Set              ((uint32_t)0x00000040)
#define CR_LOCK_Set              ((uint32_t)0x00000080)
/*************************************************************/

										  
		
/****************************************函数声明***************************************/


void FlashTest(void);
FLASH_Status FlashWriteWords(uint32_t addr, uint32_t *wdata, uint32_t len);
FLASH_Status FlashWriteHalfWords(uint32_t addr, uint16_t *wdata, uint32_t len);
FLASH_Status FlashWriteBytes(uint32_t addr, uint8_t *wdata, uint32_t len);
void FlashReadWords(uint32_t addr, uint32_t **rdata, uint16_t len);
void FlashReadHalfWords(uint32_t addr, uint16_t **rdata, uint16_t len);
void FlashReadBytes(uint32_t addr, uint8_t **rdata, uint16_t len);
void FlashReadBytes2SRAM(uint32_t addr, uint8_t *rdata, uint16_t len);
FLASH_Status FlashErasePage(uint16_t page, uint16_t pagenum);
FLASH_Status FlashEraseSection(uint32_t startAddr, uint32_t EndAddr);
FLASH_Status FlashEraseSegment(uint32_t startAddr, uint32_t EndAddr);
uint8_t CompareWordList(uint32_t *arr1, uint32_t *arr2, uint16_t len);
uint8_t CompareHalfWordList(uint16_t *arr1, uint16_t *arr2, uint16_t len);
uint8_t CompareByteList(uint8_t *arr1, uint8_t *arr2, uint16_t len);

#endif //_BSP_FLASH_H_

