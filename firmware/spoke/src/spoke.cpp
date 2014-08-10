#include <util/delay.h>

#include <pins.h>
#include <SPI.h>
#include <SerialIO.h>
#include <EEPROM.h>
#include <DHT11.h>
#include <IRSend.h>

#include <spoke.h>

int main() {
  SerialIO_Init();
  SPI_begin();
  EEPROM_init();
  IRSend_init();

  RED_LED_ToOutput();
  GRN_LED_ToOutput();
  YLW_LED_ToOutput();

  while (1) {
    mainTest(SerialIO_recv());
  }

  return 0;
}

void testEEPROM(uint8_t input) {
  SerialIO_puts("\n -- EEPROM TEST -- \n");

  int addr = 0;
  uint8_t data[3] = { input, input + 1, input + 2 };

  SerialIO_puts("Writing 3 bytes: ");
  SerialIO_putb(data[0]);
  SerialIO_puts(" - ");
  SerialIO_putb(data[1]);
  SerialIO_puts(" - ");
  SerialIO_putb(data[2]);
  SerialIO_puts("\n");

  EEPROM_write(addr, data, 3);

  SerialIO_puts("Reading 3 bytes: ");
  uint8_t *d = EEPROM_read(addr, 3);
  SerialIO_putb(d[0]);
  SerialIO_puts(" - ");
  SerialIO_putb(d[1]);
  SerialIO_puts(" - ");
  SerialIO_putb(d[2]);
  SerialIO_puts("\n");
}

void testIRLED(uint8_t input) {
  SerialIO_puts("\n -- IR-LED TEST -- \n");
  IRSend_send(0x8166817E, 32);
  SerialIO_puts("AC should have toggled power\n");
}

void testDHT11(uint8_t input) {
  SerialIO_puts("\n -- DHT-11 TEST -- \n");

  uint8_t r = DHT11_readSensor();
  SerialIO_puts("Read result: ");
  SerialIO_putb(r);
  SerialIO_puts(" Temp: ");
  SerialIO_putb(DHT11_temperature());
  SerialIO_puts("C Humidity: ");
  SerialIO_putb(DHT11_humidity());
  SerialIO_puts("%\n");
}

void testDIAGLEDS(uint8_t input) {
  SerialIO_puts("\n -- DIAG LED TEST -- \n");

  if (input == '1') {
    RED_LED_High();
    _delay_ms(100);
    RED_LED_Low();
  } else if (input == '2') {
    YLW_LED_High();
    _delay_ms(100);
    YLW_LED_Low();
  } else if (input == '3') {
    GRN_LED_High();
    _delay_ms(100);
    GRN_LED_Low();
  }
}

void mainTest(uint8_t input) {
  testEEPROM(input);
  testIRLED(input);
  testDHT11(input);
  testDIAGLEDS(input);
}
