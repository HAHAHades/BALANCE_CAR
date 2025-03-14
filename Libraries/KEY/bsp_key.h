#ifndef _BSP_KEY_H
#define _BSP_KEY_H

#include "stm32f10x.h"


#define MAX_KEY_NUM  3 //最多几个按键  LKEY从1开始编号

#define KEYx_ON 1 //注：按键按下时默认接入高电平

#define KEYx_OFF 0 



void KEY_Register(uint8_t KEY_num, GPIO_TypeDef* KEY_GPIO_POTRx, uint16_t KEY_GPIO_PINx);
_Bool KEY_Scan(uint8_t KEY_num);




#endif /* _BSP_KEY_H */
