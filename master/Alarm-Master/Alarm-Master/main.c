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
uint8_t timeoutPlayed = 0;
char *password = "D4321";

void init(){
    init_millis(F_CPU); //frequency the atmega328p is running at
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;
    lastEntry = millis();

    chipTunes_Init();
}

int validateProtocol(char *msg){
    if(msg[0] == '$' && msg[2] == '#' && strlen(msg) == 8){
        return 1;
    }
    return 0;
}

void handlePinChange(char *newEntry, char *lastEntry)
{

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
            
            switch(controllerState)
            {
                case DISARMED:
                if(inputString[3] == 'A'){
                    timeoutPlayed = 1;
                    controllerState = ARMED;
                    printf("Alarm armed.\n");
                    fflush(stdout);
                }
                break;
                case ARMED:
                if(strcmp(digits, password) == 0 && (lastEntry + timeout) > millis()){
                    controllerState = DISARMED;
                    printf("Alarm disarmed.\n");
                    fflush(stdout);
                }
                if(inputString[1] == '0'){
                    controllerState = ALARMING;
                }
                break;
                case ALARMING:
                if(strcmp(digits, password) == 0){
                    controllerState = DISARMED;
                    printf("Alarm disarmed.\n");
                    fflush(stdout);
                }
                break;
            }
            
            if(strcmp(digits, lastDigits) != 0){
                lastEntry = millis();
                timeoutPlayed = 0;
            }
        }
        
        if((lastEntry + timeout) < millis() && !timeoutPlayed){
            //timeout for pin entry
            chipTunes_Tone(NOTE_A5, 500);
            timeoutPlayed = 1;
            printf("Pin entry timeout.\n");
            fflush(stdout);
            } else if(controllerState == ALARMING){
            printf("ALARMING.\n");
            fflush(stdout);
            chipTunes_Tone(NOTE_A2, 500);
        }
    }
    
    return 0;
}