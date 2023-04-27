#include <stdio.h>

#define F_CPU 16000000UL        // Speed of the microprocessor, in hertz (16 MHz)
#include "avr/io.h"
#include <string.h>
#include "util/delay.h"
#include "main.h"
#include "uart.h"

// macros
#define PROTOCOL_LENGTH 8

// globals
enum State {DISARMED=0, ARMED=1, ALARMING=2};

void init(){
    init_millis(F_CPU); //frequency the atmega328p is running at
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;
}

int validateProtocol(char *msg){
    if(msg[0] == '$' && msg[2] == '#' && strlen(msg) == 8){
        return 1;
    }
    return 0;
}

int main(void) {
    init();
    
    _delay_ms(2500);
    printf("Master init complete \n");
    fflush(stdout);
    
    char *inputString = malloc(sizeof(char)*9);
    char *lastDigits = "-----";
    char *lastMotion = '1';
    
    while(1) {
        uart_getstring(inputString,9);
        
        if(validateProtocol(inputString)){ // new input from slave
            char *digits = malloc(sizeof(char) * 6);
            strncpy(digits, &inputString[3], 6);
            printf("Read: %s\n",digits);
            fflush(stdout);
        }
    }
    
    
    return 0;
}