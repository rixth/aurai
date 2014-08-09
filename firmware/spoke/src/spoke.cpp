#include <avr/io.h>
#include <util/delay.h>

#include <pins.h>
#include <SPI.h>
#include <SerialIO.h>
#include <EEPROM.h>

int main() {
  SerialIO_Init();
  SPI_begin();
  EEPROM_init();

  bool redOn = false;
  bool yellowOn = false;
  bool greenOn = false;

  RED_LED_ToOutput();
  GRN_LED_ToOutput();
  YLW_LED_ToOutput();

  while (1) {
    if (SerialIO_hasData()) {
      uint8_t byt = SerialIO_recv();
      if (byt == '1') redOn = !redOn;
      if (byt == '2') yellowOn = !yellowOn;
      if (byt == '3') greenOn = !greenOn;
    }

    redOn ? RED_LED_High() : RED_LED_Low();
    yellowOn ? YLW_LED_High() : YLW_LED_Low();
    greenOn ? GRN_LED_High() : GRN_LED_Low();
  }

  return 0;
}
