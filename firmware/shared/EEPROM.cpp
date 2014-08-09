#include <stdint.h>
#include <avr/sfr_defs.h>
#include <avr/delay.h>
#include <SPI.h>
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
  EEPROM_CS_Low();
  SPI_transfer(EEPROM_INSTR_READ);
  SPI_transfer(addr >> 16);
  SPI_transfer(addr >> 8);
  SPI_transfer(addr);
  uint8_t result = SPI_transfer(0);
  EEPROM_CS_High();
  return result;
}

uint8_t* EEPROM_read(uint32_t addr, uint16_t count) {
  uint8_t data[count];
  int i;

  EEPROM_CS_Low();
  SPI_transfer(EEPROM_INSTR_READ);
  SPI_transfer(addr >> 16);
  SPI_transfer(addr >> 8);
  SPI_transfer(addr);

  for(i = 0; i < count; i++) {
    data[i] = SPI_transfer(0);
  }

  EEPROM_CS_High();
  return data;
}

void EEPROM_write(uint32_t addr, uint8_t byte) {
  EEPROM_CS_Low();
  SPI_transfer(EEPROM_INSTR_WREN);
  EEPROM_CS_High();
  _delay_us(10);
  EEPROM_CS_Low();
  SPI_transfer(EEPROM_INSTR_WRITE);
  SPI_transfer(addr >> 16);
  SPI_transfer(addr >> 8);
  SPI_transfer(addr);
  SPI_transfer(byte);
  EEPROM_CS_High();

  EEPROM__loop_until_written();
}

void EEPROM_write(uint32_t addr, uint8_t *bytes, uint16_t count) {
  int i;

  EEPROM_CS_Low();
  SPI_transfer(EEPROM_INSTR_WREN);
  EEPROM_CS_High();
  _delay_us(10);
  EEPROM_CS_Low();
  SPI_transfer(EEPROM_INSTR_WRITE);
  SPI_transfer(addr >> 16);
  SPI_transfer(addr >> 8);
  SPI_transfer(addr);

  for(i = 0; i < count; i++) {
    SPI_transfer(bytes[i]);
  }

  EEPROM_CS_High();

  EEPROM__loop_until_written();
}

void EEPROM__loop_until_written() {
  while (1) {
    EEPROM_CS_Low();
    SPI_transfer(EEPROM_INSTR_RDSR);
    uint8_t status = SPI_transfer(0);
    EEPROM_CS_High();
    if (bit_is_clear(status, 0)) {
      break;
    }
  }
}