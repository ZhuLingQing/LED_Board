#include "Board.h"
#include "RS232.h"
#include "ring_buf.h"

static unsigned char rxBuf[RS232_RX_BUF_SIZE];
static unsigned char txBuf[RS232_TX_BUF_SIZE];

static RING_BUF rs232_rx;
static RING_BUF rs232_tx;

void RS232_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	RBUF_Init(&rs232_rx,rxBuf,sizeof(rxBuf));
	RBUF_Init(&rs232_tx,txBuf,sizeof(txBuf));
    
    /* ��1������GPIO��USARTʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    /* ��2������USART1 Tx@PA9��GPIO����Ϊ���츴��ģʽ */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* ��3������USART1 Rx@PA10��GPIO����Ϊ��������ģʽ */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15 - 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);
	
    /* ��4��������USART1����
    ������   = 9600
    ���ݳ��� = 8
    ֹͣλ   = 1
    У��λ   = No
    ��ֹӲ������(����ֹRTS��CTS)
    ʹ�ܽ��պͷ���
    */
    USART_InitStructure.USART_BaudRate = RS232_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;



    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(USART1, &USART_InitStructure);
    
    
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
    /* ��5����ʹ�� USART1�� ������� */
    USART_Cmd(USART1, ENABLE);
}

void USART1_IRQHandler()  
{  
	unsigned char data;
    if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)  
    {
		data = USART_ReceiveData(USART1);
        RBUF_Push(&rs232_rx,&data,1);
    }
    if(USART_GetITStatus(USART1,USART_IT_TXE) != RESET)  
    {
		if(RBUF_Pop(&rs232_tx,&data,1) == 1)
			USART_SendData(USART1,data);
		if(RBUF_Count(&rs232_tx) == 0)
    		USART_ITConfig(USART1,USART_IT_TXE,DISABLE);
    }
}

void RS232_Send(const unsigned char *buf, unsigned int length)
{
    RBUF_Push(&rs232_tx,buf,length);
    USART_ITConfig(USART1,USART_IT_TXE,ENABLE);
}

unsigned int RS232_Read(unsigned char *buf, unsigned int buf_size)
{
	unsigned int nPop;
	__disable_irq();
    nPop = RBUF_Pop(&rs232_rx,buf,buf_size);
	__enable_irq();
	return nPop;
}

unsigned int RS232_RxSize()
{
	return RBUF_Count(&rs232_rx);
}

void RS232_RxReset()
{
	__disable_irq();
	RBUF_Reset(&rs232_rx);
	__enable_irq();
}