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
  SPI_begin();
  NRF24_init();

  // Let components (esp. radio) wake up
  _delay_ms(100);

  RED_LED_ToOutput();
  GRN_LED_ToOutput();
  YLW_LED_ToOutput();

  while (1) {
    mainTest(SerialIO_recv());
  }

  return 0;
}


void testRadio(uint8_t input) {
  if (input != 'c' && input != 's') {
    SerialIO_puts("\r\n -- Skipping radio test -- \r\n");
    return;
  }

  // Shared setup
  NRF24_init();
  NRF24_configure();

  NRF24_printStatus();
  NRF24_printConfig();
  NRF24_printAddresses();

  uint8_t payload[1] = { 1 };

  if (input == 'c') {
    SerialIO_puts("\r\n -- Radio test (client) -- \r\n");

    // Client setup
    NRF24_setRxAddr((uint8_t *)"clie1", 5);
    NRF24_setTxAddr((uint8_t *)"serv1", 5);

    // Client loop
    while (!SerialIO_hasData()) {
      SerialIO_puts("TX ");
      SerialIO_putb(++payload[0]);
      SerialIO_puts(":");

      if (NRF24_send(payload, 1)) {
        SerialIO_puts(" sent. RX: ");
      } else {
        SerialIO_puts(" failed. RX: ");
      }

      _delay_ms(10);

      int i = 0;
      while(!NRF24_dataAvailable()){
        if (i++ > 1000) {
          SerialIO_puts("TIMEOUT\r\n");
          break;
        }
        _delay_ms(1);
      }

      if (i < 1000) {
        uint8_t data[1];
        NRF24_read(NRF24_CMD_R_RX_PAYLOAD, data, 1);
        SerialIO_putb(data[0]);
        SerialIO_puts("\r\n");
        NRF24_write(NRF24_CMD_FLUSH_RX);
      }

      _delay_ms(1000);
    }
  } else if (input == 's') {
    SerialIO_puts("\r\n -- Radio test (server) -- \r\n");

    // Server setup
    NRF24_setRxAddr((uint8_t *)"serv1", 5);
    NRF24_rxMode();
    NRF24_setTxAddr((uint8_t *)"clie1", 5);

    // Server loop
    while (!SerialIO_hasData()) {
      int i = 0;
      while(1) {
        if (NRF24_dataAvailable()) {
          SerialIO_puts("RX: ");
          uint8_t data[1];
          NRF24_read(NRF24_CMD_R_RX_PAYLOAD, data, 1);
          SerialIO_putb(data[0]);
          SerialIO_puts(" TX: ");
          SerialIO_putb(data[0]);

          if (NRF24_send(data, 1)) {
            SerialIO_puts(" sent!\r\n");
          } else {
            SerialIO_puts(" failed!\r\n");
          }
        } else {
          if (i++ > 4000) {
            SerialIO_puts("Timeout waiting for client!\r\n");
            break;
          }
          _delay_ms(1);
        }
      }
    }
  }

  // Consume the char that was used to break the loop
  SerialIO_recv();
}

void testEEPROM(uint8_t input) {
  SerialIO_puts("\r\n -- EEPROM TEST -- \r\n");

  int addr = 0;
  uint8_t data[3] = { input, static_cast<uint8_t>(input + 1), static_cast<uint8_t>(input + 2) };

  SerialIO_puts("Writing 3 bytes: ");
  SerialIO_putb(data[0]);
  SerialIO_puts(" - ");
  SerialIO_putb(data[1]);
  SerialIO_puts(" - ");
  SerialIO_putb(data[2]);
  SerialIO_puts("\r\n");

  EEPROM_write(addr, data, 3);

  SerialIO_puts("Reading 3 bytes: ");
  EEPROM_read(addr, data, 3);
  SerialIO_putb(data[0]);
  SerialIO_puts(" - ");
  SerialIO_putb(data[1]);
  SerialIO_puts(" - ");
  SerialIO_putb(data[2]);
  SerialIO_puts("\r\n");
}

void testIRLED(uint8_t input) {
  SerialIO_puts("\r\n -- IR-LED TEST -- \r\n");
  IRSend_send(0x8166817E, 32);
  SerialIO_puts("AC should have toggled power\r\n");
}

void testDHT11(uint8_t input) {
  SerialIO_puts("\r\n -- DHT-11 TEST -- \r\n");

  uint8_t r = DHT11_readSensor();
  SerialIO_puts("Read result: ");
  SerialIO_putb(r);
  SerialIO_puts(" Temp: ");
  SerialIO_putb(DHT11_temperature());
  SerialIO_puts("C Humidity: ");
  SerialIO_putb(DHT11_humidity());
  SerialIO_puts("%\r\n");
}

void testDIAGLEDS(uint8_t input) {
  SerialIO_puts("\r\n -- DIAG LED TEST -- \r\n");

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
  testRadio(input);
  testEEPROM(input);
  testIRLED(input);
  testDHT11(input);
  testDIAGLEDS(input);
}
