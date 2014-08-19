#include "SPI.h"
#include "Serial.h"
#include "NRF24L01.h"
#include "DiagLEDS.h"
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
  NRF24_setRegister(NRF24_REG_CONFIG, (NRF24_PWR_UP | NRF24_EN_CRC) & ~NRF24_CRCO);
  NRF24_setRegister(NRF24_REG_DYNPD, NRF24_DPL_P0 | NRF24_DPL_P1);
  NRF24_setRegister(NRF24_REG_FEATURE, NRF24_EN_DPL);
  NRF24_setRegister(NRF24_REG_SETUP_RETR, 0xFF);
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

int8_t NRF24_fetch(uint8_t *buf, uint8_t maxLen) {
  uint8_t len = NRF24_read(NRF24_CMD_R_RX_PL_WID);

  // Datasheet says to flush rx if len > 32 b/c of corruption
  if (len > 32) {
    NRF24_write(NRF24_CMD_FLUSH_RX);
    return -1;
  } else if (len > maxLen) {
    return -1;
  }

  NRF24_read(NRF24_CMD_R_RX_PAYLOAD, buf, len);
  // Clear RX_DR bit by writing to it
  NRF24_setRegister(NRF24_REG_STATUS, NRF24_RX_DR);

  return len;
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

void NRF24_flush() {
  NRF24_write(NRF24_CMD_FLUSH_TX);
  NRF24_write(NRF24_CMD_FLUSH_RX);
}

void NRF24_test(uint8_t input) {
  if (input != 'c' && input != 's') {
    Serial.print("\r\n -- Skipping radio test -- \r\n");
    return;
  }

  // Shared setup
  NRF24_init();
  NRF24_configure();

  NRF24_printStatus();
  NRF24_printConfig();
  NRF24_printAddresses();

  uint8_t payload[1] = { 0 };

  if (input == 'c') {
    Serial.print("\r\n -- Radio test (client) -- \r\n");

    // Client setup
    NRF24_setRxAddr((uint8_t *)"clie1", 5);
    NRF24_setTxAddr((uint8_t *)"serv1", 5);

    // Client loop
    while (!Serial.available()) {
      Serial.print("TX ");
      Serial.putb(++payload[0]);
      Serial.print(":");

      DiagLEDS_set(payload[0]);

      if (payload[0] == 7) {
        payload[0] = 0;
      }

      if (NRF24_send(payload, 1)) {
        Serial.print(" sent. RX: ");
      } else {
        Serial.print(" failed. RX: ");
      }

      _delay_ms(10);

      int i = 0;
      while(!NRF24_dataAvailable()){
        if (i++ > 1000) {
          Serial.print("TIMEOUT\r\n");
          break;
        }
        _delay_ms(1);
      }

      if (i < 1000) {
        uint8_t data[1];
        NRF24_read(NRF24_CMD_R_RX_PAYLOAD, data, 1);
        Serial.putb(data[0]);
        Serial.print("\r\n");
        NRF24_write(NRF24_CMD_FLUSH_RX);
      }

      _delay_ms(1000);
    }
  } else if (input == 's') {
    Serial.print("\r\n -- Radio test (server) -- \r\n");

    // Server setup
    NRF24_setRxAddr((uint8_t *)"serv1", 5);
    NRF24_rxMode();
    NRF24_setTxAddr((uint8_t *)"clie1", 5);

    // Server loop
    while (!Serial.available()) {
      int i = 0;
      while(1) {
        if (NRF24_dataAvailable()) {
          Serial.print("RX: ");
          uint8_t data[1];
          NRF24_read(NRF24_CMD_R_RX_PAYLOAD, data, 1);
          Serial.putb(data[0]);
          Serial.print(" TX: ");
          Serial.putb(data[0]);

          DiagLEDS_set(data[0]);

          if (NRF24_send(data, 1)) {
            Serial.print(" sent!\r\n");
          } else {
            Serial.print(" failed!\r\n");
          }
        } else {
          if (i++ > 4000) {
            Serial.print("Timeout waiting for client!\r\n");
            break;
          }
          _delay_ms(1);
        }
      }
    }
  }

  // Consume the char that was used to break the loop
  Serial.read();
}

void NRF24_printConfig() {
  uint8_t config = NRF24_getRegister(NRF24_REG_CONFIG);
  uint8_t rfCh = NRF24_getRegister(NRF24_REG_RF_CH);
  uint8_t rfSetup = NRF24_getRegister(NRF24_REG_RF_SETUP);
  uint8_t aw = NRF24_getRegister(NRF24_REG_SETUP_AW);

  Serial.print("Config: ");
  Serial.putbin(config);
  Serial.print(" ");
  if (!(config & NRF24_MASK_RX_DR)) Serial.print("MASK_RX_DR ");
  if (!(config & NRF24_MASK_TX_DS)) Serial.print("MASK_TX_DS ");
  if (!(config & NRF24_MASK_MAX_RT)) Serial.print("MASK_MAX_RT ");
  if (config & NRF24_EN_CRC) Serial.print("EN_CRC ");
  if (config & NRF24_CRCO) {
    Serial.print("CRC-2 ");
  } else {
    Serial.print("CRC-1 ");
  }
  if (config & NRF24_PWR_UP) Serial.print("PWR_UP ");
  if (config & NRF24_PRIM_RX) Serial.print("PRIM_RX ");
  Serial.print("\r\n");

  Serial.print("RF setup: ");
  Serial.putbin(rfSetup);
  Serial.print(" CH: ");
  Serial.putb(rfCh);
  Serial.print(" ");
  if (rfSetup & NRF24_CONT_WAVE) Serial.print("CONT_WAVE ");
  if (rfSetup & NRF24_PLL_LOCK) Serial.print("PLL_LOCK ");
  if (rfSetup & NRF24_RF_DR_250KB) {
    Serial.print("250kbps ");
  } else if (rfSetup & NRF24_RF_DR_2MBPS) {
    Serial.print("2mbps ");
  } else {
    Serial.print("1mbps ");
  }
  if ((rfSetup & NRF24_RF_PWR0) && (rfSetup & NRF24_RF_PWR1)) {
    Serial.print("0dbm ");
  } else if (!(rfSetup & NRF24_RF_PWR0) && (rfSetup & NRF24_RF_PWR1)) {
    Serial.print("-6dbm ");
  } else if ((rfSetup & NRF24_RF_PWR0) && !(rfSetup & NRF24_RF_PWR1)) {
    Serial.print("-12dbm ");
  } else if (!(rfSetup & NRF24_RF_PWR0) && !(rfSetup & NRF24_RF_PWR1)) {
    Serial.print("-18dbm ");
  } else {
    Serial.print("-XXdbm ");
  }
  Serial.print("\r\n");

  Serial.print("AW setup: ");
  Serial.putbin(aw);
  if (aw & NRF24_AW_5BITS) {
    Serial.print(" 5 bytes\r\n");
  } else if (aw & NRF24_AW_4BITS) {
    Serial.print(" 4 bytes\r\n");
  } else if (aw & NRF24_AW_3BITS) {
    Serial.print(" 3 bytes\r\n");
  } else {
    Serial.print(" X bytes\r\n");
  }
}

void NRF24_printAddresses() {
  uint8_t dynPld = NRF24_getRegister(NRF24_REG_DYNPD);
  uint8_t enRx = NRF24_getRegister(NRF24_REG_EN_RXADDR);
  uint8_t enAa = NRF24_getRegister(NRF24_REG_EN_AA);
  uint8_t i, j;
  uint8_t addr[5];

  Serial.print("TX  Address: ");
  NRF24_getRegister(NRF24_REG_TX_ADDR, addr, 5);
  for(i = 0; i < 5; i++) {
    Serial.puth(addr[i]);
    Serial.print(" ");
  }
  Serial.print("\r\n");

  for (i = 0; i < 6; i++) {
    if (enRx & _BV(i)) {
      Serial.print("RX");
      Serial.putb(i);
      Serial.print(" Address: ");

      if (i == 0 || i == 1) {
        NRF24_getRegister(NRF24_REG_RX_ADDR_P0 + i, addr, 5);
        for(j = 0; j < 5; j++) {
          Serial.puth(addr[j]);
          Serial.print(" ");
        }
      } else {
        NRF24_getRegister(NRF24_REG_RX_ADDR_P0 + i, addr, 1);
        Serial.puth(addr[0]);
        Serial.print(" ");
      }

      Serial.print("Payload: ");
      if (dynPld & _BV(i)) {
        Serial.print("dynamic.");
      } else {
        Serial.putb(NRF24_getRegister(NRF24_REG_RX_PW_P0 + i));
        Serial.print(" bytes.");
      }

      Serial.print((enAa & _BV(i)) ? " AA on. \r\n" : " AA off. \r\n");
    }
  }
}

void NRF24_printStatus() {
  uint8_t status = NRF24_write(NRF24_CMD_NOP);
  Serial.print("Status: ");
  Serial.putbin(status);
  Serial.print(" ");
  if (status & NRF24_ST_TX_FULL) Serial.print("ST_TX_FULL ");
  if (status & NRF24_MAX_RT) Serial.print("MAX_RT ");
  if (status & NRF24_TX_DS) Serial.print("TX_DS ");
  if (status & NRF24_RX_DR) Serial.print("RX_DR ");
  Serial.print(" RX_P_NO: ");
  Serial.putb((status & NRF24_RX_P_NO) > 1);
  Serial.print("\r\n");
}

void NRF24_printFifoStatus() {
  uint8_t status = NRF24_getRegister(NRF24_REG_FIFO_STATUS);
  Serial.print("FIFO: ");
  Serial.putbin(status);
  Serial.print(" ");
  if (status & NRF24_TX_REUSE) Serial.print("TX_REUSE ");
  if (status & NRF24_TX_FULL) Serial.print("TX_FULL ");
  if (status & NRF24_TX_EMPTY) {
    Serial.print("TX_EMPTY ");
  } else {
    Serial.print("TX_HAS_DATA ");
  }
  if (status & NRF24_RX_FULL) Serial.print("RX_FULL ");
  if (status & NRF24_RX_EMPTY) {
    Serial.print("RX_EMPTY ");
  } else {
    Serial.print("RX_HAS_DATA ");
  }
  Serial.print("\r\n");
}

void NRF24_printObserveTx() {
  uint8_t status = NRF24_getRegister(NRF24_REG_OBSERVE_TX);
  Serial.print("ObserveTx: ");
  Serial.putbin(status);
  Serial.print(" PLOS_CNT: ");
  Serial.putb((status & NRF24_PLOS_CNT) >> 4);
  Serial.print(" ARC_CNT: ");
  Serial.putb(status & NRF24_ARC_CNT);
  Serial.print("\r\n");
}

void NRF24_printRPD() {
  NRF24_printRegister(NRF24_REG_RPD, "RPD");
}

void NRF24_printRegister(uint8_t addr, const char name[]) {
  uint8_t reg = NRF24_getRegister(addr);
  Serial.print(name);
  Serial.print(": ");
  Serial.putb(reg);
  Serial.print(" ");
  Serial.putbin(reg);
  Serial.print("\r\n");
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
  SPI.transfer(cmd);
  for (i = 0; i < len; i++) {
    *buf++ = SPI.transfer(0);
  }
  NRF24_CSN_High();
}

uint8_t NRF24_write(uint8_t cmd) {
  NRF24_CSN_Low();
  uint8_t result = SPI.transfer(cmd);
  NRF24_CSN_High();
  return result;
}

void NRF24_write(uint8_t cmd, uint8_t val) {
  NRF24_CSN_Low();
  SPI.transfer(cmd);
  SPI.transfer(val);
  NRF24_CSN_High();
}

void NRF24_write(uint8_t cmd, const uint8_t* val, uint8_t len) {
  uint8_t i;
  NRF24_CSN_Low();
  SPI.transfer(cmd);
  for (i = 0; i < len; i++) {
    SPI.transfer(val[i]);
  }
  NRF24_CSN_High();
}

