#include "stm32f10x.h" 
#ifndef __RING_QUEUE_H__ 
#define __RING_QUEUE_H__ 
#define QUEUESIZE   256    //最多缓存个数
typedef struct
{
	uint16_t Head;		//数据头
	uint16_t Tail;		//数据尾
	uint16_t Length;	//数据长度
	uint8_t Rev_Data[QUEUESIZE];	//缓存区
}ringbuff_t;

uint8_t Queue_Init(ringbuff_t *pQue);		//队列初始化
uint8_t Queue_IfEmpty(ringbuff_t *pQue);		//队列是否为空
uint8_t Queue_IfFull(ringbuff_t *pQue);		//队列是否满了
uint8_t Queue_Put(ringbuff_t *pQue, uint8_t element);	//在队列中加入一个字节数据
uint8_t Queue_Get(ringbuff_t *pQue, uint8_t *element);	//在队列中取出一个字节数据
uint8_t Queue_QueryHead(ringbuff_t *pQue, uint8_t *e);  //获取队头位置
uint8_t Queue_Clear(ringbuff_t *pQue );  	//清空队列
uint16_t Queue_Length(ringbuff_t *pQue);  	//获取当前长度

#endif 
