#include "board.h"
#include "LED.h"

static const GPIO_DEF led_def[LED_COUNT] = {	
	{GPIOC,GPIO_Pin_14},
	{GPIOC,GPIO_Pin_15},
	{GPIOA,GPIO_Pin_0 },
	{GPIOA,GPIO_Pin_1 },
	{GPIOC,GPIO_Pin_1 },
	{GPIOC,GPIO_Pin_13},
	{GPIOA,GPIO_Pin_2 },
	{GPIOA,GPIO_Pin_3 },
	
	{GPIOC,GPIO_Pin_11},
	{GPIOA,GPIO_Pin_8 },
	{GPIOB,GPIO_Pin_14},
	{GPIOB,GPIO_Pin_13},
	{GPIOC,GPIO_Pin_9 },
	{GPIOC,GPIO_Pin_8 },
	{GPIOC,GPIO_Pin_7 },
	{GPIOC,GPIO_Pin_6 },
		
};

static LED_STATUS _led_status[LED_COUNT] = {0};

void LED_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	for(int i = 0; i < LED_COUNT; i++)
	{
    	GPIO_InitStructure.GPIO_Pin = led_def[i].GPIO_Pin;
    	GPIO_Init(led_def[i].GPIOx, &GPIO_InitStructure);
		LED_Config(i,_led_status[i]);
	}
}

void LED_Config(unsigned int index, LED_STATUS status)
{
	if(status == LED_OFF)
		GPIO_Set(led_def + index,Bit_SET);//GPIO_WriteBit(led_def[index].GPIOx,led_def[index].GPIO_Pin,Bit_SET);
	else if(status == LED_ON)
		GPIO_Set(led_def + index,Bit_RESET);//GPIO_WriteBit(led_def[index].GPIOx,led_def[index].GPIO_Pin,Bit_RESET);
	_led_status[index] = status;
}

void LED_Handler()
{
	static unsigned int _led_handle_tick_s = 0;
	static unsigned int _led_handle_tick_f = 0;
	int i;
	
	if(GetMiliSecondPassed(_led_handle_tick_f) >= LED_FAST_INTERVAL)
	{
		for(i = 0; i < LED_COUNT; i++)
		{
			if(_led_status[i] == LED_BLINK_FAST)
				GPIO_Set(led_def + i,!GPIO_GetOut(led_def + i));
		}
		_led_handle_tick_f = GetSysTickCount();
	}
	if(GetMiliSecondPassed(_led_handle_tick_s) >= LED_SLOW_INTERVAL)
	{
		for(i = 0; i < LED_COUNT; i++)
		{
			if(_led_status[i] == LED_BLINK_SLOW)
				GPIO_Set(led_def + i,!GPIO_GetOut(led_def + i));
		}
		_led_handle_tick_s = GetSysTickCount();
	}
}

void TASK_LED(void *param)
{
	int i = 0;
	while(1)
	{
		LED_Config(i,LED_ON);
		Sleep(500);
		LED_Config(i,LED_OFF);
		if(++i == LED_COUNT) i = 0;
		//TRACE_LINE("Tick:%d",GetSysTickCount());
	}
}
