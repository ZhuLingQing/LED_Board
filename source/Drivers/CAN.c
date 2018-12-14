#include "Board.h"
#include "CAN.h"
#include "ring_buf.h"

static unsigned char rxBuf[CAN_RX_BUF_SIZE];
static unsigned char txBuf[CAN_TX_BUF_SIZE];

static RING_BUF can_rx;
static RING_BUF can_tx;

//������=APB1Clock/(1+CAN_BS1+CAN_BS2)/CAN_Prescaler
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure CAN pin: TX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    //GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
  	RCC_GetClocksFreq(&RCC_ClocksStatus);
	
	CAN_DeInit(CAN1); //������CAN��ȫ���Ĵ�������Ϊȱʡֵ 
	CAN_StructInit(&CAN_InitStructure); //��CAN_InitStruct�е�ÿһ��������ȱʡֵ���� 
	/* CAN cell init */ 
	CAN_InitStructure.CAN_TTCM=DISABLE; //û��ʹ��ʱ�䴥��ģʽ 
	CAN_InitStructure.CAN_ABOM=DISABLE; //û��ʹ���Զ����߹��� 
	CAN_InitStructure.CAN_AWUM=DISABLE; //û��ʹ���Զ�����ģʽ 
	CAN_InitStructure.CAN_NART=DISABLE; //û��ʹ�ܷ��Զ��ش�ģʽ 
	CAN_InitStructure.CAN_RFLM=DISABLE; //û��ʹ�ܽ���FIFO����ģʽ 
	CAN_InitStructure.CAN_TXFP=DISABLE; //û��ʹ�ܷ���FIFO���ȼ� 
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal; //CAN����Ϊ����ģʽ 
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq; //����ͬ����Ծ���1��ʱ�䵥λ 
	CAN_InitStructure.CAN_BS1=CAN_BS1_7tq; //ʱ���1Ϊ3��ʱ�䵥λ 
	CAN_InitStructure.CAN_BS2=CAN_BS2_7tq; //ʱ���2Ϊ2��ʱ�䵥λ 
	//CAN_InitStructure.CAN_Prescaler=24; //ʱ�䵥λ����Ϊ60 	
    CAN_InitStructure.CAN_Prescaler = RCC_ClocksStatus.PCLK1_Frequency/(1 + 7 + 7)/CAN_BAUDRATE;
	CAN_Init(CAN1,&CAN_InitStructure);
	
	// CAN filter init ��������ע�⣬ֻ�������������������ݣ������������� 
	CAN_FilterInitStructure.CAN_FilterNumber=1; 	//ָ��������Ϊ1 
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	//ָ��������Ϊ��ʶ������λģʽ 
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //������λ��Ϊ32λ 
	
	//CAN_FilterInitStructure.CAN_FilterIdHigh= (((u32)0x1314<<3)&0xFFFF0000)>>16;  
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;//((0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //Ҫ���˵�ID��λ 
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;//0xFFFF; //���������α�ʶ���ĸ�16λֵ 
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;//0xFFFF; //���������α�ʶ���ĵ�16λֵ 
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0; //�趨��ָ���������FIFOΪ0 
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //ʹ�ܹ����� 
	CAN_FilterInit(&CAN_FilterInitStructure); //������Ĳ�����ʼ�������� 
	
	/* CAN FIFO0 message pending interrupt enable */ 
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE); //ʹ��FIFO0��Ϣ�Һ��ж�
	
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;   
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;             //CAN1_RX0�ж��������п���
    NVIC_Init(&NVIC_InitStructure);

}
//CAN����֡����
void CAN_TxMessageInit(CanTxMsg* _can_msg_tx, unsigned int std_id, unsigned int ext_id, unsigned char ide, unsigned char rtr, unsigned char dlc, const unsigned char *pdata)
{
    unsigned char i;
    _can_msg_tx->StdId = std_id&0x7ff; //�趨��׼��ʶ��0~0x7ff  11λ
    _can_msg_tx->ExtId = ext_id&0x3ffff; //�趨�����ʶ��0~0x3ffff 18λ
    _can_msg_tx->IDE = ide; //�����ʶ������,STD(��׼��ʶ��)��EXT(�����ʶ��)
    _can_msg_tx->RTR = rtr; //���֡����,DATA(����֡)����REMOTE(Զ��֡)
    _can_msg_tx->DLC = dlc; //֡���ȣ�0~8
	for(i=0; i<dlc; i++)
	{
		_can_msg_tx->Data[i] = *(pdata+i);
	}
}

unsigned char rdata[8];
unsigned char tdata[8] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
//CAN�жϽ��պ���
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CanRxMsg _can_msg_rx;
	ITStatus Status;

	Status = CAN_GetITStatus(CAN1, CAN_IT_FMP0);             //�жϽ��ܵ��������ж��ź�

	if(Status == SET)
	{
		CAN_Receive(CAN1, CAN_FIFO0, &_can_msg_rx);        //����һ��CAN����
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
	CAN_TxMessageInit(&msg,CAN_TX_STD_ID, CAN_TX_EXT_ID, CAN_TX_FRAME_TYPE, CAN_TX_RTR_TYPE, length, buf);    //����CAN����
	CAN_Transmit(CAN1, &msg);                         //����CAN����
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
