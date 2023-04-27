
/*
 * uart.c
 *
 * Created: 06/03/2023 16:15:26
 *  Author: aleks
 */ 
#define F_CPU 16000000UL        // Speed of the microprocessor, in hertz (16 MHz)

#include <avr/io.h>
#include <stdio.h>
#include <avr/sfr_defs.h>

// Baud rate for serial
#ifndef BAUD
#define BAUD 9600
#endif
#include <util/setbaud.h>

void uart_init(void) {
	// Write prescaler value to baud registers
	// Higher byte
	UBRR0H = UBRRH_VALUE;
	// Lower byte
	UBRR0L = UBRRL_VALUE;
	
	#if USE_2X
	UCSR0A |= _BV(U2X0);
	#else
	UCSR0A &= ~(_BV(U2X0));
	#endif

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

void uart_putchar(char c, FILE *stream) {
	if (c == '\n') {
		uart_putchar('\r', stream);
	}
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
}

char uart_getchar(FILE *stream) {
	if(bit_is_set(UCSR0A, RXC0)){
        return UDR0;
    }
	return 0x63;
}