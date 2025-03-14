#include "bsp_TFT_display.h"



//
uint8_t TFT_LCD_FlashBuff[X_MAX_PIXEL][Y_MAX_PIXEL];



/*
在TFT上显示字符串
每个字符占用Xlen*Ylen的点阵
8*12 8*16  12*18 12*24  16*24 16*32
参数：XS:X轴起始地址 YS:Y轴起始地址
	  FColor:前景色  BColor:背景色
	  Xlen：X向宽度  Ylen：Y向宽度
      ascii:要显示的字符串
注：字符串的内容应在ASCII表0x20-0x7e范围内或是换行符
*/
extern uint8_t ASCII_Font8x12[];
extern uint8_t ASCII_Font8x16[];
//extern uint8_t ASCII_Font12x18[];
//extern uint8_t ASCII_Font12x24[];
//extern uint8_t ASCII_Font16x24[];
//extern uint8_t ASCII_Font16x32[];




/*
在TFT上显示字符串
每个字符占用8*16的点阵
参数：XS:X轴起始地址 YS:Y轴起始地址
	  FColor:前景色  BColor:背景色
      ascii:要显示的字符串
注：字符串的内容应在ASCII表0x20-0x7e范围内或是换行符
*/

void Display_8x16_ASCII(uint8_t XS,uint8_t YS,uint16_t FColor,uint16_t BColor,char* ascii)
{
	uint8_t i,j;
	uint8_t* pChar = ASCII_Font8x16;
	do
	{
		pChar = ASCII_Font8x16 + ((*ascii-0x20)*16);
		for(i=0;i<16;i++)//y
		{
			for(j=0;j<8;j++)//x
			{
				if((*(pChar+i)) & (0x80>>j))
				{
					LCD_DispDot(j+XS, i+YS,FColor);
				}
				else
				{
					LCD_DispDot(j+XS, i+YS,BColor);
				}
			}	
		}
		XS+=8;
		ascii++;
		while(*ascii==0x0a)//\n
		{XS = 0;YS+=16;ascii++;}
		if(XS>120)
		{
			if(YS>120)
				return;//行列都超范围直接退出
			else
			{
				XS = 0;
				YS += 16;//行超范围直接换行
			}
		}
	}while(*ascii != 0x00);
	
}

/*
在TFT上显示16位 16进制数
每个字符占用8*16的点阵
参数：XS:X轴起始地址 YS:Y轴起始地址
	  FColor:前景色  BColor:背景色
      num:要显示的16位数字
*/
void Display_8x16_NUM(uint8_t XS,uint8_t YS,uint16_t FColor,uint16_t BColor,uint16_t num)
{
	uint8_t i,j,k;
	uint8_t* pChar = ASCII_Font8x16;
	

	for(k=0;k<4;k++)//每次显示一个数（四位）
	{
		if((num/(1<<(4*3))) < 0xa)
		{
			pChar = ASCII_Font8x16 + 16*16 + ((num/(1<<(4*3)))*16);
		}
		else
		{
			pChar = ASCII_Font8x16 + 33*16 + (((num/(1<<(4*3)))-10)*16);
		}
		for(i=0;i<16;i++)/*y*/
		{
			for(j=0;j<8;j++)/*x*/
			{
				if((*(pChar+i)) & (0x80>>j))
				{
					LCD_DispDot(j+XS, i+YS,FColor);
				}
				else
				{
					LCD_DispDot(j+XS, i+YS,BColor);
				}
			}	
		}
		XS+=8;
		num<<=4;
		if(XS>128)
		{XS = 0;YS+=16;}
		if(XS>128||YS>128){return;}//超范围直接退出
	}
	
}

/*
在TFT的某16x24区域内显示数字
参数：XS:X轴起始地址 YS:Y轴起始地址
	  Xlen:X方向宽度 YE:Y方向宽度
	  FColor:前景色  BColor:背景色
      num:显示的字符
*/

//extern  uint8_t Num_16x24_Buffer[];
//void Display_16x24_Num(uint8_t XS, uint8_t YS, uint8_t Xlen, uint8_t Ylen, uint16_t FColor, uint16_t BColor, uint8_t num)
//{
//	uint8_t i,j;
//	uint8_t k;
//	/*
//	pChar为数字num对应数组中的首字节地址
//	用Xlen列*Ylen行的区域表示一个字符
//	每(Xlen/8)字节为显示区域的一行，共Ylen行
//	*/
//	uint8_t *pChar = (Num_16x24_Buffer+(num*(Xlen/8)*Ylen));

//	for(i=0;i<Ylen;i++)/*y*/
//	{
//		for(j=0;j<(Xlen/8);j++)/*x*/
//		{
//			for(k=0;k<8;k++)/*x*/
//			{
//				if((*(pChar+(i*(Xlen/8))+j)) & (0x80>>k))
//				{
//					LCD_DispDot((j*8+k)+XS, i+YS,FColor);
//				}
//				else
//				{
//					LCD_DispDot((j*8+k)+XS, i+YS,BColor);
//				}
//			}
//		}
//	}	
//}


/*
在TFT的某16x24区域内显示文字
参数：XS:X轴起始地址 YS:Y轴起始地址
	  Xlen:X方向宽度 YE:Y方向宽度
	  FColor:前景色  BColor:背景色
      font:字符在字符表里的序号
*/

//extern uint8_t Font_16x24_Buffer[];
//void Display_16x24_Font(uint8_t XS, uint8_t YS, uint8_t Xlen, uint8_t Ylen, uint16_t FColor, uint16_t BColor, uint8_t font)
//{
//	uint8_t i,j;
//	uint8_t k;
//	/*
//	pChar为文字font对应数组中的首字节地址
//	用Xlen列*Ylen行的区域表示一个字符
//	每(Xlen/8)字节为显示区域的一行，共Ylen行
//	*/
//	uint8_t *pChar = (Font_16x24_Buffer+(font*(Xlen/8)*Ylen));

//	for(i=0;i<Ylen;i++)/*y*/
//	{
//		for(j=0;j<(Xlen/8);j++)/*x*/
//		{
//			for(k=0;k<8;k++)/*x*/
//			{
//				if((*(pChar+(i*(Xlen/8))+j)) & (0x80>>k))
//				{
//					LCD_DispDot((j*8+k)+XS, i+YS,FColor);
//				}
//				else
//				{
//					LCD_DispDot((j*8+k)+XS, i+YS,BColor);
//				}
//			}
//		}
//	}	
//}


/*
在TFT的某Xlen*Ylen区域内显示图片
参数：XS:X轴起始地址 YS:Y轴起始地址
	  Xlen:X方向宽度 YE:Y方向宽度
      image:图片RGB数组
*/
//extern unsigned char gImage_wyy[3200];/*40*40*/

//void Display_Image(uint8_t XS, uint8_t YS, uint8_t Xlen, uint8_t Ylen, uint8_t* pImge)
//{
//	uint16_t im=0;
//	uint32_t k;
//	
//	LCD_SetRegion(XS,YS,XS+Xlen-1,YS+Ylen-1);
//	for(k=0;k<Xlen*Ylen;k++)/*k个像素点*/
//	{
//		im |= (uint16_t)*(pImge+2*k);//高8位
//		im<<=8;
//		im |=(uint16_t) *(pImge+2*k+1);//低8位
//		//LCD_DispDot((k%Xlen)+XS, (k/Xlen)+YS,im);
//		LCD_WriteData_16bit(im);
//	}
//}



/*

在TFT上画线段(含浮点数运算)

*/
void Draw_Line(TFTLine_TypeDef* Line_Struct,uint16_t FColor)
{
	float Radian;
	uint16_t i,j;

	if(Line_Struct->Radian > 0.001)//优先按照弧度计算，float大概可以达到6位精度
	{
		Radian =  Line_Struct->Radian;
		Line_Struct->Angel = ((Line_Struct->Radian)/3.141593)*180 ;
	}
	else
	{
		Radian = ((Line_Struct->Angel)/180.0)*3.141593;
	}
	if(!((Line_Struct->XEnd)||(Line_Struct->YEnd)))	//优先按照给定的终点绘制
	{
		Line_Struct->XEnd = (Line_Struct->XS + (Line_Struct->Length*cos(Radian))+0.5);//四舍五入
		Line_Struct->YEnd = (Line_Struct->YS + (Line_Struct->Length*sin(Radian))+0.5);//四舍五入
	}
	else//优先按照给定的终点计算长度
	{
		Line_Struct->Length = (0.5+sqrt(((Line_Struct->XEnd - Line_Struct->XS)* \
										(Line_Struct->XEnd - Line_Struct->XS))- \
										((Line_Struct->YEnd - Line_Struct->YS)* \
										(Line_Struct->YEnd - Line_Struct->YS))));
	}
	
	
	
	if(((Line_Struct->Angel >= 0) && (Line_Struct->Angel <= 45)) || \
		((Line_Struct->Angel >= 135) && (Line_Struct->Angel <= 225)) || \
	((Line_Struct->Angel >= 315) && (Line_Struct->Angel < 360)))
	{
		/*
		y=tan(Radian)*(x-XS)+YS
		*/
		if(Line_Struct->XEnd < Line_Struct->XS)
		{
			for(i=Line_Struct->XS; i>=Line_Struct->XEnd;i--)
			{
				j = tan(Radian)*(i-Line_Struct->XS)+Line_Struct->YS + 0.5;
				if(((Line_Struct->YEnd >= Line_Struct->YS) && j > Line_Struct->YEnd) || \
					((Line_Struct->YEnd < Line_Struct->YS) && (j < Line_Struct->YEnd)))
				{return;}
				LCD_DispDot(i, j,FColor);
			}
		}
		else
		{
			for(i=Line_Struct->XS; i<=Line_Struct->XEnd;i++)
			{
				j = tan(Radian)*(i-Line_Struct->XS)+Line_Struct->YS + 0.5;
				if(((Line_Struct->YEnd >= Line_Struct->YS) && j > Line_Struct->YEnd) || \
					((Line_Struct->YEnd < Line_Struct->YS) && (j < Line_Struct->YEnd)))
				{return;}
				LCD_DispDot(i, j,FColor);
			}
		}
	}
	else
	{
		/*
		x=(y-YS)*ctan(Radian)+XS
		*/
		if(Line_Struct->YEnd < Line_Struct->YS)
		{
			for(j=Line_Struct->YS; j>=Line_Struct->YEnd; j--)
			{
				i = ((j-Line_Struct->YS)/tan(Radian))+Line_Struct->XS + 0.5;
				if(((Line_Struct->XEnd >= Line_Struct->XS) && i > Line_Struct->XEnd) || \
					((Line_Struct->XEnd < Line_Struct->XS) && (i < Line_Struct->XEnd)))
				{return;}
				LCD_DispDot(i, j,FColor);
			}
		}
		else
		{
			for(j=Line_Struct->YS; j<=Line_Struct->YEnd; j++)
			{
				i = ((j-Line_Struct->YS)/tan(Radian))+Line_Struct->XS + 0.5;
				if(((Line_Struct->XEnd >= Line_Struct->XS) && i > Line_Struct->XEnd) || \
					((Line_Struct->XEnd < Line_Struct->XS) && (i < Line_Struct->XEnd)))
				{return;}
				LCD_DispDot(i, j,FColor);
			}
		}
		
	}
}

/*
Bresenham算法画线

y=kx+b (0<k1);  
令k=dy/dx;
令e为x变化时直线距离其下方已点亮点的最小垂直误差，当e<0.5时应点亮直线下方的点，否则点亮上方的点
e0=0；
当x=x+1时;y=y+1*k;
if k>0.5 y=y+1;  //点亮上方的点，则下一个误差应按照直线到新点的距离算，新点与旧点的距离为一  下一个点误差e=e + 1*k - 1;
esle     y=y;//点亮直线下方的点，则下一个误差任按照直线到旧点的距离算，下一个点误差e = e + 1*k；
（例：x点误差e=0<0.5 下个e=0+0.4 ；x+1点y=y+0.4 误差为e=0.4<0.5点亮下侧点 下个e=0.4+0.4 ; x+2点y=y+0.8 误差e=0.4+0.4>0.5点亮上侧点 下个e=0.4+0.4+0.4-1; 
x+3点y=y+1.2 误差为e=0.2<0.5点亮下侧点 下个e=0.2+0.4; ...... ）

为除去浮点数运算，令e0=-0.5; 则e>0点亮上侧点 e<=0点亮下侧点
令err=2*dx*e 同样可根据err符号判断
则点亮上方的点时 err=2*dx*e + 2*dy - 2*dx = err + 2*dy - 2*dx;
点亮下方的点时 err=2*dx*e + 2*dy = err + 2*dy;
err0 = 2*dx*e0 = -dx；
这样就只剩整数的加法和逻辑运算了
*/
void Draw_BresenhamLine(TFTLine_TypeDef* Line_Struct,uint16_t FColor)
{
	float Radian;
	uint16_t i,j,dx,dy;
	int32_t err;
	
	if(Line_Struct->Radian>0.001)//优先按照弧度计算
	{
		Radian =  Line_Struct->Radian;
		Line_Struct->Angel = ((Line_Struct->Radian)/3.141593)*180 ;//float大概可以达到6位精度
	}
	else
	{
		Radian = ((Line_Struct->Angel)/180.0)*3.141593;
	}
	if(!((Line_Struct->XEnd)||(Line_Struct->YEnd)))	//优先按照给定的终点绘制
	{
		Line_Struct->XEnd = (Line_Struct->XS + (Line_Struct->Length*cos(Radian))+0.5);//四舍五入
		Line_Struct->YEnd = (Line_Struct->YS + (Line_Struct->Length*sin(Radian))+0.5);//四舍五入
	}
	else//优先按照给定的终点计算长度
	{
		Line_Struct->Length = (0.5+sqrt(((Line_Struct->XEnd - Line_Struct->XS)* \
										(Line_Struct->XEnd - Line_Struct->XS))- \
										((Line_Struct->YEnd - Line_Struct->YS)* \
										(Line_Struct->YEnd - Line_Struct->YS))));
	}

	dx = Line_Struct->XEnd - Line_Struct->XS;
	dy = Line_Struct->YEnd - Line_Struct->YS;
	err= -dx;
	j=Line_Struct->YS;
	for(i = Line_Struct->XS; i<=Line_Struct->XEnd; i++)
	{
		if(err>0)
		{
			j+=1;//y+1
			err -= 2*dx;
		}
		err += 2*dy;
		 if(j>Line_Struct->YEnd)
		 {
			 return;
		 }
		 LCD_DispDot(i, j,FColor);
	}
	
}






/*
Bresenham法画一个八分象限圆弧（获取坐标）

圆方程：(x-x0)^2 + *(y-y0)^2 = R^2
从正上方顺时针画45°
(1) 第一点(x,y) = (x0,y0+R)直接点亮
(2) x=x+1;  此时应点亮(x+1,y)或(x+1,y-1)
令 d为A点(x+1,y-0.5)到圆心距离
若 d^2 = (x+1-x0)^2 + (y-0.5-y0)^2 >=R^2 点亮(x+1,y-1)
否则 							   <	 点亮(x+1,y)

(3) x=x+2;此时
1. 若(2)中点亮的是(x+1,y) 即 d^2 < R^2
	此时应点亮(x+2,y-1)或(x+2,y)
	若d1^2 = (x+2-x0)^2 + (y-0.5-y0)^2 >=R^2  点亮(x+2,y-1)
	否则 							   <	  点亮(x+2,y)
2.若(2)中点亮的是(x+1,y-1) 即 d^2 >= R^2
	此时应点亮(x+2,y-2)或(x+2,y-1)
	若d1^2 = (x+2-x0)^2 + (y-1.5-y0)^2 >=R^2  点亮(x+2,y-2)
	否则 							   <	  点亮(x+2,y-1)

由(2) d^2 = (x+1-x0)^2 + (y-0.5-y0)^2 >=R^2 带入点(x0,y0+R)  得d^2 = 1.25+R^2-R
	d^2 >= R^2 即 1.25-R >= 0
可令e = 4*(d^2 - R^2) 则初值e0 = 5 - 4*R
由(3) e0 >= 0时
	  e1 = e0+8*(x-y-x0+y0)+20  y=y-1;
	  e0 < 0时
	  e1 = e0+8*(x-x0)+12  y=y;
	  
TFTCircle_Struct：圆参数结构体地址（只需指定圆心及半径）
CoordinateStructArry：第一个八分象限需要点亮的座标结构体数组地址
k：第一个八分象限需要点亮的座标数的地址
FColor：绘制的颜色
*/
void Draw_BresenhamARC(TFTCircle_TypeDef* TFTCircle_Struct, TFTCoordinate_TypeDef* CoordinateStructArry , uint8_t* k ,uint16_t FColor)
{
	uint16_t i,j;
	int32_t err;

	j = TFTCircle_Struct->mY + TFTCircle_Struct->R;
	i=TFTCircle_Struct->mX;
	err = 5-4*TFTCircle_Struct->R;
	*k=0;
	for(i=TFTCircle_Struct->mX;i<=j;i++)
	{
		if(err>0)
		{
			err +=  20+8* (i-j-TFTCircle_Struct->mX+TFTCircle_Struct->mY);
			j-=1;
		}
		else
		{
			err +=  12+8* (i-TFTCircle_Struct->mX);
		}
		(CoordinateStructArry+i)->X = i;
		(CoordinateStructArry+i)->Y = j;
		(*k)++;
	}	
	(*k)--;	
	
	/*disp*/
//	for(i=0;i<=(*k);i++)
//	{
//		LCD_DispDot((CoordinateStructArry+TFTCircle_Struct->mX+i)->X, (CoordinateStructArry+TFTCircle_Struct->mX+i)->Y,FColor);//1
//	}	
}	



/*
画圆弧
TFTCircle_Struct：圆参数结构体地址（只需指定圆心及半径）
AngelS, AngelE:从y轴正向顺时针(往x轴正向)开始的起止角度（°）
FColor：绘制的颜色
*/
void Draw_ARC(TFTCircle_TypeDef* TFTCircle_Struct,uint16_t AngelS,uint16_t AngelE,uint16_t FColor)
{
	uint8_t i,j,quadrantS,quadrantE,xS,xE;
	uint8_t k=0;//第一个八分象限需要点亮的座标数
	float Radian;
	_Bool flag = 0;
	TFTCoordinate_TypeDef CoordinateStructArry[130];//第一个八分象限需要点亮的座标
	Draw_BresenhamARC(TFTCircle_Struct, CoordinateStructArry, &k ,RED);//Bresenham法获取第一个八分象限坐标

	quadrantS = AngelS/45 + 1;
	quadrantE = (AngelE-0.1)/45 + 1;//起止八分象限，-0.1是为了防止终点刚好在八分轴上
	if(quadrantS!=quadrantE)
	{flag = 1;}
	if(!(quadrantS%2))/*起点在2 4 6 8*/
	{
		Radian = ((45-(AngelS-45*(quadrantS-1)))/180.0)*3.141593;//起点角度
		xS = sin(Radian)*TFTCircle_Struct->R + TFTCircle_Struct->mX;
	}
	else/*起点在1 3 5 7*/
	{
		Radian = ((AngelS-45*(quadrantS-1))/180.0)*3.141593;//起点角度
		xS = sin(Radian)*TFTCircle_Struct->R + TFTCircle_Struct->mX;
	}
	if(!(quadrantE%2))/*终点在2 4 6 8*/
	{
		Radian = ((45-(AngelE-45*(quadrantE-1)))/180.0)*3.141593;//起点角度
		xE = sin(Radian)*TFTCircle_Struct->R + TFTCircle_Struct->mX;
	}
	else/*终点在1 3 5 7*/
	{
		Radian = ((AngelE-45*(quadrantE-1))/180.0)*3.141593;//终点角度
		xE = sin(Radian)*TFTCircle_Struct->R + TFTCircle_Struct->mX;
	}
	

	
	if(flag)//起点终点不在同一八分象限
	{
		/*起点*/
		j=k+TFTCircle_Struct->mX;
		switch (quadrantS)
		{
			case 1:
			{
				for(i=xS; i<j; i++)
				{
					LCD_DispDot(CoordinateStructArry[i].X, CoordinateStructArry[i].Y, FColor);//1
				}
				break;
			}
			case 2:
			{
				for(i=xS; i>TFTCircle_Struct->mX; i--)
				{
					LCD_DispDot(CoordinateStructArry[i].Y, CoordinateStructArry[i].X,FColor);//2
				}
				break;
			}
			case 3:
			{
				for(i=xS; i<j; i++)
				{
					LCD_DispDot(CoordinateStructArry[i].Y, 2*TFTCircle_Struct->mX - CoordinateStructArry[i].X,FColor);//3
				}
				break;
			}
			case 4:
			{
				for(i=xS; i>TFTCircle_Struct->mX; i--)
				{
					LCD_DispDot(CoordinateStructArry[i].X, 2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y,FColor);//4
				}
				break;
			}
			case 5:
			{
				for(i=xS; i<j; i++)
				{
					LCD_DispDot(2*TFTCircle_Struct->mX - CoordinateStructArry[i].X, 2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y, FColor);//5
				}
				break;
			}
			case 6:
			{
				for(i=xS; i>TFTCircle_Struct->mX; i--)
				{
					LCD_DispDot(2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y, 2*TFTCircle_Struct->mX - CoordinateStructArry[i].X,FColor);//6
				}
				break;
			}
			case 7:
			{
				for(i=xS; i<j; i++)
				{
					LCD_DispDot(2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y, CoordinateStructArry[i].X, FColor);//7
				}
				break;
			}
			default:
			{return ;}
		}

		/*中间段*/
		j=k+TFTCircle_Struct->mX;
		if((quadrantS < 2) && (quadrantE > 2))
		{
			for(i=TFTCircle_Struct->mX;i<j;i++)
			{
				LCD_DispDot(CoordinateStructArry[i].Y, CoordinateStructArry[i].X,FColor);//2
			}
		}
		if((quadrantS < 3) && (quadrantE > 3))
		{
			for(i=TFTCircle_Struct->mX;i<j;i++)
			{
				LCD_DispDot(CoordinateStructArry[i].Y, 2*TFTCircle_Struct->mX - CoordinateStructArry[i].X,FColor);//3
			}
		}
		if((quadrantS < 4) && (quadrantE > 4))
		{
			for(i=TFTCircle_Struct->mX;i<j;i++)
			{
				LCD_DispDot(CoordinateStructArry[i].X, 2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y,FColor);//4
			}
		}
		if((quadrantS < 5) && (quadrantE > 5))
		{
			for(i=TFTCircle_Struct->mX;i<j;i++)
			{
				LCD_DispDot(2*TFTCircle_Struct->mX - CoordinateStructArry[i].X, 2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y, FColor);//5
			}
		}
		if((quadrantS < 6) && (quadrantE > 6))
		{
			for(i=TFTCircle_Struct->mX;i<j;i++)
			{
				LCD_DispDot(2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y, 2*TFTCircle_Struct->mX - CoordinateStructArry[i].X,FColor);//6
			}
		}
		if((quadrantS < 7) && (quadrantE > 7))
		{
			for(i=TFTCircle_Struct->mX;i<j;i++)
			{
				LCD_DispDot(2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y, CoordinateStructArry[i].X, FColor);//7
			}
		}
	
		/*终点*/
		j=k+TFTCircle_Struct->mX;
		switch (quadrantE)
		{
			case 2:
			{
				for(i=j; i>=xE; i--)
				{
					LCD_DispDot(CoordinateStructArry[i].Y, CoordinateStructArry[i].X,FColor);//2
				}
				break;
			}
			case 3:
			{
				for(i=TFTCircle_Struct->mX; i<=xE; i++)
				{
					LCD_DispDot(CoordinateStructArry[i].Y, 2*TFTCircle_Struct->mX - CoordinateStructArry[i].X,FColor);//3
				}
				break;
			}
			case 4:
			{
				for(i=j; i>=xE; i--)
				{
					LCD_DispDot(CoordinateStructArry[i].X, 2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y,FColor);//4
				}
				break;
			}
			case 5:
			{
				for(i=TFTCircle_Struct->mX; i<=xE; i++)
				{
					LCD_DispDot(2*TFTCircle_Struct->mX - CoordinateStructArry[i].X, 2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y, FColor);//5
				}
				break;
			}
			case 6:
			{
				for(i=j; i>=xE; i--)
				{
					LCD_DispDot(2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y, 2*TFTCircle_Struct->mX - CoordinateStructArry[i].X,FColor);//6
				}
				break;
			}
			case 7:
			{
				for(i=TFTCircle_Struct->mX; i<=xE; i++)
				{
					LCD_DispDot(2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y, CoordinateStructArry[i].X, FColor);//7
				}
				break;
			}
			case 8:
			{
				for(i=j; i>=xE; i--)
				{
					LCD_DispDot(2*TFTCircle_Struct->mX - CoordinateStructArry[i].X, CoordinateStructArry[i].Y, FColor);//8
				}
				break;
			}
			default:
			{return ;}
		}		
			
	}
	else//起点终点在同一八分象限
	{
		switch(quadrantS)
		{
			case 1:
			{
				for(i=xS;i<=xE;i++)
				{
					LCD_DispDot(CoordinateStructArry[i].X, CoordinateStructArry[i].Y, FColor);//1
				}
				break;
			}
			case 2:
			{
				for(i=xS;i>=xE;i--)
				{
					LCD_DispDot(CoordinateStructArry[i].Y, CoordinateStructArry[i].X,FColor);//2
				}
				break;
			}
			case 3:
			{
				for(i=xS;i<=xE;i++)
				{
					LCD_DispDot(CoordinateStructArry[i].Y, 2*TFTCircle_Struct->mX - CoordinateStructArry[i].X,FColor);//3
				}
				break;
			}
			case 4:
			{
				for(i=xS;i>=xE;i--)
				{
					LCD_DispDot(CoordinateStructArry[i].X, 2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y,FColor);//4
				}
				break;
			}
			case 5:
			{
				for(i=xS;i<=xE;i++)
				{
					LCD_DispDot(2*TFTCircle_Struct->mX - CoordinateStructArry[i].X, 2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y, FColor);//5
				}
				break;
			}
			case 6:
			{
				for(i=xS;i>=xE;i--)
				{
					LCD_DispDot(2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y, 2*TFTCircle_Struct->mX - CoordinateStructArry[i].X,FColor);//6
				}
				break;
			}
			case 7:
			{
				for(i=xS;i<=xE;i++)
				{
					LCD_DispDot(2*TFTCircle_Struct->mY - CoordinateStructArry[i].Y, CoordinateStructArry[i].X, FColor);//7
				}
				break;
			}
			case 8:
			{
				for(i=xS;i>=xE;i--)
				{
					LCD_DispDot(2*TFTCircle_Struct->mX - CoordinateStructArry[i].X, CoordinateStructArry[i].Y, FColor);//8
				}
				break;
			}
			default:
			{
				return;
			}
			
		}
	}
		
	
}



/**
  * @brief   显示任意大小的ASCII字符
  * @param   
  *		@arg XS：字符起点X坐标
  *		@arg YS：字符起点Y坐标
  *		@arg width：字符宽度
  *		@arg height：字符高度
  *		@arg FColor：字符颜色
  *		@arg BColor：字符背景色
  *		@arg ascii：要显示的字符串
  * @retval  
  *注：字符串的内容应在ASCII表0x20-0x7e范围内或是换行符
  */
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
void TFT_Display_XxX_ASCII(uint8_t XS,uint8_t YS, uint8_t width,uint8_t height, uint16_t FColor,uint16_t BColor,char* fmt,...)
{
	uint8_t i,j;
	uint8_t* pChar = ASCII_Font8x12;//根据8*12的字库缩放
	
	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
	va_end(ap);
	
	
	
	
	if(width==8&&height==12)//无需缩放
	{
		do
		{
			pChar = ASCII_Font8x12 + ((*pStr-0x20)*12);//定位字符位置，ASCII前32个字符不显示
			for(i=0;i<12;i++)//y
			{
				for(j=0;j<8;j++)//x
				{
					if((*(pChar+i)) & (0x80>>j))
					{
						LCD_DispDot(j+XS, i+YS,FColor);
					}
					else
					{
						LCD_DispDot(j+XS, i+YS,BColor);
					}
				}	
			}
			XS+=8;
			pStr++;
			while(*pStr==0x0a)//\n
			{XS = 0;YS+=12;pStr++;}
			if(XS>120)
			{
				if(YS>120)
					return;//行列都超范围直接退出
				else
				{
					XS = 0;
					YS += 12;//行超范围直接换行
				}
			}
		}while(*pStr != 0x00);
	}
	else //需要缩放
	{
		
		float X_rate = 80.0/width;
		float Y_rate = 120.0/height; //比例放大10倍
		
		uint8_t X_zoom = (int)X_rate+0.5;
		uint8_t Y_zoom = (int)Y_rate+0.5;//转为整数，注意比例应小于255
		
		uint8_t x=0;
		uint8_t y=0;//缩放后坐标像素在原字库的位置
		
		
		do
		{
			pChar = ASCII_Font8x12 + ((*pStr-0x20)*12);//定位字符位置，ASCII前32个字符不显示
			for(i=0;i<height;i++)//y
			{
				for(j=0;j<width;j++)//x
				{
					y = i*Y_zoom/10;
					x = j*X_zoom/10; //线性插值
					x =  x < width ? x : width  ;
					y =  y < height ? y : height ;
					
					if((*(pChar+y)) & (0x80>>x))
					{
						LCD_DispDot(j+XS, i+YS,FColor);
					}
					else
					{
						LCD_DispDot(j+XS, i+YS,BColor);
					}
				}	
			}
			XS+=width;
			pStr++;
			while(*pStr==0x0a)//\n
			{XS = 0;YS+=height;pStr++;}
			if(XS>120)
			{
				if(YS>120)
					return;//行列都超范围直接退出
				else
				{
					XS = 0;
					YS += height;//行超范围直接换行
				}
			}
		}while(*pStr != 0x00);
	
	}
	
	

}




