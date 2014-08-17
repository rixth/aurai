#include <avr/io.h>
#include <avr/sfr_defs.h>

#include <SPI.h>

void NativeSPI::begin() {
  SS_DDR |= SS_MASK;
  SS_PORT |= SS_MASK;

  SPCR = _BV(SPE) | _BV(MSTR);

  SCK_DDR |= SCK_MASK;
  MOSI_DDR |= MOSI_MASK;
  MISO_DDR &= ~(MISO_MASK);
}

void NativeSPI::end() {
  SPCR &= ~(_BV(SPE));
}

void NativeSPI::setBitOrder(uint8_t order) {
  if (order == SPI_LSBFIRST) {
    SPCR |= _BV(DORD);
  } else {
    SPCR &= ~(_BV(DORD));
  }
}

void NativeSPI::setClockDivider(uint8_t divider) {
  SPCR = (SPCR & ~(3)) | divider;
}

void NativeSPI::setDataMode(uint8_t mode) {
  SPCR = (SPCR & ~(12)) | mode;
}

void NativeSPI::setUse2x(bool use2x) {
  if (use2x) {
    SPSR |= _BV(SPI2X);
  } else {
    SPSR &= ~(_BV(SPI2X));
  }
}

uint8_t NativeSPI::transfer(uint8_t val) {
  SPDR = val;
  loop_until_bit_is_set(SPSR, SPIF);
  return SPDR;
}
