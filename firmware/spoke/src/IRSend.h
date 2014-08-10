#include <stdint.h>

#ifndef IRSend_h
#define IRSend_h

// From datasheet: hz = F_CPU / (2 * PRESCALE * (1 + OCRnx))
#if F_CPU == 8000000L
  // Prescaler of /1 and OCR at 104 gives 38095hz
  // 8000000 / ( 2 * 1 * (1 + 104)) = 38095hz
  #define IRSend_2B_REG _BV(CS20)
  #define IRSend_OCR 104
#else
  #error Timer not configured for IRSend
#endif

#define NEC_PREAMBLE_MARK  9000
#define NEC_PREAMBLE_SPACE 4500
#define NEC_BITSTART_MARK  560
#define NEC_SPACE_1 1600
#define NEC_SPACE_0  560

void IRSend_init();
void IRSend_send(uint32_t data, uint8_t len);
void IRSend__on();
void IRSend__off();

#endif
