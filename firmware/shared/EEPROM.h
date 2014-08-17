#include <avr/io.h>
#include <stdint.h>

#ifndef EEPROM_CS_DDR
#define EEPROM_CS_DDR        DDRD
#define EEPROM_CS_PORT       PORTD
#define EEPROM_CS_MASK       (1 << 3)
#endif

#define EEPROM_INSTR_WREN  6 // Set Write Enable Latch
#define EEPROM_INSTR_WRDI  4 // Reset Write Enable Latch
#define EEPROM_INSTR_RDSR  5 // Read Status Register
#define EEPROM_INSTR_WRSR  1 // Write Status Register
#define EEPROM_INSTR_READ  3 // Read Data from Memory Array
#define EEPROM_INSTR_WRITE 2 // Write Data to Memory Array

void EEPROM_init();
void EEPROM_write(uint32_t addr, uint8_t byte);
void EEPROM_write(uint32_t addr, uint8_t *bytes, uint16_t count);
uint8_t EEPROM_read(uint32_t addr);
void EEPROM_read(uint32_t addr, uint8_t *buf, uint16_t count);
void EEPROM__loop_until_written();
void EEPROM_test(uint8_t input);