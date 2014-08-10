#include <SerialIO.h>
#include <avr/io.h>

// UDRn – USART I/O Data Register n
// UCSRnA – USART Control and Status Register n A
// UCSRnB – USART Control and Status Register n B
// UCSRnC – USART Control and Status Register n C
// UBRRnL and UBRRnH – USART Baud Rate Registers

void SerialIO_Init() {
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

void SerialIO_put(uint8_t byte) {
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = byte;
}

void SerialIO_puts(const char str[]) {
  uint8_t i = 0;
  while (str[i]) {
    SerialIO_put(str[i++]);
  }
}

void SerialIO_putb(uint8_t byte) {
  if (byte >= 100) {
    SerialIO_put('0' + (byte / 100));
  }
  if (byte >= 10) {
    SerialIO_put('0' + ((byte / 10) % 10));
  }
  SerialIO_put('0' + (byte % 10));
}

void SerialIO_putbin(uint8_t byte) {
  uint8_t bit;

  for (bit = 7; bit < 255; bit--) {
    SerialIO_put(bit_is_set(byte, bit) ? '1' : '0');
  }
}

bool SerialIO_hasData() {
  return bit_is_set(UCSR0A, RXC0);
}

uint8_t SerialIO_recv() {
  loop_until_bit_is_set(UCSR0A, RXC0);
  return UDR0;
}