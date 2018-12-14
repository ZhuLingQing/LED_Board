#include "board.h"
#include "LED.h"
#include "Ticker.h"
#include "Button.h"
#include "RS232.h"
#include "CAN.h"

static unsigned char boardAddress = 0;
unsigned char GetBoardAddress()
{
	return boardAddress;
}

void BOARD_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	boardAddress |= (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)?0:1);
	boardAddress |= (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)?0:2);
	
	//USART_SendData(USART1,boardAddress + '0');
}

void INIT_NVIC_Configuration(void)
{

    #ifdef IAP_BOOT
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000); 
    #endif

    /* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

}
void GPIO_Reset(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC |\
                           RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    GPIO_InitStructure.GPIO_Pin = 0xffff;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOB, &GPIO_InitStructure);

}

void SoftReset( void )
{
    __set_FAULTMASK(1);//关闭所有中端
    NVIC_SystemReset();//复位
}

void HARDWARE_Initialize()
{
	extern void DEBUG_Initialize();
    INIT_NVIC_Configuration();
    
    GPIO_Reset();
#ifdef TRACE_ON
    DEBUG_Initialize();
#endif
}

void SYSTEM_Initialize()
{
	RCC_ClocksTypeDef RCC_Clocks;
    /*
    sys_device.uid = *(unsigned int*)(0X1FFFF7F0);
   
    ChipUniqueID[2] = *(__IO u32 *)(0X1FFFF7E8); // Low
    ChipUniqueID[1] = *(__IO u32 *)(0X1FFFF7EC); // 
    ChipUniqueID[0] = *(__IO u32 *)(0X1FFFF7F0); // High
    */
#ifdef _NDEBUG
	TRACE_LINE("\r\n\t\tBoard: %s Version: %d.%d.%d",gProductName,gProductVersion[0],gProductVersion[1],gProductVersion[2]);
#else
	TRACE_LINE("\r\n\t\tBoard: %s Version: %d.%d (DEBUG)",gProductName,gProductVersion[0],gProductVersion[1]);
#endif
	RCC_GetClocksFreq(&RCC_Clocks);
	TRACE_LINE("SYSCLK_Frequency %d \r\n",RCC_Clocks.SYSCLK_Frequency);
    TRACE_LINE("\t\tBuild time: [%s __ %s]",__DATE__,__TIME__);
	
	LED_Init();
	TICKER_Init();
	BUTTON_Init();
	RS232_Init();
	CANBUS_Init();
	BOARD_Init();
	
	__enable_irq();
}
