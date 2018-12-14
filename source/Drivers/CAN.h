#ifndef _CAN_H_
#define _CAN_H_

void CANBUS_Init();

void CANBUS_Send(const unsigned char *buf, unsigned int length);

unsigned int CANBUS_Read(unsigned char *buf, unsigned int buf_size);

unsigned int CANBUS_RxSize();

void CANBUS_RxReset();
#endif
