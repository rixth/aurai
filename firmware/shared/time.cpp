#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>

#include "time.h"

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

ISR(TIMER0_OVF_vect) {
  // copy these to local variables so they can be stored in registers
  // (volatile variables must be read from memory on every access)
  unsigned long m = timer0_millis;
  unsigned char f = timer0_fract;

  m += MILLIS_INC;
  f += FRACT_INC;
  if (f >= FRACT_MAX) {
    f -= FRACT_MAX;
    m += 1;
  }

  timer0_fract = f;
  timer0_millis = m;
  timer0_overflow_count++;
}

void Time_init() {
  sei();

  // use fastpwm mode
  TCCR0A |= _BV(WGM01) | _BV(WGM00);

  // set timer 0 prescale factor to 64
  TCCR0B |= _BV(CS01) | _BV(CS00);

  // enable timer 0 overflow interrupt
  TIMSK0 |= _BV(TOIE0);
}

unsigned long Time_micros() {
    unsigned long m;
    uint8_t oldSREG = SREG, t = TCNT0;

    cli();
    m = timer0_overflow_count;

    if ((TIFR0 & _BV(TOV0)) && (t & 255)) {
      m++;
    }

    SREG = oldSREG;

    return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}