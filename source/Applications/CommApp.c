
#include "LED.h"
#include "Ticker.h"
#include "Button.h"
#include "RS232.h"
#include "CAN.h"

#define COMM_LED_COUNT	32

typedef enum {
	COMM_WAIT_36 = 0,
	COMM_WAIT_16,
	COMM_WAIT_ADDR,
	COMM_WAIT_A0,
	COMM_WAIT_LED,
	COMM_WAIT_28,
	COMM_WAIT_98,
}COMM_STATUS;

typedef struct {
	unsigned int nLedGet;
	unsigned char ledValue[COMM_LED_COUNT];
	unsigned int StartTick;
	COMM_STATUS status;
}COMM_DEF;

static COMM_DEF commRs232 = {0};
static COMM_DEF commCan = {0};

static void COMM_Proc(COMM_DEF* comm, unsigned char data, unsigned int timeout)
{
	int i;
	if(data == 0x36 && comm->status != COMM_WAIT_LED)
		comm->status = COMM_WAIT_36;
	else if(timeout && (GetSysTickCount() - comm->StartTick >= timeout) )
		comm->status = COMM_WAIT_36;
	
	switch(comm->status)
	{
	case COMM_WAIT_36:
		comm->status = (data == 0x36)?COMM_WAIT_16:COMM_WAIT_36;
		comm->StartTick = GetSysTickCount();
		break;
	case COMM_WAIT_16:
		comm->status = (data == 0x16)?COMM_WAIT_ADDR:COMM_WAIT_36;
		break;
	case COMM_WAIT_ADDR:
		comm->status = (data == GetBoardAddress())?COMM_WAIT_A0:COMM_WAIT_36;
		break;
	case COMM_WAIT_A0:
		comm->status = (data == 0xA0)?COMM_WAIT_LED:COMM_WAIT_36;
		if(comm->status == COMM_WAIT_LED)
			comm->nLedGet = 0;
		break;
	case COMM_WAIT_LED:
		comm->ledValue[comm->nLedGet++] = data;
		if(comm->nLedGet == COMM_LED_COUNT)
			comm->status = COMM_WAIT_28;
		break;
	case COMM_WAIT_28:
		comm->status = (data == 0x28)?COMM_WAIT_98:COMM_WAIT_36;
		break;
	case COMM_WAIT_98:
		if(data == 0x98)
		{
			for(i = 0; i < LED_COUNT; i++)
				LED_Config(i,(LED_STATUS)comm->ledValue[i]);
		}
		comm->status = COMM_WAIT_36;
		break;
	}
}

void COMM_Handler()
{
	unsigned char buf[128];
	unsigned int i,nRead;
	
	nRead = RS232_Read(buf,sizeof(buf));
	for( i = 0; i < nRead; i++)
		COMM_Proc(&commRs232,buf[i],RS232_RX_TIMEOUT);
	
	nRead = CANBUS_Read(buf,sizeof(buf));
	for( i = 0; i < nRead; i++)
		COMM_Proc(&commCan,buf[i],CAN_RX_TIMEOUT);
}