#ifndef _TFT_FONT_H
#define _TFT_FONT_H

/*
本文件只支持1.44’ 128*128 TFT
*/


/*****************************************************/
#include "stm32f10x.h"
#include "bsp_144spi_tft_lcd.h"
#include "delay.h" 
#include "math.h" 


/*
XS,YS:线段起点           （必须指定）
线段结构体
XEnd，YEnd：线段终点     （可选）
Length:线段长度          （可选）
Angel：与X轴正向夹角（°）（可选）
Radian:Angel对应的弧度   （可选）

注：Angel与Radian只能指定一个
	若指定了起点和终点则，其余参数都无需指定
	未指定的数据都需赋值为0
*/
typedef struct
{
	uint8_t XS;
	uint8_t YS; 
	uint8_t XEnd;
	uint8_t YEnd; 
	uint8_t Length;
	float Angel;
	float Radian;
}TFTLine_TypeDef;

/*
圆结构体
mX,mY:圆心
R:半径
Angel；八分圆弧的角度（0-45°）
*/
typedef struct
{
	uint8_t mX;
	uint8_t mY; 
	uint8_t R; 
	float Angel;

}TFTCircle_TypeDef;


/*
坐标结构体
*/
typedef struct
{
	uint8_t X;
	uint8_t Y; 
}TFTCoordinate_TypeDef;








void Display_8x16_Num(uint8_t XS,uint8_t XE,uint8_t YS,uint8_t YE,uint16_t FColor,uint16_t BColor,uint8_t num);

void Display_16x24_Num(uint8_t XS, uint8_t YS, uint8_t Xlen, uint8_t Ylen, uint16_t FColor, uint16_t BColor, uint8_t num);
void Display_16x24_Font(uint8_t XS, uint8_t YS, uint8_t Xlen, uint8_t Ylen, uint16_t FColor, uint16_t BColor, uint8_t font);
void Display_Image(uint8_t XS, uint8_t YS, uint8_t Xlen, uint8_t Ylen, uint8_t* pImge);
void showimage(const unsigned char *p);
void Display_8x16_ASCII(uint8_t XS,uint8_t YS,uint16_t FColor,uint16_t BColor,char* ascii);
void Display_8x16_NUM(uint8_t XS,uint8_t YS,uint16_t FColor,uint16_t BColor,uint16_t num);

void Draw_Line(TFTLine_TypeDef* Line_Struct,uint16_t FColor);
void Draw_BresenhamLine(TFTLine_TypeDef* Line_Struct,uint16_t FColor);
void Draw_BresenhamARC(TFTCircle_TypeDef* TFTCircle_Struct, TFTCoordinate_TypeDef* CoordinateStructArry , uint8_t* k ,uint16_t FColor);
void Draw_ARC(TFTCircle_TypeDef* TFTCircle_Struct,uint16_t AngelS,uint16_t AngelE,uint16_t FColor);
void TFT_Display_XxX_ASCII(uint8_t XS,uint8_t YS, uint8_t width,uint8_t height, uint16_t FColor,uint16_t BColor,char* ascii,...);
#endif /*_TFT_FONT_H*/
