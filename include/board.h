#pragma once

//DEBUG_TRACE
//#define TRACE_ON

//TIME
#define SYSTICK_MILI_SECONDS	1
extern volatile unsigned int system_ticks;
extern void TICKER_DelayMs(int ms);
#define Sleep(ms)				TICKER_DelayMs(ms)
#define GetSysTickCount()		(system_ticks)
#define GetTickCountPerSec()	(SYSTICK_MILI_SECONDS*1000)
#define GetMiliSecondPassed(t)	((GetSysTickCount() - (t))*SYSTICK_MILI_SECONDS)


//GPIO
typedef struct __gpio_def
{
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	uint16_t reserved;
}GPIO_DEF;

#define GPIO_Set(pIo,bSet)		GPIO_WriteBit((pIo)->GPIOx,(pIo)->GPIO_Pin,(bSet)/*?Bit_SET:Bit_RESET*/)
#define GPIO_Get(pIo)			GPIO_ReadInputDataBit((pIo)->GPIOx,(pIo)->GPIO_Pin)
#define GPIO_GetOut(pIo)		GPIO_ReadOutputDataBit((pIo)->GPIOx,(pIo)->GPIO_Pin)

extern const char gProductName[];
extern const unsigned char gProductVersion[3];

//IDENTIFY
#define IDENTIFY_COUNT		1
extern unsigned char GetBoardAddress();

//LED
#define	LED_COUNT				16
#define LED_FAST_INTERVAL		500				//ms
#define LED_SLOW_INTERVAL		1000			//ms

//BUTTON
#define BUTTON_COUNT			1
#define BUTTON_DEATH			500				//ms
#define BUTTON_INTERVAL			1000			//ms

//RS232
#define RS232_BAUDRATE			9600
#define RS232_RX_BUF_SIZE		128
#define RS232_TX_BUF_SIZE		32
#define RS232_RX_TIMEOUT		1000			//ms, 0 will disable timeout

//CAN
#define CAN_BAUDRATE			100000			//hz
#define CAN_RX_BUF_SIZE			128
#define CAN_TX_BUF_SIZE			32
#define CAN_RX_TIMEOUT			1000			//ms, 0 will disable timeout
//#define CAN_RX_STD_ID			0xABC			//接收标准ID，任意地址都收就注释掉
//#define CAN_RX_EXT_ID			0x00000ABC		//接收扩展ID，任意地址都收就注释掉
#define CAN_TX_STD_ID			0x123			//本机标准ID
#define CAN_TX_EXT_ID			0x00000123		//本机扩展ID
#define CAN_TX_FRAME_TYPE		CAN_ID_STD		//CAN_ID_EXT
#define	CAN_TX_RTR_TYPE			CAN_RTR_DATA	//CAN_RTR_REMOTE
