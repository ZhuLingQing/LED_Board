#ifndef _RING_BUF_
#define _RING_BUF_

typedef struct {
	unsigned char *data;
	unsigned char *in;
	unsigned char *out;
	unsigned short size;
	unsigned short count;
}RING_BUF;


void RBUF_Init(RING_BUF* buf, unsigned char* data, unsigned int size);
void RBUF_Reset(RING_BUF* buf);
unsigned short RBUF_Push(RING_BUF *buf, const unsigned char *data, unsigned short length);
unsigned short RBUF_Pop(RING_BUF *buf, unsigned char *data, unsigned short length);
#define RBUF_Count(buf)		((buf)->count)
#endif
