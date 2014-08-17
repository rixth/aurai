#include <stdint.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <SPI.h>
#include <pins.h>
#include <EEPROM.h>

#ifndef EEPROM_CS_High
#define EEPROM_CS_High() EEPROM_CS_PORT |= EEPROM_CS_MASK;
#define EEPROM_CS_Low()  EEPROM_CS_PORT &= ~EEPROM_CS_MASK;
#endif

void EEPROM_init() {
  EEPROM_CS_DDR |= EEPROM_CS_MASK;
  EEPROM_CS_High();
}

uint8_t EEPROM_read(uint32_t addr) {
  uint8_t buf[1];
  EEPROM_read(addr, buf, 1);
  return buf[0];
}

void EEPROM_read(uint32_t addr, uint8_t *buf, uint16_t count) {
  uint8_t i;

  EEPROM_CS_Low();
  SPI.transfer(EEPROM_INSTR_READ);
  SPI.transfer(addr >> 16);
  SPI.transfer(addr >> 8);
  SPI.transfer(addr);

  for(i = 0; i < count; i++) {
    *buf++ = SPI.transfer(0);
  }

  EEPROM_CS_High();
}

void EEPROM_write(uint32_t addr, uint8_t byte) {
  EEPROM_CS_Low();
  SPI.transfer(EEPROM_INSTR_WREN);
  EEPROM_CS_High();
  _delay_us(10);
  EEPROM_CS_Low();
  SPI.transfer(EEPROM_INSTR_WRITE);
  SPI.transfer(addr >> 16);
  SPI.transfer(addr >> 8);
  SPI.transfer(addr);
  SPI.transfer(byte);
  EEPROM_CS_High();

  EEPROM__loop_until_written();
}

void EEPROM_write(uint32_t addr, uint8_t *bytes, uint16_t count) {
  uint8_t i;

  EEPROM_CS_Low();
  SPI.transfer(EEPROM_INSTR_WREN);
  EEPROM_CS_High();
  _delay_us(10);
  EEPROM_CS_Low();
  SPI.transfer(EEPROM_INSTR_WRITE);
  SPI.transfer(addr >> 16);
  SPI.transfer(addr >> 8);
  SPI.transfer(addr);

  for(i = 0; i < count; i++) {
    SPI.transfer(bytes[i]);
  }

  EEPROM_CS_High();

  EEPROM__loop_until_written();
}

void EEPROM__loop_until_written() {
  while (1) {
    EEPROM_CS_Low();
    SPI.transfer(EEPROM_INSTR_RDSR);
    uint8_t status = SPI.transfer(0);
    EEPROM_CS_High();
    if (bit_is_clear(status, 0)) {
      break;
    }
  }
}
