#include "SPI.h"
#include "SerialIO.h"
#include "NRF24L01.h"
#include "pins.h"
#include "util/delay.h"

void NRF24_init() {
  NRF24_CE_ToOutput();
  NRF24_CSN_ToOutput();
  NRF24_CE_Low();
  NRF24_CSN_High();
}

void NRF24_configure() {
  NRF24_setRegister(NRF24_REG_RF_CH, 1);
  NRF24_setRegister(NRF24_REG_RX_PW_P0, sizeof(uint8_t));
  NRF24_setRegister(NRF24_REG_RX_PW_P1, sizeof(uint8_t));
  NRF24_setRegister(NRF24_REG_CONFIG, (NRF24_PWR_UP | NRF24_EN_CRC) & ~NRF24_CRCO);
}

void NRF24_setTxAddr(const uint8_t* data, uint8_t len) {
  NRF24_setRegister(NRF24_REG_RX_ADDR_P0, data, 5);
  NRF24_setRegister(NRF24_REG_TX_ADDR, data, 5);
}

void NRF24_setRxAddr(const uint8_t* data, uint8_t len) {
  NRF24_setRegister(NRF24_REG_RX_ADDR_P1, data, 5);
}

void NRF24_rxMode() {
  NRF24_CE_Low();
  NRF24_setRegisterOr(NRF24_REG_CONFIG, NRF24_PRIM_RX);
  NRF24_setRegister(NRF24_REG_STATUS, (NRF24_RX_DR) | (NRF24_TX_DS) | (NRF24_MAX_RT));
  NRF24_CE_High();
  NRF24_write(NRF24_CMD_FLUSH_RX);
}

bool NRF24_send(const uint8_t* val, uint8_t len) {
  NRF24_CE_Low();

  // Power up tx
  NRF24_setRegisterAnd(NRF24_REG_CONFIG, ~NRF24_PRIM_RX);

  // Set payload
  NRF24_write(NRF24_CMD_W_TX_PAYLOAD, val, len);

  // Pulse CE
  NRF24_CE_High();
  _delay_us(15);
  NRF24_CE_Low();

  while (1) {
    uint8_t status = NRF24_write(NRF24_CMD_NOP);
    if(status & NRF24_TX_DS || status & NRF24_MAX_RT) {
      NRF24_rxMode();
      if (status & NRF24_TX_DS) {
        return true;
      } else {
        return false;
      }
    }
  }
}

bool NRF24_dataAvailable() {
  // We can short circuit on RX_DR, but if it's not set, we still need
  // to check the FIFO for any pending packets
  if (NRF24_write(NRF24_CMD_NOP) & NRF24_RX_DR) {
    return true;
  }

  uint8_t fifoStatus = NRF24_getRegister(NRF24_REG_FIFO_STATUS);
  return !(fifoStatus & NRF24_RX_EMPTY);
}

void NRF24_printConfig() {
  uint8_t config = NRF24_getRegister(NRF24_REG_CONFIG);
  uint8_t rfCh = NRF24_getRegister(NRF24_REG_RF_CH);
  uint8_t rfSetup = NRF24_getRegister(NRF24_REG_RF_SETUP);
  uint8_t aw = NRF24_getRegister(NRF24_REG_SETUP_AW);

  SerialIO_puts("Config: ");
  SerialIO_putbin(config);
  SerialIO_puts(" ");
  if (!(config & NRF24_MASK_RX_DR)) SerialIO_puts("MASK_RX_DR ");
  if (!(config & NRF24_MASK_TX_DS)) SerialIO_puts("MASK_TX_DS ");
  if (!(config & NRF24_MASK_MAX_RT)) SerialIO_puts("MASK_MAX_RT ");
  if (config & NRF24_EN_CRC) SerialIO_puts("EN_CRC ");
  if (config & NRF24_CRCO) {
    SerialIO_puts("CRC-2 ");
  } else {
    SerialIO_puts("CRC-1 ");
  }
  if (config & NRF24_PWR_UP) SerialIO_puts("PWR_UP ");
  if (config & NRF24_PRIM_RX) SerialIO_puts("PRIM_RX ");
  SerialIO_puts("\r\n");

  SerialIO_puts("RF setup: ");
  SerialIO_putbin(rfSetup);
  SerialIO_puts(" CH: ");
  SerialIO_putb(rfCh);
  SerialIO_puts(" ");
  if (rfSetup & NRF24_CONT_WAVE) SerialIO_puts("CONT_WAVE ");
  if (rfSetup & NRF24_PLL_LOCK) SerialIO_puts("PLL_LOCK ");
  if (rfSetup & NRF24_RF_DR_250KB) {
    SerialIO_puts("250kbps ");
  } else if (rfSetup & NRF24_RF_DR_2MBPS) {
    SerialIO_puts("2mbps ");
  } else {
    SerialIO_puts("1mbps ");
  }
  if ((rfSetup & NRF24_RF_PWR0) && (rfSetup & NRF24_RF_PWR1)) {
    SerialIO_puts("0dbm ");
  } else if (!(rfSetup & NRF24_RF_PWR0) && (rfSetup & NRF24_RF_PWR1)) {
    SerialIO_puts("-6dbm ");
  } else if ((rfSetup & NRF24_RF_PWR0) && !(rfSetup & NRF24_RF_PWR1)) {
    SerialIO_puts("-12dbm ");
  } else if (!(rfSetup & NRF24_RF_PWR0) && !(rfSetup & NRF24_RF_PWR1)) {
    SerialIO_puts("-18dbm ");
  } else {
    SerialIO_puts("-XXdbm ");
  }
  SerialIO_puts("\r\n");

  SerialIO_puts("AW setup: ");
  SerialIO_putbin(aw);
  if (aw & NRF24_AW_5BITS) {
    SerialIO_puts(" 5 bytes\r\n");
  } else if (aw & NRF24_AW_4BITS) {
    SerialIO_puts(" 4 bytes\r\n");
  } else if (aw & NRF24_AW_3BITS) {
    SerialIO_puts(" 3 bytes\r\n");
  } else {
    SerialIO_puts(" X bytes\r\n");
  }
}

void NRF24_printAddresses() {
  uint8_t dynPld = NRF24_getRegister(NRF24_REG_DYNPD);
  uint8_t enRx = NRF24_getRegister(NRF24_REG_EN_RXADDR);
  uint8_t enAa = NRF24_getRegister(NRF24_REG_EN_AA);
  uint8_t i, j;
  uint8_t addr[5];

  SerialIO_puts("TX  Address: ");
  NRF24_getRegister(NRF24_REG_TX_ADDR, addr, 5);
  for(i = 0; i < 5; i++) {
    SerialIO_puth(addr[i]);
    SerialIO_puts(" ");
  }
  SerialIO_puts("\r\n");

  for (i = 0; i < 6; i++) {
    if (enRx & _BV(i)) {
      SerialIO_puts("RX");
      SerialIO_putb(i);
      SerialIO_puts(" Address: ");

      if (i == 0 || i == 1) {
        NRF24_getRegister(NRF24_REG_RX_ADDR_P0 + i, addr, 5);
        for(j = 0; j < 5; j++) {
          SerialIO_puth(addr[j]);
          SerialIO_puts(" ");
        }
      } else {
        NRF24_getRegister(NRF24_REG_RX_ADDR_P0 + i, addr, 1);
        SerialIO_puth(addr[0]);
        SerialIO_puts(" ");
      }

      SerialIO_puts("Payload: ");
      if (dynPld & _BV(i)) {
        SerialIO_puts("dynamic.");
      } else {
        SerialIO_putb(NRF24_getRegister(NRF24_REG_RX_PW_P0 + i));
        SerialIO_puts(" bytes.");
      }

      SerialIO_puts((enAa & _BV(i)) ? " AA on. \r\n" : " AA off. \r\n");
    }
  }
}

void NRF24_printStatus() {
  uint8_t status = NRF24_write(NRF24_CMD_NOP);
  SerialIO_puts("Status: ");
  SerialIO_putbin(status);
  SerialIO_puts(" ");
  if (status & NRF24_ST_TX_FULL) SerialIO_puts("ST_TX_FULL ");
  if (status & NRF24_MAX_RT) SerialIO_puts("MAX_RT ");
  if (status & NRF24_TX_DS) SerialIO_puts("TX_DS ");
  if (status & NRF24_RX_DR) SerialIO_puts("RX_DR ");
  SerialIO_puts(" RX_P_NO: ");
  SerialIO_putb((status & NRF24_RX_P_NO) > 1);
  SerialIO_puts("\r\n");
}

void NRF24_printFifoStatus() {
  uint8_t status = NRF24_getRegister(NRF24_REG_FIFO_STATUS);
  SerialIO_puts("FIFO: ");
  SerialIO_putbin(status);
  SerialIO_puts(" ");
  if (status & NRF24_TX_REUSE) SerialIO_puts("TX_REUSE ");
  if (status & NRF24_TX_FULL) SerialIO_puts("TX_FULL ");
  if (status & NRF24_TX_EMPTY) {
    SerialIO_puts("TX_EMPTY ");
  } else {
    SerialIO_puts("TX_HAS_DATA ");
  }
  if (status & NRF24_RX_FULL) SerialIO_puts("RX_FULL ");
  if (status & NRF24_RX_EMPTY) {
    SerialIO_puts("RX_EMPTY ");
  } else {
    SerialIO_puts("RX_HAS_DATA ");
  }
  SerialIO_puts("\r\n");
}

void NRF24_printObserveTx() {
  uint8_t status = NRF24_getRegister(NRF24_REG_OBSERVE_TX);
  SerialIO_puts("ObserveTx: ");
  SerialIO_putbin(status);
  SerialIO_puts(" PLOS_CNT: ");
  SerialIO_putb((status & NRF24_PLOS_CNT) >> 4);
  SerialIO_puts(" ARC_CNT: ");
  SerialIO_putb(status & NRF24_ARC_CNT);
  SerialIO_puts("\r\n");
}

void NRF24_printRPD() {
  NRF24_printRegister(NRF24_REG_RPD, "RPD");
}

void NRF24_printRegister(uint8_t addr, const char name[]) {
  uint8_t reg = NRF24_getRegister(addr);
  SerialIO_puts(name);
  SerialIO_puts(": ");
  SerialIO_putb(reg);
  SerialIO_puts(" ");
  SerialIO_putbin(reg);
  SerialIO_puts("\r\n");
}

void NRF24_getRx(uint8_t *buf, uint8_t len) {
  return NRF24_read(NRF24_CMD_R_RX_PAYLOAD, buf, len);
}

void NRF24_setTx(const uint8_t* data, uint8_t len) {
  NRF24_write(NRF24_CMD_W_TX_PAYLOAD, data, len);
}

uint8_t NRF24_getRegister(uint8_t addr) {
  return NRF24_read(NRF24_CMD_R_REGISTER | addr);
}

void NRF24_getRegister(uint8_t addr, uint8_t *buf, uint8_t len) {
  return NRF24_read(NRF24_CMD_R_REGISTER | addr, buf, len);
}

void NRF24_setRegister(uint8_t addr, uint8_t val) {
  NRF24_write(NRF24_CMD_W_REGISTER | addr, val);
}

void NRF24_setRegisterAnd(uint8_t addr, uint8_t val) {
  uint8_t existing = NRF24_read(NRF24_CMD_R_REGISTER | addr);
  NRF24_write(NRF24_CMD_W_REGISTER | addr, existing & val);
}

void NRF24_setRegisterOr(uint8_t addr, uint8_t val) {
  uint8_t existing = NRF24_read(NRF24_CMD_R_REGISTER | addr);
  NRF24_write(NRF24_CMD_W_REGISTER | addr, existing | val);
}

void NRF24_setRegisterAndOr(uint8_t addr, uint8_t andVal, uint8_t orVal) {
  uint8_t existing = NRF24_read(NRF24_CMD_R_REGISTER | addr);
  NRF24_write(NRF24_CMD_W_REGISTER | addr, (existing & andVal) | orVal);
}

void NRF24_setRegister(uint8_t addr, const uint8_t* val, uint8_t len) {
  NRF24_write(NRF24_CMD_W_REGISTER | addr, val, len);
}

uint8_t NRF24_read(uint8_t cmd) {
  uint8_t buf[1];
  NRF24_read(cmd, buf, 1);
  return buf[0];
}

void NRF24_read(uint8_t cmd, uint8_t *buf, uint8_t len) {
  uint8_t i;
  NRF24_CSN_Low();
  SPI_transfer(cmd);
  for (i = 0; i < len; i++) {
    *buf++ = SPI_transfer(0);
  }
  NRF24_CSN_High();
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

void NRF24_write(uint8_t cmd, const uint8_t* val, uint8_t len) {
  uint8_t i;
  NRF24_CSN_Low();
  SPI_transfer(cmd);
  for (i = 0; i < len; i++) {
    SPI_transfer(val[i]);
  }
  NRF24_CSN_High();
}

