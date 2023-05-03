
/*
The millis() function known from Arduino
Calling millis() will return the milliseconds since the program started
Tested on atmega328p
Using content from http://www.adnbr.co.uk/articles/counting-milliseconds
Author: Monoclecat, https://github.com/monoclecat/avr-millis-function
REMEMBER: Add sei(); after init_millis() to enable global interrupts!
 */

#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>

#include "millis.h"

ISR(TIMER2_COMPA_vect)
{
  if(chipTunes_IsPlaying()){
    chipTunes_ISR();
  }
  timer1_millis++;
}

void init_millis(unsigned long f_cpu)
{
  unsigned long ctc_match_overflow;

  ctc_match_overflow = ((f_cpu / 1000) / 8); //when timer1 is this value, 1ms has passed

  // (Set timer to clear when matching ctc_match_overflow) | (Set clock divisor to 8)
  TCCR2B |= (1 << WGM12) | (1 << CS11);

  // high byte first, then low byte
  //OCR1AH
  //OCR2B = (ctc_match_overflow >> 8);
  OCR2A = ctc_match_overflow;

  // Enable the compare match interrupt
  TIMSK2 |= (1 << OCIE2A);

  //REMEMBER TO ENABLE GLOBAL INTERRUPTS AFTER THIS WITH sei(); !!!
}

unsigned long millis (void)
{
  unsigned long millis_return;

  // Ensure this cannot be disrupted
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    millis_return = timer1_millis;
  }
  return millis_return;
}