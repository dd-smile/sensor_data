#include "Device/Include/stm32f10x.h"   // Device header
#ifndef __ESP8266_H
#define __ESP8266_H


#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志


void ESP8266_Init(void);

void ESP8266_Clear(void);

void ESP8266_SendData(unsigned char *data, unsigned short len);

unsigned char *ESP8266_GetIPD(unsigned short timeOut);

_Bool ESP8266_SendCmd(char *cmd, char *res);



#endif
