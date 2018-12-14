/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */


//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "trace.h"
#include <assert.h>

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------

/// Trace level can be set at applet initialization
#if !defined(NOTRACE) && (DYN_TRACES == 1)
    unsigned int traceLevel = TRACE_LEVEL;
#endif

#ifndef NOFPUT
#include <stdio.h>
#include <stdarg.h>

//#include "stm32f10x_lib.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Routine to write a char                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void SendChar(int ch)
{
  /* Write a character to the USART */
  USART_SendData(USART1, (u8) ch);

  /* Loop until the end of transmission */
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {
  }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Routine to get a char                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
char GetChar(void)
{
	while(USART_GetFlagStatus(USART1,USART_IT_RXNE)==RESET)
		Sleep(100);
	return (char)USART_ReceiveData(USART1);
}

unsigned char IsCharReady()
{
	return (USART_GetFlagStatus(USART1,USART_IT_RXNE)==SET);//(UART0->FSR.RX_EMPTY == 0 )?1:0;
}

//------------------------------------------------------------------------------
/// \exclude
/// Implementation of fputc using the DBGU as the standard output. Required
/// for printf().
/// \param c  Character to write.
/// \param pStream  Output stream.
/// \param The character written if successful, or -1 if the output stream is
/// not stdout or stderr.
//------------------------------------------------------------------------------
signed int fputc(signed int c, FILE *f)
{
        TRACE_PutChar(c);
        return c;
}

//------------------------------------------------------------------------------
/// \exclude
/// Implementation of fputs using the DBGU as the standard output. Required
/// for printf(). Does NOT currently use the PDC.
/// \param pStr  String to write.
/// \param pStream  Output stream.
/// \return Number of characters written if successful, or -1 if the output
/// stream is not stdout or stderr.
//------------------------------------------------------------------------------
signed int fputs(const char *pStr, FILE *f)
{
    signed int num = 0;

    while (*pStr != 0) {

        if (fputc(*pStr,f) == -1) {

            return -1;
        }
        num++;
        pStr++;
    }

    return num;
}

#undef putchar

//------------------------------------------------------------------------------
/// \exclude
/// Outputs a character on the DBGU.
/// \param c  Character to output.
/// \return The character that was output.
//------------------------------------------------------------------------------
signed int putchar(signed int c)
{
    return fputc(c,NULL);
}

#endif //#ifndef NOFPUT

//------------------------------------------------------------------------------
//         Local Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Print char if printable. If not print a point
/// \param c char to
//------------------------------------------------------------------------------
static void PrintChar(unsigned char c)
{
    if( (/*c >= 0x00 &&*/ c <= 0x1F) ||
        (c >= 0xB0 && c <= 0xDF) ) {

       printf(".");
    }
    else {

       printf("%c", c);
    }
}

//------------------------------------------------------------------------------
//         Global Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Displays the content of the given frame on the Trace interface.
/// \param pFrame  Pointer to the frame to dump.
/// \param size  Buffer size in bytes.
//------------------------------------------------------------------------------
void TRACE_DumpFrame(unsigned char *pFrame, unsigned int size)
{
    unsigned int i;

    for (i=0; i < size; i++) {
        printf("%02X ", pFrame[i]);
    }

    printf("\n\r");
}

//------------------------------------------------------------------------------
/// Displays the content of the given buffer on the Trace interface.
/// \param pFrame  Pointer to the buffer to dump.
/// \param size     Buffer size in bytes.
/// \param address  Start address to display
//------------------------------------------------------------------------------
void TRACE_DumpMemory(
    unsigned char *pFrame,
    unsigned int size,
    unsigned int address
    )
{
    unsigned int i, j;
    unsigned int lastLineStart;
    unsigned char* pTmp;

    for (i=0; i < (size / 16); i++) {

        printf("0x%08X: ", address + (i*16));
        pTmp = (unsigned char*)&pFrame[i*16];
        for (j=0; j < 4; j++) {
            printf("%02X%02X%02X%02X ", pTmp[0],pTmp[1],pTmp[2],pTmp[3]);
            pTmp += 4;
        }

        pTmp = (unsigned char*)&pFrame[i*16];
        for (j=0; j < 16; j++) {
            PrintChar(*pTmp++);
        }

        printf("\n\r");
    }

    if( (size%16) != 0) {
        lastLineStart = size - (size%16);
        printf("0x%08X: ", address + lastLineStart);

        for (j= lastLineStart; j < lastLineStart+16; j++) {

            if( (j!=lastLineStart) && (j%4 == 0) ) {
                printf(" ");
            }
            if(j<size) {
                printf("%02X", pFrame[j]);
            }
            else {
                printf("  ");
            }
        }

        printf(" ");
        for (j= lastLineStart; j <size; j++) {
            PrintChar(pFrame[j]);
        }

        printf("\n\r");
    }
}

//------------------------------------------------------------------------------
/// Reads an integer
//------------------------------------------------------------------------------
unsigned char TRACE_GetInteger(unsigned int *pValue)
{
    unsigned char key;
    unsigned char nbNb = 0;
    unsigned int value = 0;
    while(1) {
        key = TRACE_GetChar();
        TRACE_PutChar(key);
        if(key >= '0' &&  key <= '9' )
		{
            value = (value * 10) + (key - '0');
            nbNb++;
        }
		else if(key == '\b' || key == 0x7F)
		{
			value /= 10;
			if(nbNb)
			{
				TRACE_PutChar(' ');
				TRACE_PutChar(key);
				nbNb--;
			}
		}
        else//(key == 0x0D || key == ' ')
		{
            if(nbNb == 0)
			{
                printf("\n\rWrite a number and press ENTER or SPACE!\n\r");
                return 0;
            }
			else
			{
                printf("\n\r");
                *pValue = value;
                return 1;
            }
        }
    }
}

//------------------------------------------------------------------------------
/// Reads an integer and check the value
//------------------------------------------------------------------------------
unsigned char TRACE_GetIntegerMinMax(
    unsigned int *pValue,
    unsigned int min,
    unsigned int max
    )
{
    unsigned int value = 0;

    if( TRACE_GetInteger(&value) == 0) {
        return 0;
    }

    if(value < min || value > max) {
        printf("\n\rThe number have to be between %d and %d\n\r", min, max);
        return 0;
    }

    printf("\n\r");
    *pValue = value;
    return 1;
}

//------------------------------------------------------------------------------
/// Reads an hexadecimal number
//------------------------------------------------------------------------------
unsigned char TRACE_GetHex(unsigned int *pValue, signed int digNumber)
{
    unsigned char key;
    unsigned int i = 0;
    unsigned int value = 0;
	
	assert(digNumber &&(digNumber <=8));
    for(i = 0; i < digNumber; i++) {
        key = TRACE_GetChar();
        TRACE_PutChar(key);
        if(key >= '0' &&  key <= '9' ) {
            value = (value * 16) + (key - '0');
        }
        else if(key >= 'A' &&  key <= 'F' ) {
            value = (value * 16) + (key - 'A' + 10) ;
        }
        else if(key >= 'a' &&  key <= 'f' ) {
            value = (value * 16) + (key - 'a' + 10) ;
        }
		else if(key == ' ' || key == '\r' || key == '\n' || key == 0x1B)
			break;
		else if(key == 0xB || key == 0x7F)
		{
			if(i)
			{
				TRACE_PutChar(' ');
				TRACE_PutChar(key);
				i -= 2;
				value >>= 4;
			}
			else
				i--;
		}
        else {
            TRACE("\n\rIt is not a hexa character!\n\r");
            return 0;
        }
    }

    TRACE("\n\r");
    *pValue = value;
    return 1;
}

#if 0
void TRACE_INTERVAL()
{
	extern volatile unsigned int SysInterval;
	unsigned int tmp = SysInterval;
	SysInterval = 0;
	TRACE_INFO("[%4d]\t",tmp);
}
#endif
