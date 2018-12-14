#ifndef _RS232_H_
#define _RS232_H_

void RS232_Init();

void RS232_Send(const unsigned char *buf, unsigned int length);

unsigned int RS232_Read(unsigned char *buf, unsigned int buf_size);

unsigned int RS232_RxSize();

void RS232_RxReset();

#endif
