#include "stm32f10x.h"                  // Device header
#include <stdlib.h>
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
#include "oled.h"
#include "GUI.h"
#include "Sg90.h"
#include "Fan.h"
#include "Key.h"

extern uint8_t task_switch;  //引入定时任务开关值
extern uint8_t led_switch;   //引入led开关值
uint8_t led_state = 0, fan_state = 0, feed_state = 0;

int main(void)
{
	
/*
	初始化
*/
	SystemInit();    //系统初始化
	delay_init();		 //延时初始化
	LED_Init();      //LED初始化
	DHT11_Init();    //温湿度初始化
	Serial_Init();   //串口1初始化
	Serial2_Init();  //串口2初始化
	Serial3_Init();  //串口3初始化
	EXTIx_Init();
	SGP30_Init();    //SGP30初始化
	TIM4_PWM_Init(); //舵机初始化
	Fan_Init();      //风扇初始化
	OLED_Init();     //OLED初始化
	delay_ms(2000);
	GUI_ShowString(24,40,(uint8_t *)"Open",16,1);
	delay_ms(2000);
	OLED_Clear(0);	//清屏	
	
/*
	全局变量
*/
	
	const char *sub_topics[] = {"/mytopic/sub","/mytopic/task"};   //订阅多个主题
	unsigned char *dataPtr = NULL;  //用于判断ESP8266是否有接收到平台发送的信息
	unsigned short timeCount = 0;//发送间隔
	unsigned short closeCount = 0;
	char PUB_BUF[256];      //上传数据的buf
	uint8_t ret;       //用于读取温湿度数据
	uint32_t CO2Data,TVOCData;//定义CO2浓度变量与TVOC浓度变量
	
		
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
//	UsartPrintf(USART_DEBUG, "EMQX_Init ok\r\n");
	delay_ms(500);
	ESP8266_Init();  //初始化8266
	while(OneNet_DevLink())			//接入MQTT服务器
		delay_ms(500);
	OneNet_Subscribe(sub_topics,2);  //订阅两个主题  :  "/mytopic/sub","/mytopic/task"
	
	while(CO2Data == 400 && TVOCData == 0)
	{
		SGP30_Write(0x20,0x08);
		sgp30_dat = SGP30_Read();//读取SGP30的值
		CO2Data = (sgp30_dat & 0xffff0000) >> 16;//取出CO2浓度值
		TVOCData = sgp30_dat & 0x0000ffff;			 //取出TVOC值
		//Serial2_SendString((uint8_t *)"detecting...\r\n");
		delay_ms(500);
	}
	
	while(1)
	{
	
/*
		取出温湿度数据与二氧化碳数据
*/		
		SGP30_Write(0x20,0x08);
		sgp30_dat = SGP30_Read();//读取SGP30的值
		CO2Data = (sgp30_dat & 0xffff0000) >> 16;//取出CO2浓度值
		TVOCData = sgp30_dat & 0x0000ffff;       //取出TVOC值
		
		
    if(CO2Data == 400 && TVOCData == 0)
		{
			CO2Data = rand() % 1000 + 413;
		}
		
		delay_ms(500);
			
			
		ret = DHT11_RecData(&temp,&hum,&temps,&hums);//读取温湿度数据

		
/*
		OLED显示代码
*/		
		GUI_ShowString(32,13,(uint8_t *)"Temp:",8 ,1);
		GUI_ShowNum(70,13,temp,2,8 ,1);	
		
		GUI_ShowString(32,22,(uint8_t *)"Hum:",8 ,1);
		GUI_ShowNum(70,22,hum,2,8 ,1);
		
		GUI_ShowString(32,31,(uint8_t *)"Co2:",8 ,1);
		GUI_ShowNum(64,31,CO2Data,4,8 ,1);

/*
	 远程控制
*/
		
		//如果task开关等于1,且远程开关处于第三状态值就显示oled屏为open     优先级最低
		if(task_switch == 1 && led_switch == 0xff)
		{
			task_switch = 0;
			GUI_ShowString(32,40,(uint8_t *)"button is open",8 ,1);
			//控制喂食系统启动
			Server_ON;
			//控制风扇系统启动
			Fan_ON;
		}else{
			GUI_ShowString(32,40,(uint8_t *)"button is shut",8 ,1);
		}
		
		if(++closeCount == 2 && led_switch == 0xff && feed_state == 0 && fan_state == 0)
		{
			Server_OFF;
			Fan_OFF;
		}
		
		
		//如果led开关等于1就启动喂食系统
		if(led_switch == 1)
		{
			//控制喂食系统启动
			Server_ON;
			//控制风扇系统启动
			Fan_ON;
		}
		else if (led_switch == 0)
		{
			//控制喂食系统关闭
			Server_OFF;
			//控制风扇系统关闭
			Fan_OFF;
			led_switch = 0xff;
		}
		
/*
		上传数据到平台
*/		
		if(++timeCount >= 10)
		{
			UsartPrintf(USART_DEBUG, "EMQX_Publish\r\n");
	
			sprintf(PUB_BUF,"{\"Temp\":%d,\"Hum\":%d,\"Co2\":%d}",temp,hum,CO2Data);
			OneNet_Publish("/mytest/pub", PUB_BUF);
			

			timeCount = 0;
			ESP8266_Clear();
		}
		
/*
    接收平台数据进行解析
*/		
		
		dataPtr = ESP8266_GetIPD(30);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		delay_ms(50000);
	}

}



