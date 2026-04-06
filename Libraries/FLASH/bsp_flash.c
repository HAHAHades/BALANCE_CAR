#include "bsp_flash.h"


uint8_t BSP_FLASH_EraseBuff[FLASH_PAGE_SIZE];//Flash 擦写缓冲区;
//不建议将Flash 擦写缓冲区放在高地址，链接器会将整个从 RAM 起点到数组末尾的地址空间全部视为 ZI 区域，这样会浪费很多RAM


#if BSP_FLASH_TEST_ONLY // 仅测试FLASH
#include "bsp_key.h"

/**
  * @brief   Flash测试程序
  * @param   
  * @retval  
**/
void FlashTest(void)
{
	uint8_t w8data[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
	uint8_t *r8data;
	uint16_t w16data[5] = {0x12, 0x23, 0x34, 0x45, 0x56};
	uint16_t *r16data;
	uint32_t w32data[5] = {0x21, 0x32, 0x43, 0x54, 0x65};
	uint32_t *r32data;
	uint8_t len = 5;
	//uint32_t addr = FLASH_PAGE_BASE_ADDR(29);//第29页基地址
	
	KEY_Register(1,GPIOA, GPIO_Pin_4);
	KEY_Register(2,GPIOA, GPIO_Pin_5);
	KEY_Register(3,GPIOA, GPIO_Pin_6);

	FlashErasePage(29,3);//从第29页开始擦除3页

	while (1)
	{

		if (KEY_Scan(1))
		{
			/* 按键按下 */
			FlashErasePage(30,1);
			FlashWriteHalfWords(FLASH_PAGE_BASE_ADDR(30), w16data, len);//写入数据
			FlashReadHalfWords(FLASH_PAGE_BASE_ADDR(30), &r16data, len);//读取数据
			if (CompareHalfWordList(w16data, r16data, len))
			{
				/* 数据相同 */
				BSP_FLASH_DEBUG("Flash Half Words test ok!\n");
			}
			else
			{
				BSP_FLASH_DEBUG("Flash Half Words test err!\n");
				for (uint8_t i = 0; i < len; i++)
				{
					printf("r16data[%d]:%d, ", i, r16data[i]);
				}
				printf("\n");
			}
			

		}
		if (KEY_Scan(2))
		{
			/* 按键按下 */
			FlashErasePage(31,1);
			FlashWriteWords(FLASH_PAGE_BASE_ADDR(31), w32data, len);//写入数据
			FlashReadWords(FLASH_PAGE_BASE_ADDR(31), &r32data, len);//读取数据
			if (CompareWordList(w32data, r32data, len))
			{
				/* 数据相同 */
				BSP_FLASH_DEBUG("Flash Words test ok!\n");
			}
			else
			{
				BSP_FLASH_DEBUG("Flash Words test err!\n");
				for (uint8_t i = 0; i < len; i++)
				{
					printf("r32data[%d]:%d, ", i, r32data[i]);
				}
				printf("\n");
			}
		
		}
		if (KEY_Scan(3))
		{
			/* 按键按下 */
			FlashErasePage(29,1);
			FlashWriteBytes(FLASH_PAGE_BASE_ADDR(29), w8data, len);//写入数据
			FlashReadBytes(FLASH_PAGE_BASE_ADDR(29), &r8data, len);//读取数据
			if (CompareByteList(w8data, r8data, len))
			{
				/* 数据相同 */
				BSP_FLASH_DEBUG("Flash Bytes test ok!\n");
			}
			else
			{
				BSP_FLASH_DEBUG("Flash Bytes test err!\n");
				for (uint8_t i = 0; i < len; i++)
				{
					printf("r8data[%d]:%d, ", i, r8data[i]);
				}
				printf("\n");
			}
		
		}

	}

	// return ;
}

#endif // BSP_FLASH_TEST_ONLY


/**
  * @brief   Flash写入字(32bit)
  * @note	 仅写入无擦除操作
  * @param   addr: 写入起始地址
  * @param   wdata: 要写入的32bit数据
  * @param   len: 写入数据的长度
  * @retval  FLASH_ERROR_PG, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
**/
FLASH_Status FlashWriteWords(uint32_t addr, uint32_t *wdata, uint32_t len)
{
	assert_param(IS_FLASH_ADDRESS(addr));
	FLASH_Unlock();
	FLASH_Status status = FLASH_COMPLETE;
	status = FLASH_WaitForLastOperation(ProgramTimeout);
	FLASH->CR |= CR_PG_Set;//使能编程位
	for (uint16_t i = 0; i < len; i++)
	{
		if(status == FLASH_COMPLETE)
		{
			/* 开始写入低16位 */
			*(__IO uint16_t*)addr = (uint16_t)wdata[i];
			status = FLASH_WaitForLastOperation(ProgramTimeout);
			if(status == FLASH_COMPLETE)
			{
				/* 写入高16位 */
				addr+=2;
				*(__IO uint16_t*)addr = wdata[i]>>16;
				status = FLASH_WaitForLastOperation(ProgramTimeout);
				addr+=2;
			}
			else
			{
				/* 超时 出错 提前退出 */
				break;
			}
		}
		else
		{
			/* 超时 出错 提前退出 */
			break;
		}
	}
	FLASH->CR &= CR_PG_Reset;//失能编程位

	FLASH_Lock();
	return status;
}


/**
  * @brief   Flash写入半字(16bit)
  * @note	 仅写入无擦除操作
  * @param   addr: 写入起始地址
  * @param   wdata: 要写入的16bit数据
  * @param   len: 写入数据的长度
  * @retval  FLASH_ERROR_PG, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
**/
FLASH_Status FlashWriteHalfWords(uint32_t addr, uint16_t *wdata, uint32_t len)
{
	assert_param(IS_FLASH_ADDRESS(addr));
	FLASH_Unlock();
	FLASH_Status status = FLASH_COMPLETE;
	status = FLASH_WaitForLastOperation(ProgramTimeout);
	FLASH->CR |= CR_PG_Set;//使能编程位
	for (uint16_t i = 0; i < len; i++)
	{
		if(status == FLASH_COMPLETE)
		{
			/* 开始写入 */
			*(__IO uint16_t*)addr = wdata[i];
			status = FLASH_WaitForLastOperation(ProgramTimeout);
			addr+=2;
		}
		else
		{
			/* 超时 出错 提前退出 */
			break;
		}
	}
	FLASH->CR &= CR_PG_Reset;//失能编程位

	FLASH_Lock();
	return status;
}


/**
  * @brief   Flash写入字节(8bit)
  * @note	 仅写入无擦除操作
  * @param   addr: 写入起始地址
  * @param   wdata: 要写入的8bit数据
  * @param   len: 写入数据的长度
  * @retval  FLASH_ERROR_PG, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
**/
FLASH_Status FlashWriteBytes(uint32_t addr, uint8_t *wdata, uint32_t len)
{
	assert_param(IS_FLASH_ADDRESS(addr));
	uint16_t* w16data = (uint16_t*)wdata;
	uint32_t len16 = len/2;
	uint8_t len_remainder = len%2;
	uint16_t tmpData16;
	FLASH_Unlock();
	FLASH_Status status = FLASH_COMPLETE;
	status = FLASH_WaitForLastOperation(ProgramTimeout);
	FLASH->CR |= CR_PG_Set;//使能编程位
	for (uint32_t i = 0; i < len16; i++)
	{
		if(status == FLASH_COMPLETE)
		{
			/* 开始写入 */
			*(__IO uint16_t*)addr = w16data[i];
			status = FLASH_WaitForLastOperation(ProgramTimeout);
			addr+=2;
		}
		else
		{
			/* 超时 出错 提前退出 */
			break;
		}
	}
	if (len_remainder)
	{
		tmpData16 = (((uint16_t)wdata[len-1])) | (uint16_t)0xff00;
		if(status == FLASH_COMPLETE)
		{
			/* 开始写入 */
			*(__IO uint16_t*)addr = tmpData16;
			status = FLASH_WaitForLastOperation(ProgramTimeout);
		
		}
	}
	
	FLASH->CR &= CR_PG_Reset;//失能编程位

	FLASH_Lock();
	return status;
}


/**
  * @brief   Flash读取字(32bit)
  * @param   addr: 读取起始地址
  * @param   rdata: 读取的32bit数据
  * @param   len: 读取数据的长度
  * @retval  
  * @note	 返回的是Flash的地址
**/
void FlashReadWords(uint32_t addr, uint32_t **rdata, uint16_t len)
{
	*rdata = (uint32_t*)addr;

	return;
}


/**
  * @brief   Flash读取半字(16bit)
  * @param   addr: 读取起始地址
  * @param   rdata: 读取的16bit数据
  * @param   len: 读取数据的长度
  * @retval  
  * @note	 返回的是Flash的地址
**/
void FlashReadHalfWords(uint32_t addr, uint16_t **rdata, uint16_t len)
{
	*rdata = (uint16_t*)addr;

	return;
}


/**
  * @brief   Flash读取字节(8bit)
  * @param   addr: 读取起始地址
  * @param   rdata: 读取的8bit数据
  * @param   len: 读取数据的长度
  * @retval  
  * @note	 返回的是Flash的地址
**/
void FlashReadBytes(uint32_t addr, uint8_t **rdata, uint16_t len)
{
	*rdata = (uint8_t*)addr;

	return;
}


/**
  * @brief   Flash读取字节(8bit)
  * @param   addr: 读取起始地址
  * @param   rdata: 读取的8bit数据
  * @param   len: 读取数据的长度
  * @retval  
  * @note	 返回Flash数据到指定SRAM地址
**/
void FlashReadBytes2SRAM(uint32_t addr, uint8_t *rdata, uint16_t len)
{
	uint8_t * flashAddr = (uint8_t *)addr;
	for (uint16_t i = 0; i < len; i++)
	{
		/* code */
		rdata[i] = flashAddr[i];
	}
	
	return;
}

/**
  * @brief   Flash擦除页
  * @param   page: 从第几页开始擦除
  * @param   pagenum: 要擦除多少页
  * @retval  FLASH_ERROR_PG, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
**/
FLASH_Status FlashErasePage(uint16_t page, uint16_t pagenum)
{
	FLASH_Status status;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR|FLASH_FLAG_EOP);
	for (uint16_t i = 0; i < pagenum; i++)
	{
		status = FLASH_ErasePage(FLASH_PAGE_BASE_ADDR(page+i));
		if (status!=FLASH_COMPLETE)
		{
			break;
		}
	}
	FLASH_Lock();
	return status;
}


/**
  * @brief   Flash擦除片区,无保留
  * @param   startAddr: 起始地址
  * @param   EndAddr: 结束地址(可访问的最后地址+1)
  * @retval  FLASH_ERROR_PG, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
**/
FLASH_Status FlashEraseSection(uint32_t startAddr, uint32_t EndAddr)
{
	FLASH_Status status = FLASH_ERROR_PG;

	uint16_t sPage;
	uint16_t ePage;
	uint16_t ePageRemainder;
	if ((startAddr>=FLASH_START_ADDR)&&(EndAddr<=FLASH_END_ADDR))
	{
		/* 地址合法 */
		sPage = (startAddr-FLASH_START_ADDR)/FLASH_PAGE_SIZE;
		ePage = (EndAddr-FLASH_START_ADDR)/FLASH_PAGE_SIZE;
		ePageRemainder = (EndAddr-FLASH_START_ADDR)%FLASH_PAGE_SIZE;
		if (ePageRemainder!=0)
		{
			ePage++;
		}
		status = FlashErasePage(sPage,  ePage-sPage);
	}
	
	return status;
}


/**
  * @brief   Flash擦除片区，保留无需擦除的部分
  * @param   startAddr: 起始地址
  * @param   EndAddr: 结束地址(可访问的最后地址+1)
  * @retval  FLASH_ERROR_PG, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
**/
FLASH_Status FlashEraseSegment(uint32_t startAddr, uint32_t EndAddr)
{
	FLASH_Status status = FLASH_COMPLETE;
	uint16_t sPage;
	uint16_t ePage;
	uint16_t PageNUM;
	uint16_t ePageRemainder;
	if ((startAddr>=FLASH_START_ADDR)&&(EndAddr<=FLASH_END_ADDR))
	{
		/* 地址合法 */
		sPage = (startAddr-FLASH_START_ADDR)/FLASH_PAGE_SIZE;
		ePage = (EndAddr-FLASH_START_ADDR)/FLASH_PAGE_SIZE;
		ePageRemainder = (EndAddr-FLASH_START_ADDR)%FLASH_PAGE_SIZE;

		PageNUM = ePage-sPage;
		if (PageNUM > 0)
		{
			/* 不止1页 */
			if (startAddr>FLASH_PAGE_BASE_ADDR(sPage))
			{
				/* 首页有要保存的数据 */
				uint16_t sIndex = startAddr-FLASH_PAGE_BASE_ADDR(sPage);
				FlashReadBytes2SRAM(FLASH_PAGE_BASE_ADDR(sPage), BSP_FLASH_EraseBuff, FLASH_PAGE_SIZE);
				for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++)
				{
					if (i>=sIndex)
					{
						/* 需要擦除的部分 */
						BSP_FLASH_EraseBuff[i] = 0xff;
					}
				}
				status = FlashErasePage( sPage,  1);//擦除整页
				if (status==FLASH_COMPLETE)
				{
					/* 写入保存的数据 */
					status = FlashWriteBytes(FLASH_PAGE_BASE_ADDR(sPage), BSP_FLASH_EraseBuff,  FLASH_PAGE_SIZE);
				}
			}
			if (ePageRemainder!=0)
			{
				/* 尾页有要保存的数据 */
				if (status==FLASH_COMPLETE)//首页操作成功/未操作
				{
					uint16_t eIndex = EndAddr-FLASH_PAGE_BASE_ADDR(ePage);
					FlashReadBytes2SRAM(FLASH_PAGE_BASE_ADDR(ePage), BSP_FLASH_EraseBuff, FLASH_PAGE_SIZE);
					for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++)
					{
						if (i<eIndex)
						{
							/* 需要擦除的部分 */
							BSP_FLASH_EraseBuff[i] = 0xff;
						}
					}
					status = FlashErasePage( ePage,  1);//擦除整页
					if (status==FLASH_COMPLETE)
					{
						/* 写入保存的数据 */
						status = FlashWriteBytes(FLASH_PAGE_BASE_ADDR(ePage), BSP_FLASH_EraseBuff,  FLASH_PAGE_SIZE);
					}
				}
			}
			if (status==FLASH_COMPLETE)
			{
				/* 首尾页操作成功/未操作 */
				if (ePageRemainder!=0)
				{
					PageNUM++;
				}
				if ((PageNUM-2)>0)
				{
					/* 除了起止页外还有需要擦除的页 */
					status = FlashEraseSection(FLASH_PAGE_BASE_ADDR(sPage+1), FLASH_PAGE_BASE_ADDR(ePage));
				}
			}
		}
		else
		{
			/* 只有1页 */
			uint16_t sIndex = startAddr-FLASH_PAGE_BASE_ADDR(sPage);
			uint16_t eIndex = EndAddr-FLASH_PAGE_BASE_ADDR(sPage);
			FlashReadBytes2SRAM(FLASH_PAGE_BASE_ADDR(sPage), BSP_FLASH_EraseBuff, FLASH_PAGE_SIZE);
			for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++)
			{
				if ((i<eIndex)||(i>=sIndex))
				{
					/* 需要擦除的部分 */
					BSP_FLASH_EraseBuff[i] = 0xff;
				}
			}
			status = FlashErasePage( sPage,  1);//擦除整页
			if (status==FLASH_COMPLETE)
			{
				/* 写入保存的数据 */
				status = FlashWriteBytes(FLASH_PAGE_BASE_ADDR(sPage), BSP_FLASH_EraseBuff,  FLASH_PAGE_SIZE);
			}
			
		}
		
	}

	return	status;
}

/**
  * @brief   比较两个数组
  * @param   arr1: 数组1
  * @param   arr2: 数组2
  * @param   arr2: 数组长度
  * @retval  1：数组相同 0：数组不同
**/
uint8_t CompareByteList(uint8_t *arr1, uint8_t *arr2, uint16_t len)
{
	uint8_t ret = 1;

	for (uint16_t i = 0; i < len; i++)
	{
		if (arr1[i]!=arr2[i])
		{
			ret = 0;
			break;
		}
		
	}
	
	return ret;
}


/**
  * @brief   比较两个数组
  * @param   arr1: 数组1
  * @param   arr2: 数组2
  * @param   len: 数组长度
  * @retval  1：数组相同 0：数组不同
**/
uint8_t CompareHalfWordList(uint16_t *arr1, uint16_t *arr2, uint16_t len)
{
	uint8_t ret = 1;

	for (uint16_t i = 0; i < len; i++)
	{
		if (arr1[i]!=arr2[i])
		{
			ret = 0;
			break;
		}
		
	}
	
	return ret;
}


/**
  * @brief   比较两个数组
  * @param   arr1: 数组1
  * @param   arr2: 数组2
  * @param   len: 数组长度
  * @retval  1：数组相同 0：数组不同
**/
uint8_t CompareWordList(uint32_t *arr1, uint32_t *arr2, uint16_t len)
{
	uint8_t ret = 1;

	for (uint16_t i = 0; i < len; i++)
	{
		if (arr1[i]!=arr2[i])
		{
			ret = 0;
			break;
		}
		
	}
	
	return ret;
}




