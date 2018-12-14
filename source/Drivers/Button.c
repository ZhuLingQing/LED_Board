#include "Board.h"
#include "Button.h"
#include "RS232.h"
#include "CAN.h"

typedef enum {
	BUTTON_IDLE = 0,
	BUTTON_PUSH_DOWN,
	BUTTON_WAIT_1SEC,
	BUTTON_PRE_RELEASE,
	BUTTON_WAIT_RELEASE,
	BUTTON_RELEASED,
	BUTTON_WAIT_DEATH
}BUTTON_STATUS;

typedef struct {
	unsigned int tick;
	BUTTON_STATUS status;
}BUTTON_DEF;

static const GPIO_DEF button_def[BUTTON_COUNT] = {
	{GPIOB,GPIO_Pin_7 },
};

static BUTTON_DEF button_status[BUTTON_COUNT] = {0};

void BUTTON_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	for(int i = 0; i < BUTTON_COUNT; i++)
	{
    	GPIO_InitStructure.GPIO_Pin = button_def[i].GPIO_Pin;
    	GPIO_Init(button_def[i].GPIOx, &GPIO_InitStructure);
	}
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource7);

	EXTI_InitStructure.EXTI_Line=EXTI_Line7;  //配置为外部中断线0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  //配置外部中断线为中断请求
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; //使能中断
	EXTI_Init(&EXTI_InitStructure); //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line7) != RESET)   //检查指定的EXT0线路触发请求发生与否
	{
		if(GPIO_Get(button_def + 0) == Bit_RESET)
		{
			if((button_status + 0)->status == BUTTON_IDLE)
				(button_status + 0)->status = BUTTON_PUSH_DOWN;
			else if((button_status + 0)->status == BUTTON_PRE_RELEASE)
				(button_status + 0)->status = BUTTON_WAIT_1SEC;
		}
		else
		{
			if((button_status + 0)->status == BUTTON_WAIT_RELEASE)
				(button_status + 0)->status = BUTTON_RELEASED;
			else if((button_status + 0)->status == BUTTON_WAIT_1SEC)
				(button_status + 0)->status = BUTTON_PRE_RELEASE;
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line7);  //清除EXTI0线路挂起位
}

void BUTTON_SendPack( unsigned char keyVal)
{
	unsigned char pack_data[] = {0x36,0x16,GetBoardAddress(),0xA1,keyVal,0x28,0x98};
	RS232_Send(pack_data,sizeof(pack_data));
	CANBUS_Send(pack_data,sizeof(pack_data));
}


void BUTTON_Handler()
{
	__disable_irq();
	switch((button_status + 0)->status)
	{
	case BUTTON_IDLE:
		break;
	case BUTTON_PUSH_DOWN:
		(button_status + 0)->tick = GetSysTickCount();
		//Send down pack
		BUTTON_SendPack(0);
		(button_status + 0)->status = BUTTON_WAIT_1SEC;
		break;
	case BUTTON_WAIT_1SEC:
		if(GetMiliSecondPassed((button_status + 0)->tick) >= BUTTON_INTERVAL)
			(button_status + 0)->status = BUTTON_WAIT_RELEASE;
		break;
	case BUTTON_PRE_RELEASE:
		if(GetMiliSecondPassed((button_status + 0)->tick) >= BUTTON_INTERVAL)
			(button_status + 0)->status = BUTTON_RELEASED;
		break;
	case BUTTON_WAIT_RELEASE:
		break;
	case BUTTON_RELEASED:
		(button_status + 0)->tick = GetSysTickCount();
		//send up pack
		BUTTON_SendPack(1);
		(button_status + 0)->status = BUTTON_WAIT_DEATH;
		break;
	case BUTTON_WAIT_DEATH:
		if(GetMiliSecondPassed((button_status + 0)->tick) >= BUTTON_DEATH)
			(button_status + 0)->status = BUTTON_IDLE;
		break;
	}
	__enable_irq();
}

