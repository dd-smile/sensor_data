#ifndef __SG90_H__ 
#define __SG90_H__ 

#define Server_ON      TIM_SetCompare4(TIM4,15)
#define Server_OFF     TIM_SetCompare4(TIM4,5)  //0度

void TIM4_PWM_Init(void);

#endif
