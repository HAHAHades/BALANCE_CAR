#include "delay.h" 


/*
delay(0x2aaaa);//大约100ms
*/
void delay(uint32_t count)
{
	for(;count!=0;count--);
}


/*大约50ns*/
void delay_5_nop(void)
{
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
}


void SisTic_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//选择SysTick时钟源
	
	SysTick->LOAD = (72 & SysTick_LOAD_RELOAD_Msk) - 1;//1us
	SysTick->VAL   = 0;//当前值复位
	SysTick->CTRL &= ~1;//关闭计数器
}


void SisTic_Delay_us(uint32_t count)
{
	SysTick->LOAD = (72 & SysTick_LOAD_RELOAD_Msk) - 1;
	SysTick->VAL   = 0;//当前值复位
	SysTick->CTRL |= 0x01;//开启计数器
	do
	{
		count--;
		while(!(SysTick->CTRL&1<<16));
	}while(count);
	SysTick->CTRL &= ~0x000001;//关闭计数器
}

void SisTic_Delay_ms(uint32_t count)
{
	SysTick->LOAD = (72000 & SysTick_LOAD_RELOAD_Msk) - 1;//1ms
	SysTick->VAL   = 0;//当前值复位
	SysTick->CTRL |= 0x01;//开启计数器
	do
	{
		count--;
		while(!(SysTick->CTRL&1<<16));
	}while(count);
	SysTick->CTRL &= ~0x000001;//关闭计数器
	
	
}


