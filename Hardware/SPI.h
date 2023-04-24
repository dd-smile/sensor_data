#include "stm32f10x.h"                  // Device header
#ifndef __SPI_H__ 
#define __SPI_H__ 

void SPI1_Init(void);
uint8_t SPI_WriteByte(SPI_TypeDef* SPIx,uint8_t Byte);
void SPI_SetSpeed(SPI_TypeDef* SPIx,uint8_t SpeedSet);

#endif 
