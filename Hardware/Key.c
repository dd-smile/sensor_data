#include "stm32f10x.h"// Device header
#include "Delay.h"
#include "Key.h"
#include "LED.h"
#include "Fan.h"
#include "Sg90.h"

extern uint8_t led_state, fan_state, feed_state;
extern uint8_t led_switch;   //引入led开关值

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    //打开GPIO口时钟，先打开复用才能修改是否停用复用功能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
    //关闭JTAG，使能SWD
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	//配置下拉输入
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	//配置下拉输入
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	//配置下拉输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void EXTIx_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
		Key_Init(); //按键端口初始化
	
		EXTI_ClearITPendingBit(EXTI_Line8);     //清除中断标志位   风扇
    EXTI_ClearITPendingBit(EXTI_Line15);     //清除中断标志位   喂食
    EXTI_ClearITPendingBit(EXTI_Line4);     //清除中断标志位   照明
	
    //中断线以及中断初始化配置,下降沿触发       喂食、照明、风扇

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
    EXTI_InitStructure.EXTI_Line=EXTI_Line8;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStructure); //初始化 EXTI 寄存器

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line=EXTI_Line4;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStructure); //初始化 EXTI 寄存器

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
    EXTI_InitStructure.EXTI_Line=EXTI_Line15;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStructure); //初始化 EXTI 寄存器
		
		
		//没用上
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置 NVIC 中断分组 2,其中 2 位抢占优先级，2 位响应优先级
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;      
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;        
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;        
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void EXTI9_5_IRQHandler(void)
{
    delay_ms(10);     //延时，防抖动
	if(KEY_fan !=  RESET)   //判断是否按下
	{
		if(fan_state == 1 && led_switch == 0xff){
			Fan_ON;
			fan_state = 0;
		}	else{
			Fan_OFF;
			fan_state = 1;
		}
        EXTI_ClearITPendingBit(EXTI_Line8); 
	}
}


void EXTI4_IRQHandler(void)
{
    delay_ms(10);     //延时，防抖动
	if(KEY_led !=  RESET)   //判断是否按下
	{
		if(led_state){
			LED2_OFF();
			led_state = 0;
		} else{
			LED2_ON();
			led_state = 1;
		}

        EXTI_ClearITPendingBit(EXTI_Line4); 
	}
    
}

void EXTI15_10_IRQHandler(void)
{
    delay_ms(10);     //延时，防抖动
	if(KEY_feed != RESET)   //判断是否按下
	{
		if(feed_state == 1 && led_switch == 0xff){
			Server_ON;
			feed_state = 0;
		}	else if (feed_state == 0 && led_switch == 0xff){
			Server_OFF;
			feed_state = 1;
		}

        EXTI_ClearITPendingBit(EXTI_Line15); 
	}
}
