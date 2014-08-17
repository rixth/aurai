#include <EEPROM.h>
#include <Serial.h>
#include <stdint.h>
#include <avr/eeprom.h>

void InternalEEPROM::write(uint16_t addr, uint8_t val) {
  eeprom_write_byte((unsigned char *) addr, val);
}

uint8_t InternalEEPROM::read(uint16_t addr) {
  return eeprom_read_byte((unsigned char *) addr);
}

void InternalEEPROM::test(uint8_t input) {
  Serial.print("\r\n -- Internal EEPROM TEST -- \r\n");

  Serial.print("Writing 1 byte: ");
  Serial.putb(input);

  write(1023, input);

  Serial.print("\r\nReading 1 bytes: ");
  Serial.putb(read(1023));
  Serial.print("\r\n");
}


extern InternalEEPROM EEPROM;