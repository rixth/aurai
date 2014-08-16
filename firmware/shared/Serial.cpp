#include <Serial.h>
#include <avr/io.h>
#include <stdlib.h>

// UDRn – USART I/O Data Register n
// UCSRnA – USART Control and Status Register n A
// UCSRnB – USART Control and Status Register n B
// UCSRnC – USART Control and Status Register n C
// UBRRnL and UBRRnH – USART Baud Rate Registers

void HardwareSerial::begin() {
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

bool HardwareSerial::available() {
  return bit_is_set(UCSR0A, RXC0);
}

uint8_t HardwareSerial::read() {
  loop_until_bit_is_set(UCSR0A, RXC0);
  return UDR0;
}

void HardwareSerial::print(uint8_t byte) {
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = byte;
}

void HardwareSerial::print(const char str[]) {
  uint8_t i = 0;
  while (str[i]) {
    print(str[i++]);
  }
}

void HardwareSerial::println(const char str[]) {
  print(str);
  print("\r\n");
}

void HardwareSerial::putb(uint8_t byte) {
  if (byte >= 100) {
    print('0' + (byte / 100));
  }
  if (byte >= 10) {
    print('0' + ((byte / 10) % 10));
  }
  print('0' + (byte % 10));
}

void HardwareSerial::puth(uint8_t byte) {
  char s[3];
  itoa(byte, s, 16);
  print("0x");
  if (s[1] == NULL) {
    print("0");
  }
  print(s);
}

void HardwareSerial::putbin(uint8_t byte) {
  uint8_t bit;

  for (bit = 7; bit < 255; bit--) {
    print(bit_is_set(byte, bit) ? '1' : '0');
  }
}