#ifndef _LED_H_
#define _LED_H_

typedef enum {
	LED_OFF = 0,
	LED_ON,
	LED_BLINK_FAST,
	LED_BLINK_SLOW
}LED_STATUS;

void LED_Init();

void LED_Config(unsigned int index, LED_STATUS status);

void LED_Handler();

#endif

