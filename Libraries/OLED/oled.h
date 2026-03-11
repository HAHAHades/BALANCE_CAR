#ifndef __OLED_H
#define __OLED_H 


#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "stdlib.h"	
 
#include "bsp_iic.h"
 	 

#define OLED_IICx I2C1
#define OLED_IICx_IO_Reamp IIC1_IO_Reamp1


#define OLED_IIC_ADDRESS7 0x3C<<1  //OLED IIC 地址    0011 1100->0111 1000 0x78
#define OLED_IIC_CMD 0x00 //OLED 发送指令
#define OLED_IIC_DATA 0x40 //发送数据

#define OLED_Pixel_width 128  //OLED屏幕宽度
#define OLED_Pixel_height 32  //OLED屏幕高度


//-----------------OLED端口定义---------------- 

//#define OLED_SCL_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_0)//SCL
//#define OLED_SCL_Set() GPIO_SetBits(GPIOA,GPIO_Pin_0)

//#define OLED_SDA_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_1)//DIN
//#define OLED_SDA_Set() GPIO_SetBits(GPIOA,GPIO_Pin_1)

//#define OLED_RES_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_2)//RES
//#define OLED_RES_Set() GPIO_SetBits(GPIOA,GPIO_Pin_2)


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);


uint32_t OLED_WR_Byte(u8 dat,u8 mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
uint32_t OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);

//void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);
//void OLED_DrawCircle(u8 x,u8 y,u8 r);
//void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode);
//void OLED_ShowChar6x8(u8 x,u8 y,u8 chr,u8 mode);
//void OLED_ShowString(u8 x,u8 y,char *chr,u8 size1,u8 mode);
//void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
//void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode);
//void OLED_ScrollDisplay(u8 num,u8 space,u8 mode);
//void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode);

void OLED_Init(void);
void OLED_Display_XxX_ASCII(uint8_t XS,uint8_t YS, uint8_t width,uint8_t height, uint8_t mode, char *fmt,...);
#endif

