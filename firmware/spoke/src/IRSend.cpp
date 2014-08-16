#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <stdint.h>
#include <IRSend.h>
#include <pins.h>
#include <SerialIO.h>

void IRSend_init() {
}

void IRSend__on() {
  // CTC mode. Toggle OC2B on match
  TCCR2A = _BV(COM2B0) | _BV(WGM21);

  // Set overflow
  OCR2A = IRSend_OCR;

  // Start counter again
  TCNT2 = 0;

  IRLED_ToOutput();

  // Turn on by enabling prescaler
  TCCR2B = IRSend_2B_REG;
}

void IRSend__off() {
  // Turn off by clearing TCCR2B
  TCCR2B = 0;
  IRLED_ToInput();
}

void IRSend_send(uint32_t data, uint8_t len) {
  // Preamble
  IRSend__on();
  _delay_us(NEC_PREAMBLE_MARK);
  IRSend__off();
  _delay_us(NEC_PREAMBLE_SPACE);

  // Data
  uint8_t i;
  for (i = 0; i < len; i++) {
    IRSend__on();
    _delay_us(NEC_BITSTART_MARK);
    IRSend__off();

    _delay_us(data & 0x80000000 ? NEC_SPACE_1 : NEC_SPACE_0);
    data <<= 1;
  }

  // Stop
  IRSend__on();
  _delay_us(NEC_BITSTART_MARK);
  IRSend__off();
}

