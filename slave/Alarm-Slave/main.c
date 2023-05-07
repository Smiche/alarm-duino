#include <stdio.h>

#define F_CPU 16000000UL        // Speed of the microprocessor, in hertz (16 MHz)
#include "avr/io.h"
#include "util/delay.h"
#include "main.h"
#include "uart.h"
#include "keypad.h"

// macros
#define TRIGGER      DDB0 // Pin for the obstacle sensor

// globals
int oldTriggerVal = 0;
char *keyBuffer = "00000";

// Initialization method
void init()
{
    DDRB = DDRB & (~(1<<TRIGGER));		/* Make TRIGGER as input pin */
    
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;
}

void shiftBuffer(char newVal){
    for(int i = 3; i > -1; i--){
        keyBuffer[i+1] = keyBuffer[i];
    }
    keyBuffer[0] = newVal;
}

void protocolOut(){
    printf("$%d#%s\n",oldTriggerVal,keyBuffer);
    fflush(stdout);
}

int main(void) {
    init();
    KEYPAD_Init();
    
    _delay_ms(3000);
    fflush(stdout);
    
    while(1) {
        // Checking the obstacle sensor
        int triggerVal = PINB & (1 << TRIGGER);
        if(triggerVal != oldTriggerVal) {
            oldTriggerVal = triggerVal;
            protocolOut();
        }
        
        // Checking for keypad presses
        uint8_t keyPressed = KEYPAD_GetKey();
        if(keyPressed != 0){
            shiftBuffer(keyPressed);
            protocolOut();
        }
    }
    
    return 0;
}