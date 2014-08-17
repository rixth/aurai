#include <util/delay.h>

#include <pins.h>
#include <SPI.h>
#include <DiagLEDS.h>
#include <Serial.h>
#include <DHT11.h>
#include <IRSend.h>
#include <NRF24L01.h>
#include <EEPROM.h>

#include <spoke.h>

int main() {
  // IRSend_init();
  Serial.begin();
  SPI.begin();
  DiagLEDS_init();
  NRF24_init();

  while (1) {
    mainTest(Serial.read());
  }

  return 0;
}

void mainTest(uint8_t input) {
  NRF24_test(input);
  IRSend_test();
  DHT11_test();
  EEPROM.test(input);
  DiagLEDS_test(input);
}
