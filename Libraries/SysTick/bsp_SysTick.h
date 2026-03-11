#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"


#define SisTic_Delay_ms  SysTick_delay
#define delay_ms  SysTick_delay



int get_tick_count(unsigned long *count);
void SysTick_delay(unsigned long nTime);
void SysTick_Init(void);
void TimingDelay_Decrement(void);
void TimeStamp_Increment(void);
void delay_5_nop(void);
#endif /* __SYSTICK_H */
