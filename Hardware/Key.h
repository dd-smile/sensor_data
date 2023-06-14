#include "stm32f10x.h" 
#ifndef __KEY_H__ 
#define __KEY_H__
#define KEY_fan    EXTI_GetITStatus(EXTI_Line8)//GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)// 检测按键状态
#define KEY_led    EXTI_GetITStatus(EXTI_Line4)//GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
#define KEY_feed   EXTI_GetITStatus(EXTI_Line15)//GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15) //EXTI_GetITStatus(EXTI_Line4)

void Key_Init(void);
void EXTIx_Init(void);

#endif 
