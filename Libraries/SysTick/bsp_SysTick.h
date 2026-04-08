#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"


#define SisTic_Delay_ms  SysTick_delay
#define delay_ms  SysTick_delay



uint32_t get_tick_count(uint32_t* count);
void SysTick_delay(unsigned long nTime);
void SysTick_Init(void);
void TimingDelay_Decrement(void);
void TimeStamp_Increment(void);
void delay_us(uint32_t us);
void delay_5_nop(void);
#endif /* __SYSTICK_H */
