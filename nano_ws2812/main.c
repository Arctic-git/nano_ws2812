/*
 * nano_ws2812.c
 *
 * Created: 20.04.2021 18:44:58
 * Author : Sebastian
 */ 

#include "defines.h"
#include <avr/io.h>
#include "uart.h"
#include "light_ws2812.h"


static volatile uint32_t millis = 0; //ovf after 50 days
static uint8_t ledbuf[100*3] = {}; //RGB

ISR (TIMER1_COMPA_vect){
	millis++;
}

uint32_t getMillis(){
	return millis;
}


int main(void){

	USR_LED_INIT;
	DDRD|= (1<<ws2812_pin);

	TIMSK1 |=(1<<OCIE1A)|(0<<TOIE1); //int
	TCCR1B |= (0<<CS12)|(0<<CS11)|(1<<CS10); //prescaler (1)
	TCCR1B |= (1<<WGM12); //ctc(clear on compare match)
	OCR1A = F_CPU/(1 * 1000) - 1;

	sei();
	
	UART_Init(ledbuf, sizeof(ledbuf));

    while (1) {
		UART_RX_Task();
    }
}

