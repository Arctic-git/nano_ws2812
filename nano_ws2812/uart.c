/*
* uart.c
*
* Created: 10.02.2021 20:36:44
*  Author: Sebastian
*/

#include "defines.h"
#include "uart.h"
#include <avr/io.h>
#include "light_ws2812.h"
#include <util/delay.h>

#include <util/setbaud.h>
#include <avr/interrupt.h>
#include <string.h>

extern uint32_t getMillis();

//#define INTERRUPT_MODE

//redefine for atmega328 has only uart 0
#define UDR1 UDR0
#define UCSR1A UCSR0A
#define UDRE1 UDRE0
#define RXC1 RXC0
#define UBRR1 UBRR0
#define UCSR1B UCSR0B
#define UCSR1C UCSR0C


void UART_SendChr(char c){
	while(!(UCSR1A & (1<<UDRE1)));
	UDR1 = c;
}

char UART_GetChr(){
	while(!(UCSR1A & (1<<RXC1)));
	return UDR1;
}

int UART_GetChrNonblocking(){
	if(!(UCSR1A & (1<<RXC1))){
		return -1;
	}
	char c = UDR1;
	return c;
}

static int UART_SendChr_printf(char var, FILE *stream) {
	if (var == '\n') UART_SendChr('\r');
	UART_SendChr(var);
	return 0;
}

static int UART_GetChr_printf(FILE *stream) {
	return UART_GetChr();
}

static FILE uartStream = FDEV_SETUP_STREAM(UART_SendChr_printf, UART_GetChr_printf, _FDEV_SETUP_RW);

static uint8_t* dataBuf;
static uint16_t dataBufSz;
static uint16_t writepos=20;

void UART_Init(uint8_t* dataBuf_, uint16_t dataBufSz_){
	dataBuf = dataBuf_;
	dataBufSz = dataBufSz_;

	UBRR1 = UBRR_VALUE; //util/setbaud

	UCSR1B |= (1<<TXEN0) | (1<<RXEN0);  // UART TX einschalten
	UCSR1C = (1<<UCSZ01)|(1<<UCSZ00);

	#ifdef INTERRUPT_MODE
	UCSR1B |= (1<<RXCIE0);
	#endif

	#if USE_2X //util/setbaud
	UCSR1A |= (1 << U2X0);
	#else
	UCSR1A &= ~(1 << U2X0);
	#endif

	stdout = UART_GetStream();
}

FILE* UART_GetStream(){
	return &uartStream;
}

int UART_RX_Task(void){
	static uint32_t lastDataTime =0;
	static uint8_t ledsActive =0;
	static uint8_t frameDone =0;
	static uint8_t potentialMiss =0;

	int16_t rxchr = UART_GetChrNonblocking();

	if(rxchr >= 0){
		lastDataTime = getMillis();
		

		if(writepos < dataBufSz){
			dataBuf[writepos++] = rxchr;
			//if(writepos == dataBufSz){	//if frame size is known
				//writepos=0;
				//frameDone=1;
			//}
		}
	}else{

		uint32_t msSinceData = getMillis() - lastDataTime;

		if((writepos || frameDone) && msSinceData > 1){	//timeout after 1-2ms of no new bytes
			if(potentialMiss){
				potentialMiss=0;
			}else{
				if(writepos){
					USRLED1;
					ws2812_sendarray(dataBuf, writepos); // blocks for 1.56ms for 52 leds -> possible to miss first bytes of next packet
					USRLED0;
					ledsActive = 1;
					potentialMiss = UART_GetChrNonblocking() >= 0; //potentially missed first byte, skip until next timeout
				}
			}
			writepos=0;
			frameDone=0;
		}

		if(ledsActive && msSinceData > 6000){
			ledsActive=0;
			memset(dataBuf, 0, dataBufSz);
			ws2812_sendarray((uint8_t*)dataBuf, dataBufSz);
		}
	}

	return 0;
}


