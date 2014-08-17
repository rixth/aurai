#include <util/delay.h>

#include <pins.h>
#include <SPI.h>
#include <DiagLEDS.h>
#include <Serial.h>
#include <EEPROM.h>
#include <NRF24L01.h>

#include <hub.h>

int main() {
  EEPROM_init();
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
  EEPROM_test(input);
  DiagLEDS_test(input);
}
