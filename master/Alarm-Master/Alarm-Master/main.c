#define F_CPU 16000000UL        // Speed of the microprocessor, in hertz (16 MHz)

#include <stdio.h>
#include "avr/io.h"
#include <string.h>
#include "util/delay.h"
#include "main.h"
#include "uart.h"
#include "millis.h"
#include "chipTunes.h"

// macros
#define PROTOCOL_LENGTH 8

// globals
enum State {DISARMED=0, ARMED=1, ALARMING=2};
enum State controllerState = DISARMED;
uint32_t lastEntry = 0;
uint32_t timeout = 25000;
uint8_t timeoutPlayed = 1;
char *password = "D4321";

// Initialization for millis, uart and chipTunes libraries
void init(){
    init_millis(F_CPU); //frequency the atmega328p is running at
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;
    lastEntry = millis();

    chipTunes_Init();
}

// Method to validate a protocol message. Checks for correct length and delimiters.
int validateProtocol(char *msg){
    if(msg[0] == '$' && msg[2] == '#' && strlen(msg) == 8){
        return 1;
    }
    return 0;
}

int main(void) {
    init();
    
    _delay_ms(2500); // just a delay to give me time to reconnect serial console
    printf("Master init complete \n");
    fflush(stdout);
    
    char *inputString = malloc(sizeof(char)*9); // string holding last input string
    char *lastDigits = "00000"; // string holding the last keypad input
    char *lastMotion = '1'; // holding the last state of the motion sensor
    
    while(1) {
        uart_getstring(inputString,9); // periodic reading for input from Alarm-Slave
        
        if(validateProtocol(inputString)){ // new input from slave, consider only if it's valid
            // parsing the digits
            char *digits = malloc(sizeof(char) * 6);
            strncpy(digits, &inputString[3], 6);
            //printf("Read: %s\n",digits);
            //fflush(stdout);
            int skipTimeout = 0;
            
            // State machine
            switch(controllerState)
            {
                case DISARMED:
                if(inputString[3] == 'A'){ // 'A' is the arming key
                    timeoutPlayed = 1;
                    skipTimeout = 1;
                    controllerState = ARMED;
                    printf("Alarm armed.\n");
                    fflush(stdout);
                }
                break;
                case ARMED:
                if(strcmp(digits, password) == 0 && (lastEntry + timeout) > millis()){ // Disarm if password matches and no input timeout
                    controllerState = DISARMED;
                    skipTimeout = 1;
                    timeoutPlayed = 1;
                    printf("Alarm disarmed.\n");
                    fflush(stdout);
                }
                if(inputString[1] == '0'){ // if the motion sensor detects movement, switch to alarming
                    controllerState = ALARMING;
                }
                break;
                case ALARMING:
                if(strcmp(digits, password) == 0){ // password matches -> disarm
                    controllerState = DISARMED;
                    printf("Alarm disarmed.\n");
                    fflush(stdout);
                    skipTimeout = 1;
                    timeoutPlayed = 1;
                }
                break;
            }
            
            if(strcmp(digits, lastDigits) != 0 && skipTimeout == 0){ // tracking last pin input time
                lastEntry = millis();
                timeoutPlayed = 0;
            }
            
            strncpy(lastDigits, digits,6);
        }
        
        if((lastEntry + timeout) < millis() && !timeoutPlayed){
            // timeout for pin entry, play a note
            chipTunes_Tone(NOTE_A5, 500);
            timeoutPlayed = 1;
            printf("Pin entry timeout.\n");
            fflush(stdout);
            } else if(controllerState == ALARMING){
            // we're in alarming state, play alarm continuously
            chipTunes_Tone(NOTE_A2, 500);
        }
    }
    
    return 0;
}