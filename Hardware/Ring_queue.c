#include "stm32f10x.h"                  // Device header
#include "Ring_queue.h"

uint8_t Queue_Init(ringbuff_t *pQue)	//队列初始化
{
	uint16_t i = 0;
	for(i=0;i<QUEUESIZE;i++) pQue->Rev_Data[i] = 0;
	pQue->Head = 0;
	pQue->Tail = 0;
	pQue->Length = 0;
	return 1;
}

uint8_t Queue_IfEmpty(ringbuff_t *pQue)		//队列是否为空
{
	if(pQue->Length == 0)
		return 1;
	else
		return 0;
}

uint8_t Queue_IfFull(ringbuff_t *pQue)		//队列是否满了
{
	if(pQue->Length == QUEUESIZE)
		return 1;
	else
		return 0;
}

uint8_t Queue_Put(ringbuff_t *pQue, uint8_t element)	//在队列中加入一个字节数据
{
	if(Queue_IfFull(pQue)==1) return 0;		//判断队列是否满了
	pQue->Rev_Data[pQue->Tail] = element;	//将一字节加入队尾
	pQue->Tail = (pQue->Tail+1) % QUEUESIZE;	//队尾位置后移
	pQue->Length++;		//计算长度
	return 1;
}

uint8_t Queue_Get(ringbuff_t *pQue, uint8_t *element)	//在队列中取出一个字节数据
{
	if(Queue_IfEmpty(pQue)==1) return 0;		//判断队列是否空
	*element = pQue->Rev_Data[pQue->Head];		//将一字节从队头取出
	pQue->Head = (pQue->Head+1) % QUEUESIZE;		//队头位置后移
	pQue->Length--;		//计算长度
	return 1;
}

uint8_t Queue_QueryHead(ringbuff_t *pQue, uint8_t *e)  //获取队头位置
{  
	
	if(pQue->Length == 0) return 0;  
	*e = pQue->Rev_Data[pQue->Head];
	return 1;  
}  

uint8_t Queue_Clear(ringbuff_t *pQue )  	//清空队列
{  
	
	pQue->Head = pQue->Tail = 0;  
	pQue->Length = 0;  
	return 1;   
}  

uint16_t Queue_Length(ringbuff_t *pQue)  	//获取当前长度
{  
	return pQue->Length;  
}
