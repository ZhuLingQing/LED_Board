#include "test_basic.h"
#include <stdio.h>
#include <assert.h>

#ifdef TRACE_ON

#define DEBUG_BUF_SIZE		(256+64)
char debugBuffer[DEBUG_BUF_SIZE];
char *debugBufPtrIn,*debugBufPtrOut;
volatile int debugBufCount;

void DEBUG_Initialize()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
	debugBufPtrIn = debugBuffer;
	debugBufPtrOut = debugBufPtrIn;
	debugBufCount = 0;
    /* 第1步：打开GPIO和USART时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
#if 0
	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#else
    /* 第2步：将USART1 Tx@PA9的GPIO配置为推挽复用模式 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 第3步：将USART1 Rx@PA10的GPIO配置为浮空输入模式 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
    /* 第4步：配置USART1参数
    波特率   = 9600
    数据长度 = 8
    停止位   = 1
    校验位   = No
    禁止硬件流控(即禁止RTS和CTS)
    使能接收和发送
    */
    USART_InitStructure.USART_BaudRate = 9600;//115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;


    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15 - 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);

    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(USART1, &USART_InitStructure);
    
    
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
    /* 第5步：使能 USART1， 配置完毕 */
    USART_Cmd(USART1, ENABLE);
	
}

void USART1_IRQHandler()  
{  

    if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)  
    {
        *debugBufPtrIn++ = USART_ReceiveData(USART1);
		if(debugBufPtrIn >= debugBuffer + DEBUG_BUF_SIZE)
			debugBufPtrIn = debugBuffer;
        if(debugBufCount < DEBUG_BUF_SIZE)
		{
			debugBufCount++;
		}
		else
		{
			assert(debugBufPtrOut == debugBufPtrIn);
			if(++debugBufPtrOut >= debugBuffer + DEBUG_BUF_SIZE)
				debugBufPtrOut = debugBuffer;
		}
    }
}

int _fputc(int ch, FILE *f)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    /* 写一个字节到USART1 */
    USART_SendData(USART1, (uint8_t) ch);
    
    /* 等待发送结束 */
    return ch;
}

FlagStatus fready()
{
#if 0
	return USART_GetITStatus(USART1,USART_IT_RXNE);
#else
	return debugBufCount?SET:RESET;
#endif
}

int fgetc(FILE *f)
{
#if 0
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    /* 写一个字节到USART1 */
    return USART_ReceiveData(USART1);
#else
	int res;
    while(debugBufCount == 0)
		Sleep(5);
    
	res = *debugBufPtrOut++;
	if(debugBufPtrOut >= debugBuffer + DEBUG_BUF_SIZE)
		debugBufPtrOut = debugBuffer;
	debugBufCount--;
	return res;
#endif
}

#else

#endif



