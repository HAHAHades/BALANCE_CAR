#include "bsp_144spi_tft_lcd.h"



void LCD_Test(void)
{
	
	
	//double a,b;
	
	
//	char str[] = "0123456789\nABCDEF\n\n6789aB\ngMG?-_%$!";
//	Display_8x16_ASCII(0,0,RED,WHITE,str);
	
	uint16_t RdataBuf[]={0x05,0x06,0x07,0x08};
	uint16_t ColorBuf[]={RED,GREEN,BLUE,WHITE,BLACK,YELLOW,GRAY0,GRAY1,GRAY2};
	uint8_t i,j,k;
	
	//LCD_ReadData32bit(0x04,RdataBuf);
	/*
	04: 0011 1110   0100 0100     1111 1000
	da: 0111 1100 //1000 1001     1111 0000
	db:           //1000 1001
	dc:                         //1111 0000
	*/
//	for(i=0; i<2; i++)
//	{
//		Display_8x16_NUM(0+i*40,80,RED,WHITE,*(RdataBuf+i));
//	}
//	for(i=2; i<4; i++)
//	{
//		Display_8x16_NUM(0+(i-2)*40,100,RED,WHITE,*(RdataBuf+i));
//	}
//	*(RdataBuf) = LCD_ReadData8bit( 0xda);
//	Display_8x16_NUM(70,40,RED,WHITE,*(RdataBuf));

	LCD_ClearGlass(GREEN);
	for(i=0; i<8; i++)
	{
		LCD_DispDot(10+i, 10,*(ColorBuf+i));
		LCD_GetDotRGB(10+i,10,RdataBuf);
		for(j=0; j<4; j++)
		{
			Display_8x16_NUM(0+32*j,0+16*i,RED,WHITE,*(RdataBuf+j));
		}
	}
	SisTic_Delay_ms(500);
	SisTic_Delay_ms(2500);
	for(k=0; k<9; k++)
	{
		for(i=0; i<40; i++)
		{
			for(j=0; j<40; j++)
				LCD_DispDot(i+(k%3)*40, j+(k/3)*40,*(ColorBuf+k));
			
			
		}
	}
	
}	



void LCD_Init(void)
{
	
	LCD_GPIO_Init();
	LCD_BL_HIGH();
	LCD_Reset();
	
//	/*4k 4-4-4 mode */
//	LCD_WriteCMD(0x3A);  
//	LCD_WriteData_8bit(0x03);
//	
	/*65k 5-6-5 mode */
	LCD_WriteCMD(0x3A);  
	LCD_WriteData_8bit(0x05); 
	
	/*262k 6-6-6 mode */
//	LCD_WriteCMD(0x3A);  
//	LCD_WriteData_8bit(0x06);
		
	/*行列地址设置*/
	LCD_WriteCMD(0x2a);
	LCD_WriteData_8bit(0x00);
	LCD_WriteData_8bit(0x00);
	LCD_WriteData_8bit(0x00);
	LCD_WriteData_8bit(0x7f);

	LCD_WriteCMD(0x2b);
	LCD_WriteData_8bit(0x00);
	LCD_WriteData_8bit(0x00);
	LCD_WriteData_8bit(0x00);
	LCD_WriteData_8bit(0x7f);
	
	//Column inversion 
	LCD_WriteCMD(0xB4); 
	LCD_WriteData_8bit(0x07); 	

	//ST7735R Power Sequence
//	LCD_WriteCMD(0xC0); 
//	LCD_WriteData_8bit(0xA2); 
//	LCD_WriteData_8bit(0x02); 
//	LCD_WriteData_8bit(0x84); 
//	LCD_WriteCMD(0xC1); 
//	LCD_WriteData_8bit(0xC5); 

//	LCD_WriteCMD(0xC2); 
//	LCD_WriteData_8bit(0x0A); 
//	LCD_WriteData_8bit(0x00); 

//	LCD_WriteCMD(0xC3); 
//	LCD_WriteData_8bit(0x8A); 
//	LCD_WriteData_8bit(0x2A); 
//	LCD_WriteCMD(0xC4); 
//	LCD_WriteData_8bit(0x8A); 
//	LCD_WriteData_8bit(0xEE); 

	//VCOM 
	LCD_WriteCMD(0xC5); //VCOM 
	LCD_WriteData_8bit(0x0E); 

	//ST7735R Frame Rate
	LCD_WriteCMD(0xB1); 
	LCD_WriteData_8bit(0x01); 
	LCD_WriteData_8bit(0x2C); 
	LCD_WriteData_8bit(0x2D); 

	LCD_WriteCMD(0xB2); 
	LCD_WriteData_8bit(0x01); 
	LCD_WriteData_8bit(0x2C); 
	LCD_WriteData_8bit(0x2D); 

	LCD_WriteCMD(0xB3); 
	LCD_WriteData_8bit(0x01); 
	LCD_WriteData_8bit(0x2C); 
	LCD_WriteData_8bit(0x2D); 
	LCD_WriteData_8bit(0x01); 
	LCD_WriteData_8bit(0x2C); 
	LCD_WriteData_8bit(0x2D); 
	
	//ST7735R Gamma Sequence
//	LCD_WriteCMD(0xe0); 
//	LCD_WriteData_8bit(0x0f); 
//	LCD_WriteData_8bit(0x1a); 
//	LCD_WriteData_8bit(0x0f); 
//	LCD_WriteData_8bit(0x18); 
//	LCD_WriteData_8bit(0x2f); 
//	LCD_WriteData_8bit(0x28); 
//	LCD_WriteData_8bit(0x20); 
//	LCD_WriteData_8bit(0x22); 
//	LCD_WriteData_8bit(0x1f); 
//	LCD_WriteData_8bit(0x1b); 
//	LCD_WriteData_8bit(0x23); 
//	LCD_WriteData_8bit(0x37); 
//	LCD_WriteData_8bit(0x00); 	
//	LCD_WriteData_8bit(0x07); 
//	LCD_WriteData_8bit(0x02); 
//	LCD_WriteData_8bit(0x10); 

//	LCD_WriteCMD(0xe1); 
//	LCD_WriteData_8bit(0x0f); 
//	LCD_WriteData_8bit(0x1b); 
//	LCD_WriteData_8bit(0x0f); 
//	LCD_WriteData_8bit(0x17); 
//	LCD_WriteData_8bit(0x33); 
//	LCD_WriteData_8bit(0x2c); 
//	LCD_WriteData_8bit(0x29); 
//	LCD_WriteData_8bit(0x2e); 
//	LCD_WriteData_8bit(0x30); 
//	LCD_WriteData_8bit(0x30); 
//	LCD_WriteData_8bit(0x39); 
//	LCD_WriteData_8bit(0x3f); 
//	LCD_WriteData_8bit(0x00); 
//	LCD_WriteData_8bit(0x07); 
//	LCD_WriteData_8bit(0x03); 
//	LCD_WriteData_8bit(0x10);  

//	LCD_WriteCMD(0x2a);
//	LCD_WriteData_8bit(0x00);
//	LCD_WriteData_8bit(0x00);
//	LCD_WriteData_8bit(0x00);
//	LCD_WriteData_8bit(0x7f);

//	LCD_WriteCMD(0x2b);
//	LCD_WriteData_8bit(0x00);
//	LCD_WriteData_8bit(0x00);
//	LCD_WriteData_8bit(0x00);
//	LCD_WriteData_8bit(0x9f);	
//	
//	LCD_WriteCMD(0xF0); //Enable test command  
//	LCD_WriteData_8bit(0x01); 
//	LCD_WriteCMD(0xF6); //Disable ram power save mode 
//	LCD_WriteData_8bit(0x00); 
	
	/*设置寄存器读写方向 RGB顺序 面板刷新方向*/
	LCD_WriteCMD(0x36);  
	LCD_WriteData_8bit(0x08); //1100 1000  反向； 0000 1000正向  MY MX MV ML RGB MH - -
	
	//	LCD_ClearGlass(RED);
	LCD_ClearGlass(GREEN);
	
	LCD_WriteCMD(0x29);//打开显示
	SisTic_Delay_ms(100);//100ms
	
}	



void LCD_Reset(void)
{
	LCD_RES_LOW();
	SisTic_Delay_ms(100);//100ms
	LCD_RES_HIGH();
	SisTic_Delay_ms(50);//50ms
	
	LCD_WriteCMD(0x11);//退出睡眠 释放 RESX 后需要等待 5msec 才能发送命令。 也无法在 120 毫秒内发送休眠命令
	SisTic_Delay_ms(100);//100ms
}

void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/*复位 数据_命令切换 背光 引脚的配置*/ //CS脚直接使用SPI的NSS，在spi里配置
	LCD_RES_SCLK_CMDFUNC(LCD_RES_SCLK,ENABLE);//打开引脚时钟
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = LCD_RES_PIN|LCD_DC_PIN|LCD_BL_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LCD_RES_PORT, &GPIO_InitStruct);
}


void LCD_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end)
{
	LCD_WriteCMD(0x2a);//列设置
	LCD_WriteData_16bit(x_start+2);
	LCD_WriteData_16bit(x_end+2);//硬件问题，需要偏移才能完全显示
	
	LCD_WriteCMD(0x2b);//行设置
	LCD_WriteData_16bit(y_start+1);//硬件问题，需要偏移才能完全显示
	LCD_WriteData_16bit(y_end+1);
	
	LCD_WriteCMD(0x2c);//将设置的参数写入LCD的寄存器
}


/*LCD清屏*/
void LCD_ClearGlass(uint16_t Color)
{
	uint16_t i, m;
	LCD_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
	
	for(i=0;i<X_MAX_PIXEL;i++)
	{
		for(m=0;m<Y_MAX_PIXEL;m++)
		{	
			LCD_WriteData_16bit(Color);
		} 
	}
}


/**
  * @brief   LCD清屏
  * @param   
  *		@arg Color ：填充的颜色
  *		@arg USART_DMAReq： 需要配置的DMA请求： USART_DMAReq_Tx/USART_DMAReq_Rx
  *		@arg DMA_MemoryBaseAddr：数据传输的储存器地址
  *		@arg DMA_DIR：传输方向 :  M2P / P2M  (DMA_DIR_PeripheralDST/DMA_DIR_PeripheralSRC)
  *		@arg DMA_BufferSize：数据传输数目
  *		@arg DMA_M2M：是否允许 M2M 传输
  * @retval  
**/
uint8_t color_buff[X_MAX_PIXEL*4];
void LCD_DMA_ClearGlass(uint16_t Color)
{
//	uint16_t i, m;

	
	LCD_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
	
	LCD_WriteCMD(0x2C);//写入RAM
	
	
	LCD_DC_HIGH();//选择写入数据
	LCD_CS_LOW();//打开片选（若spi硬件控制NSS请屏蔽此句）
	
//	
//	uint8_t *ptmp_buff = color_buff;


//		for(m=0;m<Y_MAX_PIXEL*4;m++)
//		{	
//			*ptmp_buff = Color>>8;
//			ptmp_buff++;
//			*ptmp_buff = Color;
//			ptmp_buff++;
//		} 
	
	uint8_t *tmpcolor = (uint8_t*)Color;
	SPI1_DMA_SendBytes(tmpcolor , X_MAX_PIXEL*Y_MAX_PIXEL*2, 0);
	
//	for(i=0;i<X_MAX_PIXEL;i++)
//	{
//		for(m=0;m<Y_MAX_PIXEL;m++)
//		{	
//			while(SPI_I2S_GetFlagStatus(LCD_SPIx, SPI_I2S_FLAG_TXE)==RESET){;}//检测SPI发送缓存器是否为空(可写入要发送的数据)
//			SPI_I2S_SendData(LCD_SPIx,Color>>8);
//			while(SPI_I2S_GetFlagStatus(LCD_SPIx, SPI_I2S_FLAG_TXE)==RESET){;}//检测SPI发送缓存器是否为空(可写入要发送的数据)
//			SPI_I2S_SendData(LCD_SPIx,Color);//写入低8位

//		} 
//	}
	
	while(SPI_I2S_GetFlagStatus(LCD_SPIx,SPI_I2S_FLAG_BSY)!=RESET);//等待SPI线空闲
	LCD_CS_HIGH();//关闭片选（若spi硬件控制NSS请屏蔽此句）
	
}




/*在LCD上画个点*/
void LCD_DispDot(uint8_t x,uint8_t y,uint16_t Color)
{
	LCD_SetRegion(x,y,x+1,y+1);
	LCD_WriteData_16bit(Color);	
}



/*
向LCD写入8位命令
*/
void LCD_WriteCMD(uint8_t Byte)
{
	LCD_DC_LOW();//选择写入命令
	LCD_CS_LOW();//打开片选（若spi硬件控制NSS请屏蔽此句）
	delay_5_nop();
	__nop();
	__nop();
	//SPI_Cmd(LCD_SPIx, ENABLE);//打开SPI（若spi软件控制NSS请屏蔽此句）
	while(SPI_I2S_GetFlagStatus(LCD_SPIx, SPI_I2S_FLAG_TXE)==RESET){;}//检测SPI发送缓存器是否为空(可写入要发送的数据)
	
	SPI_I2S_SendData(LCD_SPIx,Byte);//写入8位命令
	
	
	while(SPI_I2S_GetFlagStatus(LCD_SPIx,SPI_I2S_FLAG_BSY)!=RESET);//等待SPI线空闲
	LCD_CS_HIGH();//关闭片选（若spi硬件控制NSS请屏蔽此句）
	//delay_5_nop();
	
}

/*
向LCD写入8位数据
*/
void LCD_WriteData_8bit(uint8_t Byte)
{
	LCD_DC_HIGH();//选择写入数据
	LCD_CS_LOW();//打开片选（若spi硬件控制NSS请屏蔽此句）
	delay_5_nop();
	__nop();
	__nop();
	while(SPI_I2S_GetFlagStatus(LCD_SPIx, SPI_I2S_FLAG_TXE)==RESET){;}//检测SPI发送缓存器是否为空(可写入要发送的数据)
	SPI_I2S_SendData(LCD_SPIx,Byte);//写入8位数据
	while(SPI_I2S_GetFlagStatus(LCD_SPIx,SPI_I2S_FLAG_BSY)!=RESET);//等待SPI线空闲
	LCD_CS_HIGH();//关闭片选（若spi硬件控制NSS请屏蔽此句）
	//delay_5_nop();
	
	
	
}

/*
向LCD写入16位数据
*/
void LCD_WriteData_16bit(uint16_t Byte)
{
	LCD_DC_HIGH();//选择写入数据
	LCD_CS_LOW();//打开片选（若spi硬件控制NSS请屏蔽此句）
	delay_5_nop();
	
	while(SPI_I2S_GetFlagStatus(LCD_SPIx, SPI_I2S_FLAG_TXE)==RESET){;}//检测SPI发送缓存器是否为空(可写入要发送的数据)
	SPI_I2S_SendData(LCD_SPIx,Byte>>8);
	while(SPI_I2S_GetFlagStatus(LCD_SPIx, SPI_I2S_FLAG_TXE)==RESET){;}//检测SPI发送缓存器是否为空(可写入要发送的数据)
	SPI_I2S_SendData(LCD_SPIx,Byte);//写入低8位
	while(SPI_I2S_GetFlagStatus(LCD_SPIx,SPI_I2S_FLAG_BSY)!=RESET);//等待SPI线空闲
	LCD_CS_HIGH();//关闭片选（若spi硬件控制NSS请屏蔽此句）
	//delay_5_nop();
}

/*
向LCD写入24位数据
*/
void LCD_WriteData_24bit(uint32_t Byte)
{
	LCD_DC_HIGH();//选择写入数据
	LCD_CS_LOW();//打开片选（若spi硬件控制NSS请屏蔽此句）
	while(SPI_I2S_GetFlagStatus(LCD_SPIx, SPI_I2S_FLAG_TXE)==RESET){;}//检测SPI发送缓存器是否为空(可写入要发送的数据)
	SPI_I2S_SendData(LCD_SPIx,Byte>>16);//写入高8位
	SPI_I2S_SendData(LCD_SPIx,Byte>>8);//写入中间8位
	SPI_I2S_SendData(LCD_SPIx,Byte);//写入低8位
	while(SPI_I2S_GetFlagStatus(LCD_SPIx,SPI_I2S_FLAG_BSY)!=RESET);//等待SPI线空闲
	LCD_CS_HIGH();//关闭片选（若spi硬件控制NSS请屏蔽此句）
}

/*
从LCD读取8位数据
*/
uint8_t LCD_ReadData8bit(uint8_t CMD)
{
	uint8_t RByte;
	//uint8_t i;
//	LCD_SPIx->CR1 |= SPI_BaudRatePrescaler_8;//波特率改为8分频
	LCD_CS_LOW();//选择从机
	LCD_DC_LOW();//选择写入命令
	delay_5_nop();
	__nop();
	__nop();
	while(SPI_I2S_GetFlagStatus(LCD_SPIx, SPI_I2S_FLAG_TXE)!=SET);
	SPI_I2S_SendData(LCD_SPIx, CMD);//写入命令
	while(SPI_I2S_GetFlagStatus(LCD_SPIx,SPI_I2S_FLAG_BSY)!=RESET);//等待SPI线空闲

	SPI_BiDirectionalLineConfig(LCD_SPIx, SPI_Direction_Rx);//切换为只读
	
	//for(i=0; i<2; i++)//
	{
		while(SPI_I2S_GetFlagStatus(LCD_SPIx, SPI_I2S_FLAG_RXNE)!=SET);
		RByte = SPI_I2S_ReceiveData(LCD_SPIx);
	}
	while(SPI_I2S_GetFlagStatus(LCD_SPIx, SPI_I2S_FLAG_BSY)!=RESET);//等待传输结束
	LCD_CS_HIGH();//取消选择从机
	SPI_I2S_ReceiveData(LCD_SPIx);//空读，复位RXNE
	//delay_5_nop();
	SPI_BiDirectionalLineConfig(LCD_SPIx, SPI_Direction_Tx);//切换为只写
	
	return RByte;
}

/*
从LCD读取16位数据（未测试）
*/
uint16_t LCD_ReadData16bit(uint8_t CMD)
{
	return 0;
}


/*
从LCD读取32位数据
*/
void LCD_ReadData32bit(uint8_t CMD,uint16_t* RByte)
{
	uint8_t i;
//	LCD_SPIx->CR1 |= SPI_BaudRatePrescaler_8;//波特率改为8分频
	LCD_CS_LOW();//选择从机
	LCD_DC_LOW();//选择写入命令
	delay_5_nop();
	__nop();
	__nop();
	while(SPI_I2S_GetFlagStatus(LCD_SPIx, SPI_I2S_FLAG_TXE)!=SET);
	SPI_I2S_SendData(LCD_SPIx, CMD);//写入命令
	while(SPI_I2S_GetFlagStatus(LCD_SPIx,SPI_I2S_FLAG_BSY)!=RESET);//等待SPI线空闲
	LCD_DC_HIGH();
	SPI_BiDirectionalLineConfig(LCD_SPIx, SPI_Direction_Rx);//切换为只读
	
	
	for(i=0; i<4; i++)//读取4个Byte
	{
		while(SPI_I2S_GetFlagStatus(LCD_SPIx, SPI_I2S_FLAG_RXNE)!=SET);
		*(RByte+i) = SPI_I2S_ReceiveData(LCD_SPIx);
	}
	while(SPI_I2S_GetFlagStatus(LCD_SPIx, SPI_I2S_FLAG_BSY)!=RESET);//等待传输结束
	LCD_CS_HIGH();//取消选择从机
	SPI_I2S_ReceiveData(LCD_SPIx);//空读，复位RXNE
	//delay_5_nop();
	SPI_BiDirectionalLineConfig(LCD_SPIx, SPI_Direction_Tx);//切换为只写
//	LCD_SPIx->CR1 &= (~SPI_BaudRatePrescaler_256);//波特率改为2分频
	
	
	/*
	04: 0011 1110   0100 0100     1111 1000
	da: 0111 1100 //1000 1001     1111 0000
	db:           //1000 1001
	dc:                         //1111 0000
	
	ST7735S连续读取时，需要在第一位加一个空时钟（所有数据右移了一位）
	
	*/
	for(i=0; i<3; i++)//读取4个Byte
	{
		*(RByte+i)<<=1;
		*(RByte+i) |= (((*(RByte+i+1)) & 0x80))>>7;
		*(RByte+i+1) &= 0xff7f;
	}
	*(RByte+3)<<=1;
}
/*
获取LCD上点的颜色
*/
uint32_t LCD_GetDotRGB(uint16_t x,uint16_t y,uint16_t* RByte)
{

	/*262k 6-6-6 mode */
	LCD_WriteCMD(0x3A);  
	LCD_WriteData_8bit(0x66);//读取前切换为18位模式
	
	LCD_SetRegion(x,y,x+1,y+1);
	LCD_ReadData32bit(0x2e, RByte);
	
	/*65k 5-6-5 mode */
	LCD_WriteCMD(0x3A);  
	LCD_WriteData_8bit(0x05); 	//读取后切回原模式
		
	/*
	R<<=6;          0011 1111
	G<<=3;0000 0111 1110 0000  1111 1100  0111 1110
	B>>=3;0000 0010 0000 0010
	*/
	*(RByte+1)<<=8;
	*(RByte+2)<<=3;
	*(RByte+3)>>=3;
	//*(RByte)=*(RByte+1)|*(RByte+2)|*(RByte+3);

	return 0;
	
}

