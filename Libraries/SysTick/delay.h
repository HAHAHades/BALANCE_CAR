#ifndef _DELAY_H
#define _DELAY_H

#include "stm32f10x.h"


void SisTic_Init(void);
void delay_5_nop(void);
void SisTic_Delay_us(uint32_t count);
void SisTic_Delay_ms(uint32_t count);
void delay(uint32_t count);

#endif /*_DELAY_H*/


