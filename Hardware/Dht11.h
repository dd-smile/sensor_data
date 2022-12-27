#include "stm32f10x.h"                  // Device header
#ifndef __DHT11_H__ 
#define __DHT11_H__ 

void DHT11_Init(void);
uint8_t DHT11_RecData(uint8_t *temp,uint8_t *hum,uint8_t *temps,uint8_t *hums);	

#endif 
