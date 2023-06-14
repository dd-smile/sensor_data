#include "sys.h"

void Fan_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB.7
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//复用推挽输出
		GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.7

    GPIO_SetBits(GPIOB, GPIO_Pin_7);
}
