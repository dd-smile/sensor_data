#include "stm32f10x.h"// Device header
#include "Delay.h"
#include "Key.h"

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOA,ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//配置上拉输入 
	GPIO_Init(GPIOE,&GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	//配置下拉输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
}

uint8_t Key_scan(void)
{
    static unsigned char key_up=1;
    if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
    {
        delay_ms(10);
        key_up=0;
        if(KEY0==0) return KEY0_PRES; // 0建按下
        else if(KEY1==0) return KEY1_PRES; // 1建按下
        else if(WK_UP==1) return WKUP_PRES; // 2建按下
    }else if(KEY0==1&&KEY1==1&&WK_UP==0) key_up=1;
    return 0;
}
