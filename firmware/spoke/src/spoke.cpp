#include <util/delay.h>

#include <pins.h>
#include <SPI.h>
#include <DiagLEDS.h>
#include <Serial.h>
#include <EEPROM.h>
#include <DHT11.h>
#include <IRSend.h>
#include <NRF24L01.h>

#include <spoke.h>

int main() {
  Serial.begin();
  EEPROM_init();
  IRSend_init();
  SPI.begin();
  DiagLEDS_init();
  NRF24_init();

  // Let components (esp. radio) wake up
  _delay_ms(100);

  RED_LED_ToOutput();
  GRN_LED_ToOutput();
  YLW_LED_ToOutput();

  while (1) {
    mainTest(Serial.read());
  }

  return 0;
}

void mainTest(uint8_t input) {
  NRF24_test(input);
  EEPROM_test(input);
  IRSend_test();
  DHT11_test();
  DiagLEDS_test(input);
}
