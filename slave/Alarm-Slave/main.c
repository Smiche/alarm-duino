#include <stdio.h>

#define F_CPU 16000000UL        // Speed of the microprocessor, in hertz (16 MHz)
#include "avr/io.h"
#include "avr/delay.h"
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
    //DDRB |= (1 << DDB1);     // set pin 3 of Port B as output

    //PORTB = PORTB | (1<<TRIGGER);		/* Enable pull-up on PD2 by writing 1 to it */
    //DDRB = DDRB & (~((1<<DDB1) | (1<<DDB2) | (1<<DDB3) | (1<<DDB4))); /* Keypad columns as inputs */
    //PORTB = PORTB | (1<<PINB1) | (1<<PINB2) | (1<<PINB3) | (1<<PINB4)); /* Keypad columns as pull-ups */
    
    //DDRC |= (1 << DDC4) | (1 << DDC5); /* Keypad rows as outputs on C pins*/
    //DDRB |= (1 << DDB3) | (1 << DDB4); /* Keypad rows as outputs on B pins */
    //DDRC = DDRC & (~((1<<DDC4) | (1<<DDC5))); // rows to low
    // Row pins to high
    //PORTC |= (1 << PINC4) | (1 << PINC5);
    //PORTB |= (1 << PINB3) | (1 << PINB4);
    
    // UART input/output initialization
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
    //printf("Init complete. \n");
    
    _delay_ms(3000);
    fflush(stdout);

    //DDRB |= (1 << DDB3) | (1 << DDB4); // DDB3 /4 outputs
    //PORTB &= (~((1<<PINB3) | (1<<PINB4)));// both to low
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