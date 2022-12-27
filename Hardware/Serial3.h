#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#ifndef __SERIAL3_H__ 
#define __SERIAL3_H__

void Serial3_Init(void);      //串口3初始化
void Serial3_SendByte(uint8_t Byte);//发送一个比特
void Serial3_SendArray(uint8_t *Array, uint16_t Length);//发送一个数组
void Serial3_SendString(uint8_t *String);//发送一个字符串
void Serial_SendNumber(uint32_t Number, uint8_t Length);	//发送一个数字
uint8_t Get_Byte_WiFi(uint8_t *dat);	//从WiFi缓存中获取一个字节

void Usart3_SendString(unsigned char *str, unsigned short len);

#endif 
