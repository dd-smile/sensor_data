#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
void DHT11_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void DHT11_Pin_Mode(GPIOMode_TypeDef mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void DHT11_Start(void)
{
	DHT11_Pin_Mode(GPIO_Mode_Out_PP);
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);		//主机拉低20ms作起始信号
	delay_ms(20);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);		//主机释放总线
	delay_us(13);
}

uint8_t DHT11_Check(void)
{
	uint8_t retry=0;
	DHT11_Pin_Mode(GPIO_Mode_IN_FLOATING);
	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)&&retry<100)	  //等待传感器拉低信号
	{
		retry++;
		delay_us(1);
	}
	if(retry>=100) return 0;
	else retry=0;
	while(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)&&retry<100)	//等待传感器拉高信号
	{
		retry++;
		delay_us(1);
	}
	if(retry>=100)return 0;
	else retry=0;
	
	return 1;
}

uint8_t DHT11_RecBit(void)
{
	uint8_t retry=0;
	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)&&retry<100)	  //等待数据位的54us低电平
	{
		retry++;
		delay_us(1);
	}
	retry = 0;
	while(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)&&retry<100)	//等待数据位的高电平
	{
		retry++;
		delay_us(1);
	}
	retry = 0;
	delay_us(40);
	
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))
		return 1;
	else
		return 0;
}

uint8_t DHT11_RecByte(void)
{
	uint8_t i,dat=0;
	for(i=0;i<8;i++)
	{
		dat<<=1;
		dat|=DHT11_RecBit();
	}
	return dat;
}

/*
temp温度整数, temps温度小数, hum湿度整数, hums湿度小数

temp - 存储单元中的数据, &temp - 存储单元的地址, *temp - 另一个存储单元中的数据
int a = 10;
int *temp = &a;
*/
uint8_t DHT11_RecData(uint8_t *temp,uint8_t *hum,uint8_t *temps,uint8_t *hums)
{
	uint8_t i;
	uint8_t buff[5];
	DHT11_Start();
	if(DHT11_Check())
	{
		for(i=0;i<5;i++)
		{
			buff[i] = DHT11_RecByte();
		}
		/*
		检验和=8bit湿度整数+8bit湿度小数+8bit温度整数+8bit温度小数
		*/
		if(buff[0]+buff[1]+buff[2]+buff[3]==buff[4])
		{
			*temp = buff[2];
			*temps = buff[3];
			*hum = buff[0];
			*hums = buff[1];
		}
	}else return 0;
	
	return 1;
}
