/*
*********************************************************************************************************

   (C) Copyright 2007, RAE Systems Inc. 	 All rights reserved

----------------- File Information ---------------------------------------------
       File Name: typedef.c
          Author: Zhu LingQing
Last Modify Date: 008/4/29
     Discription: Middle level Module for type definition.

---------------- Important Notes -----------------------------------------------


----------------- Rivision History ---------------------------------------------
         Version: V1.00
          Author: Zhu LingQing
Last Modify Date: 2008/4/29
     Discription: Initial version

*********************************************************************************************************
*/

#ifndef	_TYPE_DEF_H_
#define _TYPE_DEF_H_

typedef unsigned int HANDLE;

//typedef char                  BYTE;
typedef signed char			CHAR;
typedef unsigned char		UCHAR;
typedef unsigned char		BYTE;
typedef unsigned short       WORD;
typedef signed short	    SHORT;
typedef unsigned short		USHORT;
typedef unsigned int	    DWORD;
typedef signed int			LONG;
typedef unsigned long		ULONG;
typedef unsigned int *		WPARAM;
typedef unsigned int	    LPARAM;
typedef void *				LPVOID;
typedef signed int			INT;
typedef unsigned int	    UINT;
typedef const char*			CSTR;
typedef char*			    STR;


typedef enum{ FALSE=0,TRUE=!FALSE } BOOL;
typedef enum{ OFF=0,ON=!OFF } SWITCH;

typedef union{
    unsigned char eu8;
    struct{
        unsigned char bit0 :1;
        unsigned char bit1 :1;
        unsigned char bit2 :1;
        unsigned char bit3 :1;
        unsigned char bit4 :1;
        unsigned char bit5 :1;
        unsigned char bit6 :1;
        unsigned char bit7 :1;
    };
} EX_US_BYTE;

typedef union{
    unsigned short eu16;
    struct{
        unsigned char low;
        unsigned char high;
    };
} EX_US_SHORT;

typedef union{
    unsigned long eu32;
    struct{
        unsigned char byte1;
        unsigned char byte2;
        unsigned char byte3;
        unsigned char byte4;
    };
} EX_US_LONG;

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef void (*vvpfunc)(void *);
typedef void (*vvfunc)();
typedef unsigned int (*uivfunc)();
typedef void (*vuifunc)(unsigned int);
typedef void (*vucfunc)(unsigned char);

#ifndef IN
#define IN		0
#endif
#ifndef OUT
#define OUT		1
#endif

#define HIGH	1
#define LOW     0

#define DUMMY_BYTE 0
#define DONT_CARE_BYTE 0

#define STR_BACKUP	"\b \b"

#define SIZE_ARY(a)             (sizeof(a)/sizeof(a[0]))

#define IS_AF(b)				( ((b)>='A') && ((b)<='F') )
#define IS_af(b)				( ((b)>='a') && ((b)<='f') )
#define IS_NUM(b)				( ((b)>='0') && ((b)<='9') )
#define IS_HEX(b)				( IS_AF(b) || IS_af(b) || IS_09(b) )
#define IS_ASCII(b)				( ((b)>=0x20) && ((b)<=0x7F) )

#define IS_AZ(b)				( ((b)>='A') && ((b)<='Z') )
#define IS_az(b)				( ((b)>='a') && ((b)<='z') )
#define IS_CHAR(b)				( IS_AZ(b) || IS_az(b) )

#define IS_RANGE(b,min,max)				( ((b)>=min) && ((b)<=max) )

#define IS_PLUS(b)				( ((b)=='+') || ((b)=='=') )
#define IS_MINUS(b)				( ((b)=='-') || ((b)=='_') )
#define IS_ENTER(b)             ( (b)==0x0D )
#define IS_ESC(b)				( (b)==0x1B )
#define IS_DEL(b)				( (b)==0x7F )
#define IS_TAB(b)				( (b)==0x09 )
#define IS_BACKSPACE(b)			( (b)==0x08 )

#define BITS(b)					( (1UL<<(b))-1 )

#define BIT(b)					( 1UL<<(b) )										//位
#define SETBIT(n,b)				( (n) |= 1UL<<(b) )									//位置1
#define CLRBIT(n,b)				( (n) &= ~1UL<<(b) )									//位置0
#define REVBIT(n,b)				( ((n)&(1UL<<(b)))?( (n) &= ~1UL<<(b) ):( (n) |= 1UL<<(b) ))		//位翻转

#define max(a,b)				( ((a)>(b))?(a):(b) )			//取大
#define min(a,b)				( ((a)>(b))?(b):(a) )			//取小
#define neg(a)					( ~(a)+1 )				//取反


//#define abs(a)					( (a<0)?(~a+1):a )		//绝对值

#define SPLIT_SHORT(s,h,l)		( l = (u8)s, h = s>>8 ) //将short型拆分成2个char型
#define COMBINE_SHORT(s,h,l)	( s=h<<8, s+=l ) 		//将2个char型合并成short型

/*
********************************************************************************
* STANDARD BITS
********************************************************************************
*/
#define BIT0            (0x00000001)
#define BIT1            (0x00000002)
#define BIT2            (0x00000004)
#define BIT3            (0x00000008)
#define BIT4            (0x00000010)
#define BIT5            (0x00000020)
#define BIT6            (0x00000040)
#define BIT7            (0x00000080)
#define BIT8            (0x00000100)
#define BIT9            (0x00000200)
#define BIT10           (0x00000400)
#define BIT11           (0x00000800)
#define BIT12           (0x00001000)
#define BIT13           (0x00002000)
#define BIT14           (0x00004000)
#define BIT15           (0x00008000)
#define BIT16           (0x00010000)
#define BIT17           (0x00020000)
#define BIT18           (0x00040000)
#define BIT19           (0x00080000)
#define BIT20           (0x00100000)
#define BIT21           (0x00200000)
#define BIT22           (0x00400000)
#define BIT23           (0x00800000)
#define BIT24           (0x01000000)
#define BIT25           (0x02000000)
#define BIT26           (0x04000000)
#define BIT27           (0x08000000)
#define BIT28           (0x10000000)
#define BIT29           (0x20000000)
#define BIT30           (0x40000000)
#define BIT31           (0x80000000)

#define KILO_BYTE(v)	((v)<<10)
#define MEGA_BYTE(v)	((v)<<20)
#define GIGA_BYTE(v)	((v)<<30)
/*
typedef struct {

    volatile DWORD   transferred; /// Number of bytes transferred
    volatile DWORD   remaining;   /// Number of bytes not transferred
    OS_EVENT* semaphore;
    volatile WORD status;      /// Operation result code

}OSTransfer;
*/

typedef unsigned int (*rd_callback)(void* dev, void* data, unsigned int length);
typedef unsigned int (*wr_callback)(void* dev, const void* data, unsigned int length);
#endif
