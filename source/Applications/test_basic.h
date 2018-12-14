#ifndef _DP_TEST_BASIC_H_
#define _DP_TEST_BASIC_H_

#define UART_READ_BUF_SIZE      (1024+128)

#define UAER_CMD_HEADER 0xAA55

#define UART_DL_FPGA_BUF_SIZE   1024
typedef struct __st_UART_DL_FPGA {
	unsigned short header;
	char cmd;
	char cs;
	char erase;
	char PV;
	unsigned short res1;
	unsigned short res2;
	unsigned short filecrc;
	DWORD saddr;
	DWORD length;
	DWORD PACKAGENUM;
	DWORD ALL_PACKAGENUM;//28

	unsigned short cmdcrc;//30
	unsigned short datacrc;
	unsigned char readbuf[UART_DL_FPGA_BUF_SIZE];
}st_UART_DL_FPGA;


typedef struct __st_UART_IRQBUF{
unsigned short count;
unsigned short readp;
char buf[UART_READ_BUF_SIZE];
}st_UART_IRQBUF;


extern st_UART_IRQBUF uart_IRQBuf;


#define EN_TRACE_SPIFLASH_LOG   0
#define TRACE_LOG_ADDR          0x300000//3MB
#define TRACE_MAX_LOG_SIZE      0x100000//3MB
#define TRACE_LOG_SIZE          2048
#if EN_TRACE_SPIFLASH_LOG
extern DWORD logaddr;
extern char logbuff[TRACE_LOG_SIZE];
extern char savelogbuff[TRACE_LOG_SIZE];

extern DWORD logsize;
extern DWORD savelogsize;
#endif

extern void erase_all_spiflash_LOG(void);

extern int usart1_wdata(char* data,int len);
#endif