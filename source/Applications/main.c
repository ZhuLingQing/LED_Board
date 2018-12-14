#include "trace\trace.h"
#include "system.h"

#include "LED.h"
#include "Ticker.h"
#include "Button.h"
#include "RS232.h"
#include "CAN.h"

const char gProductName[8] @ 0x0800FFF0 = PRODUCT_NAME;
const unsigned char gProductVersion[3] @ 0x0800FFF8 = {VERSION_MAIN,VERSION_MIDDLE,VERSION_BUILD};

void HARDWARE_Initialize();
void SYSTEM_Initialize();
void COMM_Handler();

int main()
{
	FLASH_Lock();
	HARDWARE_Initialize();
	SYSTEM_Initialize();
	
	//TASK_Debug(NULL);
	//TASK_LED(NULL);
	//CANBUS_Handler();
	while(1)
	{
		LED_Handler();
		BUTTON_Handler();
		COMM_Handler();
	}
	return 0;
}
