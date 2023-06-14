#ifndef __FAN_H__ 
#define __FAN_H__ 

#define Fan_ON      GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define Fan_OFF     GPIO_SetBits(GPIOB, GPIO_Pin_7)

#define Fan_GPIO    GPIOB
#define Fan_Pin     GPIO_Pin_7

void Fan_Init(void);

#endif
