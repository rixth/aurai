#include <util/delay.h>

#include <pins.h>
#include <SPI.h>
#include <SerialIO.h>
#include <EEPROM.h>
#include <DHT11.h>
#include <IRSend.h>
#include <NRF24L01.h>

#include <spoke.h>

int main() {
  SerialIO_Init();
  EEPROM_init();
  IRSend_init();
  NRF24_init();
  SPI_begin();

  // Let components (esp. radio) wake up
  _delay_ms(100);

  RED_LED_ToOutput();
  GRN_LED_ToOutput();
  YLW_LED_ToOutput();
  TRG_ToOutput();
  TRG_Low();

  while (1) {
    // mainTest(SerialIO_recv());
    testRadio(SerialIO_recv());
  }

  return 0;
}

void testRadio(uint8_t input) {
  uint8_t mode;

  if (input == 'r') {
    mode = 'r';
    SerialIO_puts("\n -- RADIO RECEIVER TEST -- \n");
  } else if (input == 't') {
    mode = 't';
    SerialIO_puts("\n -- RADIO TRANSMITTER TEST -- \n");
  } else if (input == 'c' || input == 'v' || input == 'b') {
    mode = 'c';
    SerialIO_puts("\n -- RADIO CONFIG TEST -- \n");
  } else {
    SerialIO_puts("\n -- Skipping Radio Test -- \n");
    return;
  }

  SPI_setBitOrder(SPI_MSBFIRST);

  // Configure: CRC scheme (field cleared, 1-bit)
  // Clear interrupt flags
  // Power up
  // Wait for standby mode
  SerialIO_puts("Powering up...\n");
  NRF24_setRegister(NRF24_REG_CONFIG, NRF24_PWR_UP | NRF24_INTERRUPTS);
  _delay_us(NRF24_TIME_PD_TO_SB1);

  SerialIO_puts("Configuring...\n");

  // Configure: Frequency channel
  NRF24_setRegister(NRF24_REG_RF_CH, 10);

  // Configure: RX address width
  NRF24_setRegister(NRF24_REG_SETUP_AW, NRF24_AW_5BITS);

  // Disable RX1
  // NRF24_setRegisterAnd(NRF24_REG_EN_RXADDR, ~(NRF24_ERX_P1));

  // Configure: Air data rate
  // Configure: LNA gain
  NRF24_setRegister(NRF24_REG_RF_SETUP, NRF24_RF_DR_2MBPS | NRF24_RF_PWR_12DBM);

  // uint8_t rxAddr[5] = { 0x05, 0x04, 0x03, 0x02, 0x01 };
  // uint8_t txAddr[5] = { 0x06, 0x05, 0x04, 0x03, 0x02 };

  // uint8_t rxAddr[5] = { 'c', 'l', 'i', 'e', '1' };
  // uint8_t txAddr[5] = { 's', 'e', 'r', 'v', '1' };

  // uint8_t *rxAddr = (uint8_t *)"serv1";
  // uint8_t *txAddr = (uint8_t *)"clie1";

  TRG_High();
  // Configure: TX and RX addresses
  if (mode == 'r' || input == 'v') {
    NRF24_setRegister(NRF24_REG_RX_ADDR_P1, (uint8_t *)"serv1", 5);
    NRF24_setRegister(NRF24_REG_TX_ADDR, (uint8_t *)"clie1", 5);
    NRF24_setRegister(NRF24_REG_RX_ADDR_P0, (uint8_t *)"clie1", 5);

    // NRF24_setRegister(NRF24_REG_RX_ADDR_P5, rxAddr, 5);

    // NRF24_setRegister(NRF24_REG_RX_ADDR_P1, txAddr, 5);
    // NRF24_setRegister(NRF24_REG_RX_ADDR_P0, rxAddr, 5);
  } else if (mode == 't' || input == 'b') {
    NRF24_setRegister(NRF24_REG_RX_ADDR_P1, (uint8_t *)"clie1", 5);
    NRF24_setRegister(NRF24_REG_TX_ADDR, (uint8_t *)"serv1", 5);
    NRF24_setRegister(NRF24_REG_RX_ADDR_P0, (uint8_t *)"serv1", 5);

    // NRF24_setRegister(NRF24_REG_TX_ADDR, rxAddr, 5);
    // NRF24_setRegister(NRF24_REG_RX_ADDR_P0, rxAddr, 5);

    // NRF24_setRegister(NRF24_REG_TX_ADDR, txAddr, 5);
    // NRF24_setRegister(NRF24_REG_RX_ADDR_P0, txAddr, 5);

    // NRF24_setRegister(NRF24_REG_RX_ADDR_P0, txAddr, 5);
    // NRF24_setRegister(NRF24_REG_TX_ADDR, txAddr, 5);
    // NRF24_setRegister(NRF24_REG_RX_ADDR_P1, rxAddr, 5);
  }

  // Configure: packet size to 1 byte
  NRF24_setRegister(NRF24_REG_RX_PW_P0, 1);
  NRF24_setRegister(NRF24_REG_RX_PW_P1, 1);

  // Set lots of retries
  NRF24_setRegister(NRF24_REG_SETUP_RETR, 0xFF);

  // Enable no-ACK feature
  // NRF24_setRegisterOr(NRF24_REG_FEATURE, NRF24_EN_DYN_ACK);

  // Flush RX & TX
  // NRF24_write(NRF24_CMD_FLUSH_RX);
  NRF24_write(NRF24_CMD_FLUSH_TX);

  NRF24_printStatus();
  NRF24_printConfig();
  NRF24_printAddresses();

  if (mode == 'r') {
    // Receiver loop
    SerialIO_puts("Entering receiver loop\n");

    // Set PRIM_RX
    NRF24_setRegisterOr(NRF24_REG_CONFIG, NRF24_PRIM_RX);

    SerialIO_puts("PRIM_RX: ");
    SerialIO_putb(NRF24_getRegister(NRF24_REG_CONFIG) & NRF24_PRIM_RX);
    SerialIO_puts("\n");

    NRF24_CE_High();

    _delay_us(NRF24_TIME_SB_TO_RXTX);

    while (!SerialIO_hasData()) {
      NRF24_printRPD();
      NRF24_printFifoStatus();
      // Check for data
      if (NRF24_write(NRF24_CMD_NOP) & NRF24_RX_DR) {
        uint8_t data[1];
        NRF24_read(NRF24_CMD_R_RX_PAYLOAD, data, 1);
        SerialIO_puts("Got data: ");
        SerialIO_putb(data[0]);
        // SerialIO_puts(" - ");
        // SerialIO_putb(data[1]);
        // SerialIO_puts(" - ");
        // SerialIO_putb(data[2]);
        // SerialIO_puts(" - ");
        // SerialIO_putb(data[3]);
        // SerialIO_puts(" - ");
        // SerialIO_putb(data[4]);
        SerialIO_puts("\n");
      }
      _delay_ms(300);
    }

    NRF24_CE_Low();

    // Clear PRIM_RX
    NRF24_setRegisterAnd(NRF24_REG_CONFIG, ~(NRF24_PRIM_RX));

    SerialIO_recv();
  } else if (mode == 't') {
    // Transmitter loop
    SerialIO_puts("Entering transmitter loop\n");
    while (!SerialIO_hasData()) {
      uint8_t payload[1] = { 123 };

      NRF24_printFifoStatus();

      // Set payload
      NRF24_write(NRF24_CMD_W_TX_PAYLOAD, payload, 1);
      // NRF24_write(NRF24_CMD_W_TX_PAYLOAD_NO_ACK, payload, 1);

      NRF24_printFifoStatus();

      // Pulse CE
      NRF24_CE_High();
      _delay_us(13);
      NRF24_CE_Low();

      NRF24_printFifoStatus();
      NRF24_printFifoStatus();
      NRF24_printFifoStatus();
      NRF24_printFifoStatus();
      NRF24_printFifoStatus();
      NRF24_printFifoStatus();
      NRF24_printFifoStatus();
      _delay_ms(500);

      NRF24_write(NRF24_CMD_FLUSH_TX);
      NRF24_setRegisterAnd(NRF24_REG_STATUS, ~(NRF24_MAX_RT));
      NRF24_setRegisterOr(NRF24_REG_CONFIG, NRF24_INTERRUPTS);
    }
    SerialIO_recv();
  } else if (mode == 'c') {
    NRF24_printFifoStatus();
    NRF24_printObserveTx();
  }

  SerialIO_puts("Powering down...\n");
  NRF24_setRegister(NRF24_REG_CONFIG, 0);
}

void testEEPROM(uint8_t input) {
  SerialIO_puts("\n -- EEPROM TEST -- \n");

  int addr = 0;
  uint8_t data[3] = { input, static_cast<uint8_t>(input + 1), static_cast<uint8_t>(input + 2) };

  SerialIO_puts("Writing 3 bytes: ");
  SerialIO_putb(data[0]);
  SerialIO_puts(" - ");
  SerialIO_putb(data[1]);
  SerialIO_puts(" - ");
  SerialIO_putb(data[2]);
  SerialIO_puts("\n");

  EEPROM_write(addr, data, 3);

  SerialIO_puts("Reading 3 bytes: ");
  EEPROM_read(addr, data, 3);
  SerialIO_putb(data[0]);
  SerialIO_puts(" - ");
  SerialIO_putb(data[1]);
  SerialIO_puts(" - ");
  SerialIO_putb(data[2]);
  SerialIO_puts("\n");
}

void testIRLED(uint8_t input) {
  SerialIO_puts("\n -- IR-LED TEST -- \n");
  IRSend_send(0x8166817E, 32);
  SerialIO_puts("AC should have toggled power\n");
}

void testDHT11(uint8_t input) {
  SerialIO_puts("\n -- DHT-11 TEST -- \n");

  uint8_t r = DHT11_readSensor();
  SerialIO_puts("Read result: ");
  SerialIO_putb(r);
  SerialIO_puts(" Temp: ");
  SerialIO_putb(DHT11_temperature());
  SerialIO_puts("C Humidity: ");
  SerialIO_putb(DHT11_humidity());
  SerialIO_puts("%\n");
}

void testDIAGLEDS(uint8_t input) {
  SerialIO_puts("\n -- DIAG LED TEST -- \n");

  if (input == '1') {
    RED_LED_High();
    _delay_ms(100);
    RED_LED_Low();
  } else if (input == '2') {
    YLW_LED_High();
    _delay_ms(100);
    YLW_LED_Low();
  } else if (input == '3') {
    GRN_LED_High();
    _delay_ms(100);
    GRN_LED_Low();
  }
}

void mainTest(uint8_t input) {
  testEEPROM(input);
  testIRLED(input);
  testDHT11(input);
  testDIAGLEDS(input);
}
