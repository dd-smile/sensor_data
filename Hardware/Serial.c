#include "stm32f10x.h"                  // Device header
#include "stm32f10x_usart.h"
#include <stdio.h>
#include <stdarg.h>
#include "Ring_queue.h"
#include "Serial.h"

/*
串口1接zigbee
*/

/*
全局变量
*/
ringbuff_t zigbee_Buff;   //队列结构体声明
uint8_t Serial_RxData;  //串口1接收数据
uint8_t Serial_RxFlag;  //串口1接收标志位

/*
串口2初始化
*/

void Serial_Init(void)
{
	Queue_Init(&zigbee_Buff); //队列初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);  //1.串口时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //使能GPIOA - 复用A9与A10
	
	USART_DeInit(USART1); //2.串口复位
	
	
	//3.GPIO 端口模式设置
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	//PA9是串口1的发送
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//Tx用复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);    //初始化 GPIOA.9

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	//PA10是串口1的接收
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 //Rx用浮空输入或上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);    //初始化 GPIOA.10
	
	
	//4.串口参数初始化
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_Mode =USART_Mode_Rx | USART_Mode_Tx; 	//收发模式
	USART_InitStructure.USART_Parity = USART_Parity_No;	//无校验模式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //字长为 8 位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //无硬件数据流控制
	USART_Init(USART1,&USART_InitStructure);  //初始化串口
	
	//5.初始化 NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设 置 NVIC 中 断 分 组 2
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  //中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //IRQ通道使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;   //子优先级3
	NVIC_Init(&NVIC_InitStructure);  //中断优先级初始化
	
	//6.开启中断
	USART_ITConfig(USART1, USART_IT_RXNE,ENABLE);	 //开启串口接收中断
	
	//7.使能串口
	USART_Cmd(USART1, ENABLE);
	Serial_RxFlag = 0;   //串口1接收标志位初始化
}


/*
串口1发送一个比特
*/
void Serial_SendByte(uint8_t Byte)	
{
	USART_SendData(USART1, Byte);
	//等TDR的数据转移到移位寄存器
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);  //TXE-发送数据寄存器为空
}

/*
串口1发送一个数组
*/
void Serial_SendArray(uint8_t *Array, uint16_t Length)	
{
	uint16_t i;
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(Array[i]);
	}
}

/*
串口1发送一个字符串
*/
void Serial_SendString(uint8_t *String)
{
	uint8_t i;
	for(i=0;String[i]!='\0';i++)
	{
		Serial_SendByte(String[i]);
	}
}

/*
拆分数字,取哪位数字就是除以 10的(哪位-1)次方
Result = X^Y
*/
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result=1;
	while(Y--)
	{
		Result *= X;
	}
	return Result;
}

/*
发送一个数字
*/
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for(i=0; i<Length;i++)
	{
		Serial_SendByte(Number/Serial_Pow(10, Length-i-1) % 10 + '0');  //i从0开始, 第一个数字不是个位, 方向要反过来
	}
		
}

/*
判断串口1是否接收到数据
*/
uint8_t Serial_GetRxFlag(void)		
{
	if(Serial_RxFlag==SET)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

/*
将串口1接收到的数据存在这里
*/
uint8_t Serial_GetData(void)	
{
	return Serial_RxData;
}

/*
串口1 中断
*/
void USART1_IRQHandler(void)
{
	
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		Serial_RxData = USART_ReceiveData(USART1);
		Serial_RxFlag = 1;
		USART_ClearITPendingBit(USART1,USART_IT_RXNE); //标志位清0, 1-有数据, 0-没有数据     RXNE - 接收数据寄存器不为空
		Queue_Put(&zigbee_Buff,Serial_RxData);
	}
}




