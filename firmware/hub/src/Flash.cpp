#include <stdint.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <SPI.h>
#include <pins.h>
#include <Flash.h>
#include <Serial.h>

#ifndef FLASH_CS_High
#define FLASH_CS_High() FLASH_CS_PORT |= FLASH_CS_MASK;
#define FLASH_CS_Low()  FLASH_CS_PORT &= ~FLASH_CS_MASK;
#endif

void Flash_init() {
  FLASH_CS_DDR |= FLASH_CS_MASK;
  FLASH_CS_High();
}

uint8_t Flash_read(uint32_t addr) {
  uint8_t buf[1];
  Flash_read(addr, buf, 1);
  return buf[0];
}

void Flash_read(uint32_t addr, uint8_t *buf, uint16_t count) {
  uint8_t i;

  FLASH_CS_Low();
  SPI.transfer(FLASH_INSTR_READ);
  SPI.transfer(addr >> 16);
  SPI.transfer(addr >> 8);
  SPI.transfer(addr);

  for(i = 0; i < count; i++) {
    *buf++ = SPI.transfer(0);
  }

  FLASH_CS_High();
}

void Flash_write(uint32_t addr, uint8_t byte) {
  FLASH_CS_Low();
  SPI.transfer(FLASH_INSTR_WREN);
  FLASH_CS_High();
  _delay_us(10);
  FLASH_CS_Low();
  SPI.transfer(FLASH_INSTR_WRITE);
  SPI.transfer(addr >> 16);
  SPI.transfer(addr >> 8);
  SPI.transfer(addr);
  SPI.transfer(byte);
  FLASH_CS_High();

  Flash__loop_until_written();
}

void Flash_write(uint32_t addr, uint8_t *bytes, uint16_t count) {
  uint8_t i;

  FLASH_CS_Low();
  SPI.transfer(FLASH_INSTR_WREN);
  FLASH_CS_High();
  _delay_us(10);
  FLASH_CS_Low();
  SPI.transfer(FLASH_INSTR_WRITE);
  SPI.transfer(addr >> 16);
  SPI.transfer(addr >> 8);
  SPI.transfer(addr);

  for(i = 0; i < count; i++) {
    SPI.transfer(bytes[i]);
  }

  FLASH_CS_High();

  Flash__loop_until_written();
}

void Flash__loop_until_written() {
  while (1) {
    FLASH_CS_Low();
    SPI.transfer(FLASH_INSTR_RDSR);
    uint8_t status = SPI.transfer(0);
    FLASH_CS_High();
    if (bit_is_clear(status, 0)) {
      break;
    }
  }
}

void Flash_test(uint8_t input) {
  Serial.print("\r\n -- FLASH TEST -- \r\n");

  int addr = 0;
  uint8_t data[3] = { input, static_cast<uint8_t>(input + 1), static_cast<uint8_t>(input + 2) };

  Serial.print("Writing 3 bytes: ");
  Serial.putb(data[0]);
  Serial.print(" - ");
  Serial.putb(data[1]);
  Serial.print(" - ");
  Serial.putb(data[2]);
  Serial.print("\r\n");

  Flash_write(addr, data, 3);

  Serial.print("Reading 3 bytes: ");
  Flash_read(addr, data, 3);
  Serial.putb(data[0]);
  Serial.print(" - ");
  Serial.putb(data[1]);
  Serial.print(" - ");
  Serial.putb(data[2]);
  Serial.print("\r\n");
}
