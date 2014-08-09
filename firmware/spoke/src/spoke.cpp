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

  SerialIO_puts("\n\nSPI Test\n\n");
  _delay_ms(10);

  while (1) {
    int addr = 0;
    uint8_t byt = SerialIO_recv();

    SerialIO_puts("Writing: ");
    SerialIO_putb(byt);
    SerialIO_puts("\n");

    EEPROM_write(addr, byt);

    SerialIO_puts("Read: ");
    SerialIO_putb(EEPROM_read(addr));
    SerialIO_puts("\n");
  }

  return 0;
}
