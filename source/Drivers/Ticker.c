#include "board.h"
#include "Ticker.h"


void TICKER_Init(void)
{
	RCC_ClocksTypeDef RCC_ClocksStatus;
	
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	SysTick_Config((u32)RCC_ClocksStatus.HCLK_Frequency / GetTickCountPerSec());
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    //SysTick_ITConfig(ENABLE);
}

void TICKER_DelayMs(int ms)
{
	unsigned int ticker = GetSysTickCount() + ms;
	while(GetSysTickCount() < ticker);
}

void SysTick_Handler(void)
{
	system_ticks++;
}