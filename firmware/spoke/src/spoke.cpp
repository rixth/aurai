#include <avr/io.h>
#include <util/delay.h>

#include <pins.h>
#include <SPI.h>
#include <SerialIO.h>
#include <EEPROM.h>
#include <DHT11.h>

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

    uint8_t r = DHT11_readSensor();
    SerialIO_puts("Read result: ");
    SerialIO_putb(r);
    SerialIO_puts(" T: ");
    SerialIO_putb(DHT11_temperature());
    SerialIO_puts("C H: ");
    SerialIO_putb(DHT11_humidity());
    SerialIO_puts("%\n");

    _delay_ms(500);

    redOn ? RED_LED_High() : RED_LED_Low();
    yellowOn ? YLW_LED_High() : YLW_LED_Low();
    greenOn ? GRN_LED_High() : GRN_LED_Low();
  }

  return 0;
}
