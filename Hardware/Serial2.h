#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#ifndef __SERIAL2_H__ 
#define __SERIAL2_H__


//串口2-USART2
#define USART_DEBUG     						USART2             
#define DEBUG_USARTx			          USART2
#define DEBUG_USART_CLK		          RCC_APB1Periph_USART2  //串口时钟

//USART GPIO 引脚宏定义
#define DEBUG_USART_GPIO_CLK        RCC_APB2Periph_GPIOA

#define DEBUG_USART_TX_GPIO_PORT    GPIOA
#define DEBUG_USART_TX_GPIO_PIN     GPIO_Pin_2
#define DEBUG_USART_RX_GPIO_PORT    GPIOA
#define DEBUG_USART_RX_GPIO_PIN     GPIO_Pin_3

#define DEBUG_USART_IRQ             USART2_IRQn     //中断通道


void Serial2_Init(void);       //串口2初始化
void Serial2_SendByte(uint8_t Byte);	//发送一个比特
void Serial2_SendArray(uint8_t *Array, uint16_t Length);	//发送一个数组
void Serial2_SendString(uint8_t *String);	//发送一个字符串
void Serial2_SendNumber(uint32_t Number, uint8_t Length);	//发送一个数字
uint32_t Serial2_Pow(uint32_t X, uint32_t Y);
uint8_t Serial2_GetRxFlag(void);		//判断是否接收到数据
uint8_t Serial2_GetData(void);	    //将接收到的数据存在这里
void u32tostr(unsigned long dat,char *str);
unsigned long strtou32(char *str); 

void Usart2_SendString(unsigned char *str, unsigned short len);
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);


#endif 
