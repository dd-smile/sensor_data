#include "stm32f10x.h"                  // Device header
#include "stm32f10x_usart.h"
#include <stdio.h>
#include "Ring_queue.h"
#include <stdarg.h>
#include "Serial3.h"
#include "string.h"	
#include "Serial2.h"

/*
串口3接esp8266
*/

/*
全局变量
*/
ringbuff_t WiFibuff; 		//发送WIFI数据缓存
uint8_t Serial3_RxFlag;  //串口3接收标志位
vu16 USART3_RX_STA = 0;   //接收数据状态

/*
串口3初始化
*/
void Serial3_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);  //开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //开启GPIOA时钟, 因为A8在GPIOA上, 复用需要开启GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //开启GPIOB时钟, 因为USART3在GPIOB上, 复用需要开启GPIOB时钟
	USART_DeInit(USART3);
	
	//EN接A7需要置一
	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOA,GPIO_Pin_7);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	//PB10是串口3的发送
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//Tx用复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);    //初始化 GPIOB.10

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	//PB11是串口3的接收
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 //Rx用浮空输入或上拉输入   串口波形空闲状态是高电平, 所以不使用下拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);    //初始化 GPIOB.11
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_Mode =USART_Mode_Rx | USART_Mode_Tx; 	//收发模式
	USART_InitStructure.USART_Parity = USART_Parity_No;	//是否\何种校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //字长为 8 位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //无硬件数据流控制
	USART_Init(USART3,&USART_InitStructure);  //初始化串口
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设 置 NVIC 中 断 分 组 2
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;  //中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //IRQ通道使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;   //子优先级1
	NVIC_Init(&NVIC_InitStructure);  //中断优先级初始化
	
	USART_ITConfig(USART3, USART_IT_RXNE,ENABLE);	 //开启串口中断
	
	USART_Cmd(USART3, ENABLE);
	
	Queue_Init(&WiFibuff);       //初始化循环队列
}

/*
串口3发送一个比特
*/
void Serial3_SendByte(uint8_t Byte)	
{
	USART_SendData(USART3, Byte);
	//等TDR的数据转移到移位寄存器
	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);  //TXE-发送数据寄存器为空
}

/*
串口3发送一个数组
*/
void Serial3_SendArray(uint8_t *Array, uint16_t Length)	
{
	uint16_t i;
	for(i=0;i<Length;i++)
	{
		Serial3_SendByte(Array[i]);
	}
}

/*
串口3发送一个字符串
*/
void Serial3_SendString(uint8_t *String)
{
	uint8_t i;
	for(i=0;String[i]!='\0';i++)
	{
		Serial3_SendByte(String[i]);
	}
}

uint32_t Serial3_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result=1;
	while(Y--)
	{
		Result *= X;
	}
	return Result;
}
/*
/发送一个数字
*/
void Serial3_SendNumber(uint32_t Number, uint8_t Length)	
{
	uint8_t i;
	for(i=0; i<Length;i++)
	{
		Serial3_SendByte(Number/Serial3_Pow(10, Length-i-1) % 10 + '0');
	}
}

/*
从WiFi缓存中获取一个字节
*/
uint8_t Get_Byte_WiFi(uint8_t *dat)	
{
	return(Queue_Get(&WiFibuff,dat));
}


void Usart3_SendString(unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USART3, *str++);									//发送数据
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);		//等待发送完成
	}

}


/*
串口3 中断
*/
//uint8_t i = 0;
//void USART3_IRQHandler(void)
//{
//	uint8_t res;

//	if(USART_GetITStatus(USART3,USART_IT_RXNE)==SET)
//	{
//		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
//		res = USART_ReceiveData(USART3);
//		Serial2_SendString(&res);
//		Queue_Put(&WiFibuff,res); //在WiFi缓存中加入一个字节数据
//	}
//}


