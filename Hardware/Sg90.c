#include "sys.h"

void TIM4_PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  //定义GPIO初始化结构体
 	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; //定义定时器初始化结构体
	TIM_OCInitTypeDef  TIM_OCInitStructure;	//定义定时器输出PWM结构体
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	// 开启定时器时钟,即内部时钟CK_INT=72M
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   //开复用时钟
//	GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);   //选择部分重映射

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置输出速度
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB_Pin_9
//	GPIO_SetBits(GPIOB,GPIO_Pin_9);
	
	 
	TIM_TimeBaseStructure.TIM_Period= (200-1);							 //自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Prescaler= (7200-1);					 //驱动CNT计数器的时钟 = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		 //时钟分频因子 ，用于配置死区时间，没用到，随意
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//计数器计数模式，设置为向上计数
//	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;					 //重复计数器的值，没用到，可以随意设置
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);					 //初始化定时器
	
	//配置PWM
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;						 //配置为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;					//设置占空比大小，在主函数compare中又设置一遍
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		 //输出通道电平极性配置
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;	 //输出通道空闲电平极性配置
				   						 //初始化通道1
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);						 //使能通道1输出
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_Cmd(TIM4, ENABLE);						//使能计数器
//	TIM_CtrlPWMOutputs(TIM8, ENABLE);	// 主输出使能，当使用的是通用定时器时，这句不需要
}
