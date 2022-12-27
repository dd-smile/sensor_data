#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "delay.h"
#include "Dht11.h"
#include "Serial2.h"
#include "esp8266.h"
#include "Serial3.h"
#include "Serial.h"
#include "SGP30.h"
#include "onenet.h"
#include <math.h>

/*
定义温湿度
*/
typedef struct Date{
	uint32_t temp;
	uint32_t hum;
	uint32_t CO2Data;
}date_t;


int main(void)
{
	
/*
	全局变量
*/
	
//	const char *sub_topics[] = {"/mytest/sub"};
//	const char *pub_topics[] = {"/mytest/pub"};
	unsigned char *dataPtr = NULL;
	char PUB_BUF[256];      //上传数据的buf
//	uint32_t test_date[4];  //发送温度,湿度,CO2与一个校验位
	uint8_t ret;
//	uint8_t RxDate;
	uint32_t CO2Data,TVOCData;//定义CO2浓度变量与TVOC浓度变量

/*
	初始化
*/
	delay_init();
	LED_Init();
	DHT11_Init();
	Serial_Init();
	Serial2_Init();
	Serial3_Init();
	SGP30_Init();   //初始化SGP30
	
/*
	LED控制
*/
	
	LED2_ON();
	delay_ms(1000);
	LED2_OFF();
	delay_ms(1000);
		
	
/*
数据上云测试
*/
	uint8_t temp,hum,temps,hums; 
	uint32_t sgp30_dat;
	SGP30_Write(0x20,0x08);
	sgp30_dat = SGP30_Read();//读取SGP30的值
	CO2Data = (sgp30_dat & 0xffff0000) >> 16;
	TVOCData = sgp30_dat & 0x0000ffff;	
	//SGP30模块开机需要一定时间初始化，在初始化阶段读取的CO2浓度为400ppm，TVOC为0ppd且恒定不变，因此上电后每隔一段时间读取一次
	//SGP30模块的值，如果CO2浓度为400ppm，TVOC为0ppd，发送“正在检测中...”，直到SGP30模块初始化完成。
	//初始化完成后刚开始读出数据会波动比较大，属于正常现象，一段时间后会逐渐趋于稳定。
	//气体类传感器比较容易受环境影响，测量数据出现波动是正常的，可自行添加滤波函数。
	
		//emqx
	UsartPrintf(USART_DEBUG, "EMQX_Publish\r\n");
	delay_ms(500);
	ESP8266_Init();  //初始化8266
	while(OneNet_DevLink())			//接入OneNET
		delay_ms(500);
	//OneNet_Subscribe(sub_topics,1);
	
	while(CO2Data == 400 && TVOCData == 0)
	{
		SGP30_Write(0x20,0x08);
		sgp30_dat = SGP30_Read();//读取SGP30的值
		CO2Data = (sgp30_dat & 0xffff0000) >> 16;//取出CO2浓度值
		TVOCData = sgp30_dat & 0x0000ffff;			 //取出TVOC值
		Serial2_SendString((uint8_t *)"detecting...\r\n");
		delay_ms(500);
	}
	while(1)
	{
		
		SGP30_Write(0x20,0x08);
		sgp30_dat = SGP30_Read();//读取SGP30的值
		CO2Data = (sgp30_dat & 0xffff0000) >> 16;//取出CO2浓度值
		TVOCData = sgp30_dat & 0x0000ffff;       //取出TVOC值
		
		
		delay_ms(500);
			
			
		ret = DHT11_RecData(&temp,&hum,&temps,&hums);//读取温湿度数据
		
		
		UsartPrintf(USART_DEBUG, "EMQX_Publish\r\n");
		delay_ms(50000);
		delay_ms(50000);
		delay_ms(50000);
		delay_ms(50000);
		delay_ms(50000);
		sprintf(PUB_BUF,"{\"Temp\":%d,\"Hum\":%d,\"Co2\":%d}",temp,hum,CO2Data);
		OneNet_Publish("/mytest/pub", PUB_BUF);
		
		ESP8266_Clear();
		
		dataPtr = ESP8266_GetIPD(3);
		if(dataPtr != NULL)
		OneNet_RevPro(dataPtr);
	
		delay_ms(10);
			

	}
		
	
	
	
	
	/*
	判断串口二是否发送与接收
*/

//	while(1)
//	{
//		//Serial2_SendByte('a');
//		delay_ms(1000);
//		int i = (int)Serial2_GetRxFlag();
//		if(i == 1)
//		{
//			Serial2_SendByte('a');
//			RxDate = Serial2_GetData();
//			Serial2_SendString(&RxDate);
//		}
//	}


/*
测试串口3
*/
//UsartPrintf(USART_DEBUG, "EMQX_Publish\r\n");
//while(1)
//{
//	delay_ms(5000);
//	ESP8266_SendCmd("AT\r\n", "OK");
//	//Serial3_SendString((uint8_t *)"AT\r\n"); 
//}



/*
	判断温湿度是否接收
*/

//	for(int i = 0; i < 5; i++)
//		{
//			Serial_SendByte('b');    
//			delay_ms(1000);
//		}
//	char r[80] = {0};
//	while(1)
//	{
//		
//		uint8_t temp,hum,temps,hums; 
//		
//		ret = DHT11_RecData(&temp,&hum,&temps,&hums);
//		if(ret == 1)
//		{
//			//u32tostr(temp,r);
//			//printf("\r\n%s\r\n",r);
//			Serial2_SendNumber(temp,2);  
//			LED2_ON();
//		}else{
//			LED2_OFF();
//		}
//	}

}



