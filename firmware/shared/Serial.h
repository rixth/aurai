#define BAUD 9600

#include <stdint.h>
#include <util/setbaud.h>

class HardwareSerial {
public:
  static void begin();
  static bool available();
  static uint8_t read();
  static void print(const char str[]);
  static void println(const char str[]);
  static void print(uint8_t byte);
  static void putb(uint8_t byte);
  static void puth(uint8_t byte);
  static void putbin(uint8_t byte);
};

extern HardwareSerial Serial;