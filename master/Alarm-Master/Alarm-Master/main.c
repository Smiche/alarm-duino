#include <stdio.h>

#define F_CPU 16000000UL        // Speed of the microprocessor, in hertz (16 MHz)
#include "avr/io.h"
#include <string.h>
#include "avr/delay.h"
#include "main.h"
#include "uart.h"
#include "millis.h"

// macros
#define PROTOCOL_LENGTH 8

// globals
uint8_t *uartBuffer = "0000000000000000"; // 16 byte buffer for incoming data

void init(){
    init_millis(F_CPU); //frequency the atmega328p is running at
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;
}


void shiftBuffer(uint8_t newVal){
    if(newVal == 0x0){
        return;
    }
    
    for(int i = strlen(uartBuffer) - 2; i > -1; i--){
        uartBuffer[i+1] = uartBuffer[i];
    }
    uartBuffer[0] = newVal;
}

void uart_getstring(char *buffer, uint8_t length) {
    uint8_t i = 0;
    char c;
    uint32_t timeout_start = millis(); // assuming you have defined millis() to return the current time in milliseconds
    do {
        if (UCSR0A & (1 << RXC0)) {
            c = UDR0;
            if ((c != '\n') && (i < length-1)) {
                buffer[i++] = c;
            }
        }
    } while ((millis() - timeout_start) < 1000UL);
    buffer[i] = '\0';
    if (i == 0) {
        strcpy(buffer, ""); // return an empty string if no input was received
    }
}

void checkBuffer(){
    if(uartBuffer[0] == '\n'){
        _delay_ms(500);
        printf("Buffer: %s\n",uartBuffer);
        uartBuffer = "0000000000000000";
        fflush(stdout);
    }
}

int main(void) {
    init();
    char input;
    
    _delay_ms(2500);
    printf("Master init complete \n");
    fflush(stdout);
    
    while(1) {
        char *inputString = malloc(sizeof(char)*9);
        uart_getstring(inputString,9);
        if(inputString[0] != '\0'){
            printf("Read: %s\n",inputString);
            fflush(stdout);
        }
    }
    
    return 0;
}