#include <stdint.h>

#define SS_PORT  PORTB
#define SS_DDR   DDRB
#define SS_MASK  (1 << 2)

#define SCK_DDR  DDRB
#define SCK_MASK (1 << 5)

#define MOSI_DDR  DDRB
#define MOSI_MASK (1 << 3)

#define MISO_DDR  DDRB
#define MISO_MASK (1 << 4)

#define SPI_CLOCK_DIV4   0
#define SPI_CLOCK_DIV16  1
#define SPI_CLOCK_DIV64  2
#define SPI_CLOCK_DIV128 3

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

#define SPI_LSBFIRST 1
#define SPI_MSBFIRST 2

void SPI_begin();
void SPI_end();
void SPI_setBitOrder(uint8_t order);
void SPI_setClockDivider(uint8_t divider);
void SPI_setDataMode(uint8_t mode);
void SPI_setUse2x(bool use2x);
uint8_t SPI_transfer(uint8_t val);