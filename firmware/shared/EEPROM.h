#include <stdint.h>

class InternalEEPROM {
public:
  static void write(uint16_t addr, uint8_t val);
  static uint8_t read(uint16_t addr);
  static void test(uint8_t addr);
};

extern InternalEEPROM EEPROM;