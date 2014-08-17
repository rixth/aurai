#include <avr/io.h>
#include <stdint.h>

#ifndef FLASH_CS_DDR
#define FLASH_CS_DDR        DDRD
#define FLASH_CS_PORT       PORTD
#define FLASH_CS_MASK       (1 << 3)
#endif

#define FLASH_INSTR_WREN  6 // Set Write Enable Latch
#define FLASH_INSTR_WRDI  4 // Reset Write Enable Latch
#define FLASH_INSTR_RDSR  5 // Read Status Register
#define FLASH_INSTR_WRSR  1 // Write Status Register
#define FLASH_INSTR_READ  3 // Read Data from Memory Array
#define FLASH_INSTR_WRITE 2 // Write Data to Memory Array

void Flash_init();
void Flash_write(uint32_t addr, uint8_t byte);
void Flash_write(uint32_t addr, uint8_t *bytes, uint16_t count);
uint8_t Flash_read(uint32_t addr);
void Flash_read(uint32_t addr, uint8_t *buf, uint16_t count);
void Flash__loop_until_written();
void Flash_test(uint8_t input);