/*
 * includes.h
 *
 * Created: 20.04.2021 19:21:39
 *  Author: Sebastian
 */ 

#define F_CPU 16000000
#define BAUD 1000000

#define ws2812_port C   // Data port
#define ws2812_pin  5   // Data out pin


 #define USR_LED_INIT     DDRB |= (1<<5); USRLED0
 #define USRLED1          PORTB |= (1<<5)
 #define USRLED0          PORTB &= ~(1<<5)
 #define USRLEDT          PORTB ^= (1<<5)