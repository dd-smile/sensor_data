#include "stm32f10x.h" 
#ifndef __KEY_H__ 
#define __KEY_H__
#define KEY0 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)// 读取按键 0
#define KEY1 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)// 读取按键 1
#define WK_UP GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)// 读取按键 WK_UP
#define KEY0_PRES 1 //1按下
#define KEY1_PRES 2
#define WKUP_PRES 3

void Key_Init(void);
uint8_t Key_scan(void);

#endif 
