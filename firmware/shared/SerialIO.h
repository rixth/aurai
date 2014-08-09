#define BAUD 9600

#include <stdint.h>
#include <util/setbaud.h>

void SerialIO_Init();
void SerialIO_put(uint8_t byte);
void SerialIO_puts(const char str[]);
void SerialIO_putb(uint8_t byte);
void SerialIO_putbin(uint8_t byte);
uint8_t SerialIO_recv();
