#ifndef _ESP8266_H_
#define _ESP8266_H_



#define COM_USART USART1 //通信串口

#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志


void ESP8266_Init(void);

void ESP8266_Clear(void);

void ESP8266_SendData(unsigned char *data, unsigned short len);

unsigned char *ESP8266_GetIPD(unsigned short timeOut);
void          v_esp8266_c_COM_USART_IRQHandler(void);

#endif
