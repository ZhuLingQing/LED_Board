#include "Board.h"
#include "CAN.h"
#include "ring_buf.h"

static unsigned char rxBuf[CAN_RX_BUF_SIZE];
static unsigned char txBuf[CAN_TX_BUF_SIZE];

static RING_BUF can_rx;
static RING_BUF can_tx;

//波特率=APB1Clock/(1+CAN_BS1+CAN_BS2)/CAN_Prescaler
void CANBUS_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure; 
	CAN_InitTypeDef CAN_InitStructure; 
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	RCC_ClocksTypeDef RCC_ClocksStatus;
	
	RBUF_Init(&can_rx,rxBuf,sizeof(rxBuf));
	RBUF_Init(&can_tx,txBuf,sizeof(txBuf));

	/* Configure CAN pin: RX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure CAN pin: TX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    //GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
  	RCC_GetClocksFreq(&RCC_ClocksStatus);
	
	CAN_DeInit(CAN1); //将外设CAN的全部寄存器重设为缺省值 
	CAN_StructInit(&CAN_InitStructure); //把CAN_InitStruct中的每一个参数按缺省值填入 
	/* CAN cell init */ 
	CAN_InitStructure.CAN_TTCM=DISABLE; //没有使能时间触发模式 
	CAN_InitStructure.CAN_ABOM=DISABLE; //没有使能自动离线管理 
	CAN_InitStructure.CAN_AWUM=DISABLE; //没有使能自动唤醒模式 
	CAN_InitStructure.CAN_NART=DISABLE; //没有使能非自动重传模式 
	CAN_InitStructure.CAN_RFLM=DISABLE; //没有使能接收FIFO锁定模式 
	CAN_InitStructure.CAN_TXFP=DISABLE; //没有使能发送FIFO优先级 
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal; //CAN设置为正常模式 
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq; //重新同步跳跃宽度1个时间单位 
	CAN_InitStructure.CAN_BS1=CAN_BS1_7tq; //时间段1为3个时间单位 
	CAN_InitStructure.CAN_BS2=CAN_BS2_7tq; //时间段2为2个时间单位 
	//CAN_InitStructure.CAN_Prescaler=24; //时间单位长度为60 	
    CAN_InitStructure.CAN_Prescaler = RCC_ClocksStatus.PCLK1_Frequency/(1 + 7 + 7)/CAN_BAUDRATE;
	CAN_Init(CAN1,&CAN_InitStructure);
	
	// CAN filter init 过滤器，注意，只接收主机发过来的数据，屏蔽其他数据 
	CAN_FilterInitStructure.CAN_FilterNumber=1; 	//指定过滤器为1 
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	//指定过滤器为标识符屏蔽位模式 
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //过滤器位宽为32位 
	
	//CAN_FilterInitStructure.CAN_FilterIdHigh= (((u32)0x1314<<3)&0xFFFF0000)>>16;  
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;//((0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要过滤的ID低位 
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;//0xFFFF; //过滤器屏蔽标识符的高16位值 
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;//0xFFFF; //过滤器屏蔽标识符的低16位值 
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0; //设定了指向过滤器的FIFO为0 
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //使能过滤器 
	CAN_FilterInit(&CAN_FilterInitStructure); //按上面的参数初始化过滤器 
	
	/* CAN FIFO0 message pending interrupt enable */ 
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE); //使能FIFO0消息挂号中断
	
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;   
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;             //CAN1_RX0中断向量表中开启
    NVIC_Init(&NVIC_InitStructure);

}
//CAN发送帧构成
void CAN_TxMessageInit(CanTxMsg* _can_msg_tx, unsigned int std_id, unsigned int ext_id, unsigned char ide, unsigned char rtr, unsigned char dlc, const unsigned char *pdata)
{
    unsigned char i;
    _can_msg_tx->StdId = std_id&0x7ff; //设定标准标识符0~0x7ff  11位
    _can_msg_tx->ExtId = ext_id&0x3ffff; //设定额外标识符0~0x3ffff 18位
    _can_msg_tx->IDE = ide; //输出标识符类型,STD(标准标识符)或EXT(额外标识符)
    _can_msg_tx->RTR = rtr; //输出帧类型,DATA(数据帧)或者REMOTE(远程帧)
    _can_msg_tx->DLC = dlc; //帧长度，0~8
	for(i=0; i<dlc; i++)
	{
		_can_msg_tx->Data[i] = *(pdata+i);
	}
}

unsigned char rdata[8];
unsigned char tdata[8] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
//CAN中断接收函数
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CanRxMsg _can_msg_rx;
	ITStatus Status;

	Status = CAN_GetITStatus(CAN1, CAN_IT_FMP0);             //判断接受到过滤器中断信号

	if(Status == SET)
	{
		CAN_Receive(CAN1, CAN_FIFO0, &_can_msg_rx);        //接收一个CAN报文
#ifdef CAN_RX_STD_ID
		if(_can_msg_rx.StdId != CAN_RX_STD_ID)
			goto Eofunc;
#endif
#ifdef CAN_RX_EXT_ID
		if(_can_msg_rx.ExtId != CAN_RX_EXT_ID)
			goto Eofunc;
#endif
		RBUF_Push(&can_rx,_can_msg_rx.Data,_can_msg_rx.DLC);
Eofunc:
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	}
}
#if 1
void CANBUS_Send(const unsigned char *buf, unsigned int length)
{
	CanTxMsg msg;
	CAN_TxMessageInit(&msg,CAN_TX_STD_ID, CAN_TX_EXT_ID, CAN_TX_FRAME_TYPE, CAN_TX_RTR_TYPE, length, buf);    //生成CAN报文
	CAN_Transmit(CAN1, &msg);                         //发送CAN报文
}
#else
void CANBUS_Send(const unsigned char *buf, unsigned int length)
{
    RBUF_Push(&can_tx,buf,length);
    USART_ITConfig(USART1,USART_IT_TXE,ENABLE);
}
#endif

unsigned int CANBUS_Read(unsigned char *buf, unsigned int buf_size)
{
	unsigned int nPop;
	__disable_irq();
    nPop = RBUF_Pop(&can_rx,buf,buf_size);
	__enable_irq();
	return nPop;
}

unsigned int CANBUS_RxSize()
{
	return RBUF_Count(&can_rx);
}

void CANBUS_RxReset()
{
	__disable_irq();
	RBUF_Reset(&can_rx);
	__enable_irq();
}
