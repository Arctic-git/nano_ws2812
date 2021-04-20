/*
 * uart.h
 *
 * Created: 10.02.2021 20:37:23
 *  Author: Sebastian
 */ 


#ifndef _UART_H_ 
#define _UART_H_

#include <stdint.h>
#include <stdio.h>


void UART_Init(uint8_t* dataBuf_, uint16_t dataBufSz_);
void UART_SendChr(char c);
char UART_GetChr();
int UART_GetChrNonblocking();

FILE* UART_GetStream();
int UART_RX_Task(void);

#endif