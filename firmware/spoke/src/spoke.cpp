#include <util/delay.h>

#include <pins.h>
#include <SPI.h>
#include <Serial.h>
#include <EEPROM.h>
#include <DHT11.h>
#include <IRSend.h>
#include <NRF24L01.h>

#include <spoke.h>

int main() {
  Serial.begin();
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
    mainTest(Serial.read());
  }

  return 0;
}


void testRadio(uint8_t input) {
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

  uint8_t payload[1] = { 1 };

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

void testEEPROM(uint8_t input) {
  Serial.print("\r\n -- EEPROM TEST -- \r\n");

  int addr = 0;
  uint8_t data[3] = { input, static_cast<uint8_t>(input + 1), static_cast<uint8_t>(input + 2) };

  Serial.print("Writing 3 bytes: ");
  Serial.putb(data[0]);
  Serial.print(" - ");
  Serial.putb(data[1]);
  Serial.print(" - ");
  Serial.putb(data[2]);
  Serial.print("\r\n");

  EEPROM_write(addr, data, 3);

  Serial.print("Reading 3 bytes: ");
  EEPROM_read(addr, data, 3);
  Serial.putb(data[0]);
  Serial.print(" - ");
  Serial.putb(data[1]);
  Serial.print(" - ");
  Serial.putb(data[2]);
  Serial.print("\r\n");
}

void testIRLED(uint8_t input) {
  Serial.print("\r\n -- IR-LED TEST -- \r\n");
  IRSend_send(0x8166817E, 32);
  Serial.print("AC should have toggled power\r\n");
}

void testDHT11(uint8_t input) {
  Serial.print("\r\n -- DHT-11 TEST -- \r\n");

  uint8_t r = DHT11_readSensor();
  Serial.print("Read result: ");
  Serial.putb(r);
  Serial.print(" Temp: ");
  Serial.putb(DHT11_temperature());
  Serial.print("C Humidity: ");
  Serial.putb(DHT11_humidity());
  Serial.print("%\r\n");
}

void testDIAGLEDS(uint8_t input) {
  Serial.print("\r\n -- DIAG LED TEST -- \r\n");

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
