#include "stm32f10x.h"                  // Device header
#include "stm32f10x_usart.h"
#include <stdio.h>
#include <stdarg.h>
#include "Ring_queue.h"
#include "Serial2.h"
#include <string.h>

/*
串口2接usb转TTL
*/

/*
全局变量
*/
ringbuff_t ring_Queue;   //队列结构体声明
uint8_t Serial2_RxData;  //串口2接收数据
uint8_t Serial2_RxFlag;  //串口2接收标志位

/*
串口2初始化
*/

void Serial2_Init(void)
{
	Queue_Init(&ring_Queue); //队列初始化
	RCC_APB1PeriphClockCmd(DEBUG_USART_CLK,ENABLE);  //1.串口时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //使能GPIOA - 复用A2与A3
	
	USART_DeInit(DEBUG_USARTx); //2.串口复位
	
	
	//3.GPIO 端口模式设置
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;	//PA2是串口2的发送
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//Tx用复用推挽输出
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);    //初始化 GPIOA.2

	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;	//PA3是串口2的接收
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 //Rx用浮空输入或上拉输入
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);    //初始化 GPIOA.3
	
	
	//4.串口参数初始化
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_Mode =USART_Mode_Rx | USART_Mode_Tx; 	//收发模式
	USART_InitStructure.USART_Parity = USART_Parity_No;	//无校验模式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //字长为 8 位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //无硬件数据流控制
	USART_Init(DEBUG_USARTx,&USART_InitStructure);  //初始化串口
	
	//5.初始化 NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设 置 NVIC 中 断 分 组 2
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;  //中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //IRQ通道使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;   //子优先级3
	NVIC_Init(&NVIC_InitStructure);  //中断优先级初始化
	
	//6.开启中断
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE,ENABLE);	 //开启串口接收中断
	
	//7.使能串口
	USART_Cmd(DEBUG_USARTx, ENABLE);
	Serial2_RxFlag = 0;   //串口2接收标志位初始化
}


/*
串口2发送一个比特
*/
void Serial2_SendByte(uint8_t Byte)	
{
	USART_SendData(DEBUG_USARTx, Byte);
	//等TDR的数据转移到移位寄存器
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);  //TXE-发送数据寄存器为空
}

/*
串口2发送一个数组
*/
void Serial2_SendArray(uint8_t *Array, uint16_t Length)	
{
	uint16_t i;
	for(i=0;i<Length;i++)
	{
		Serial2_SendByte(Array[i]);
	}
}

/*
串口2发送一个字符串
*/
void Serial2_SendString(uint8_t *String)
{
	uint8_t i;
	for(i=0;String[i]!='\0';i++)
	{
		Serial2_SendByte(String[i]);
	}
}

/*
拆分数字,取哪位数字就是除以 10的(哪位-1)次方
Result = X^Y
*/
uint32_t Serial2_Pow(uint32_t X, uint32_t Y)
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
void Serial2_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for(i=0; i<Length;i++)
	{
		Serial2_SendByte(Number/Serial2_Pow(10, Length-i-1) % 10 + '0');  //i从0开始, 第一个数字不是个位, 方向要反过来
	}
		
}

/*
转换数字
将一个32位的变量dat转为字符串,
dat:带转的long型的变量
str:指向字符数组的指针，转换后的字节串放在其中
*/
void u32tostr(unsigned long dat,char *str) 
{
	char temp[20];
	unsigned char i=0,j=0;
	i=0;
	while(dat)
	{
			temp[i]=dat%10+0x30;
			i++;
			dat/=10;
	}
	j=i;
	for(i=0;i<j;i++)
	{
				str[i]=temp[j-i-1];
	}
	if(!i) {str[i++]='0';}
	str[i]=0;
}

/*
将一个字符串转为32位的变量，比如"1234"转为1234
str:指向待转换的字符串     
*/
unsigned long strtou32(char *str) 
{
    unsigned long temp=0;
    unsigned long fact=1;
    unsigned char len=strlen(str);
    unsigned char i;
    for(i=len;i>0;i--)
    {
        temp+=((str[i-1]-0x30)*fact);
        fact*=10;
    }
    return temp;
}

/*
判断串口2是否接收到数据
*/
uint8_t Serial2_GetRxFlag(void)		
{
	if(Serial2_RxFlag==SET)
	{
		Serial2_RxFlag = 0;
		return 1;
	}
	return 0;
}

/*
将串口2接收到的数据存在这里
*/
uint8_t Serial2_GetData(void)	
{
	return Serial2_RxData;
}

/*
串口2 中断
*/
void USART2_IRQHandler(void)
{
	
	if(USART_GetITStatus(DEBUG_USARTx,USART_IT_RXNE)==SET)
	{
		Serial2_RxData = USART_ReceiveData(DEBUG_USARTx);
		Serial2_RxFlag = 1;
		USART_ClearITPendingBit(DEBUG_USARTx,USART_IT_RXNE); //标志位清0, 1-有数据, 0-没有数据     RXNE - 接收数据寄存器不为空
		Queue_Put(&ring_Queue,Serial2_RxData);
	}
}

//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
    USART2->DR = (u8) ch;      
	return ch;
}


/*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Usart2_SendString(unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USART2, *str++);									//发送数据
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);		//等待发送完成
	}

}

/*
************************************************************
*	函数名称：	UsartPrintf
*
*	函数功能：	格式化打印
*
*	入口参数：	USARTx：串口组
*				fmt：不定长参
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{

	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
	va_end(ap);
	
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	}

}







