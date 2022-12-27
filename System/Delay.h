#include "stm32f10x.h" 
#ifndef __DELAY_H__ 
#define __DELAY_H__ 

void delay_init(void);
void delay_ms(u16 time);
void delay_us(unsigned int us);


#endif 
