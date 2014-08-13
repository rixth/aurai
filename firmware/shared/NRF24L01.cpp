#include "SPI.h"
#include "NRF24L01.h"
#include "pins.h"

void NRF24_init() {
  NRF24_CE_ToOutput();
  NRF24_CSN_ToOutput();
  NRF24_CE_Low();
  NRF24_CSN_High();
}

uint8_t* NRF24_getRx(uint8_t len) {
  return NRF24_read(NRF24_CMD_R_RX_PAYLOAD, len);
}

void NRF24_setTx(uint8_t* data, uint8_t len) {
  NRF24_write(NRF24_CMD_W_TX_PAYLOAD, data, len);
}

uint8_t NRF24_getRegister(uint8_t addr) {
  return NRF24_read(NRF24_CMD_R_REGISTER | addr);
}

uint8_t* NRF24_getRegister(uint8_t addr, uint8_t len) {
  return NRF24_read(NRF24_CMD_R_REGISTER | addr, len);
}

void NRF24_setRegister(uint8_t addr, uint8_t val) {
  NRF24_write(NRF24_CMD_W_REGISTER | addr, val);
}

void NRF24_setRegisterAnd(uint8_t addr, uint8_t val) {
  uint8_t existing = NRF24_read(NRF24_CMD_W_REGISTER | addr);
  NRF24_write(NRF24_CMD_W_REGISTER | addr, existing & val);
}

void NRF24_setRegisterOr(uint8_t addr, uint8_t val) {
  uint8_t existing = NRF24_read(NRF24_CMD_W_REGISTER | addr);
  NRF24_write(NRF24_CMD_W_REGISTER | addr, existing | val);
}

void NRF24_setRegisterAndOr(uint8_t addr, uint8_t andVal, uint8_t orVal) {
  uint8_t existing = NRF24_read(NRF24_CMD_W_REGISTER | addr);
  NRF24_write(NRF24_CMD_W_REGISTER | addr, (existing & andVal) | orVal);
}

void NRF24_setRegister(uint8_t addr, uint8_t* val, uint8_t len) {
  NRF24_write(NRF24_CMD_W_REGISTER | addr, val, len);
}

uint8_t NRF24_read(uint8_t cmd) {
  NRF24_CSN_Low();
  SPI_transfer(cmd);
  uint8_t result = SPI_transfer(0);
  NRF24_CSN_High();
  return result;
}

uint8_t* NRF24_read(uint8_t cmd, uint8_t len) {
  uint8_t result[len];
  uint8_t i;
  NRF24_CSN_Low();
  SPI_transfer(cmd);
  for (i = 0; i < len; i++) {
    result[i] = SPI_transfer(0);
  }
  NRF24_CSN_High();
  return result;
}

uint8_t NRF24_write(uint8_t cmd) {
  NRF24_CSN_Low();
  uint8_t result = SPI_transfer(cmd);
  NRF24_CSN_High();
  return result;
}

void NRF24_write(uint8_t cmd, uint8_t val) {
  NRF24_CSN_Low();
  SPI_transfer(cmd);
  SPI_transfer(val);
  NRF24_CSN_High();
}

void NRF24_write(uint8_t cmd, uint8_t* val, uint8_t len) {
  uint8_t i;
  NRF24_CSN_Low();
  SPI_transfer(cmd);
  for (i = 0; i < len; i++) {
    SPI_transfer(val[i]);
  }
  NRF24_CSN_High();
}

