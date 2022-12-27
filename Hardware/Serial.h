#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#ifndef __SERIAL_H__ 
#define __SERIAL_H__

void Serial_Init(void);       //串口1初始化
void Serial_SendByte(uint8_t Byte);	//发送一个比特
void Serial_SendArray(uint8_t *Array, uint16_t Length);	//发送一个数组
void Serial_SendString(uint8_t *String);	//发送一个字符串
void Serial_SendNumber(uint32_t Number, uint8_t Length);	//发送一个数字
uint32_t Serial_Pow(uint32_t X, uint32_t Y);
uint8_t Serial_GetRxFlag(void);		//判断是否接收到数据
uint8_t Serial_GetData(void);	    //将接收到的数据存在这里

#endif 
