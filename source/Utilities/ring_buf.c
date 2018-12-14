#include "ring_buf.h"


void RBUF_Init(RING_BUF* buf, unsigned char* data, unsigned int size)
{
	buf->data = data;
	buf->in = data;
	buf->out = data;
	buf->size = size;
	buf->count = 0;
}

void RBUF_Reset(RING_BUF* buf)
{
	//buf->data = data;
	buf->in = buf->data;
	buf->out = buf->data;
	//buf->size = size;
	buf->count = 0;
}

unsigned short RBUF_Push(RING_BUF *buf, const unsigned char *data, unsigned short length)
{
	unsigned short trans = 0;
	while(length -- && (buf->size > buf->count))
	{
		*buf->in++ = *data++;
		if(buf->in == buf->data + buf->size)
			buf->in = buf->data;
		buf->count++;
		trans++;
	}
	return trans;
}
unsigned short RBUF_Pop(RING_BUF *buf, unsigned char *data, unsigned short length)
{
	unsigned short trans = 0;
	while(length -- && buf->count)
	{
		*data++ = *buf->out++;
		if(buf->out == buf->data + buf->size)
			buf->out = buf->data;
		buf->count--;
		trans++;
	}
	return trans;
}