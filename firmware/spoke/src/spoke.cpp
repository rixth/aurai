#include <util/delay.h>

#include <pins.h>
#include <SPI.h>
#include <DiagLEDS.h>
#include <Serial.h>
#include <DHT11.h>
#include <IRSend.h>
#include <NRF24L01.h>
#include <EEPROM.h>
#include <SpokeCommands.h>
#include <AirConditioner.h>

#include <spoke.h>

AirConditioner ac;

int main() {
  // IRSend_init();
  Serial.begin();
  SPI.begin();
  DiagLEDS_init();
  NRF24_init();

  while (1) {
    Serial.println("Aurai SPOKE. Press b to boot, any other key to test.");
    uint8_t input = Serial.read();
    if (input == 'b') {
      Serial.println("Booting...");
      boot();
    } else {
      Serial.println("Testing...");
      mainTest(input);
    }
  }

  return 0;
}

void boot() {
  initializeRadio();
  while (!Serial.available()) {
    if (NRF24_dataAvailable()) {
      processIncomingData();
    }
    // TODO sleep here
  }
  Serial.read();
}

void initializeRadio() {
  NRF24_flush();
  NRF24_configure();
  NRF24_setRxAddr((uint8_t *)"spoke", 5);
  NRF24_setTxAddr((uint8_t *)"hub__", 5);
  NRF24_rxMode();
}

void processIncomingData() {
  uint8_t data[2];
  uint8_t len = NRF24_fetch(data, 2);

  Serial.print("Received: ");
  uint8_t i;
  for (i = 0; i < len; i++) {
    Serial.puth(data[i]);
    if (i != (len - 1)) {
      Serial.print(" ");
    }
  }
  Serial.print("\r\n");

  // The first 5 bits are the command so shift the rest off
  uint8_t cmd = data[0] >> 3 << 3;

  bool success = true;
  bool sendReply = true;

  if (cmd == SPOKE_CMD_POWER) {
    if (data[0] == SPOKE_CMD_POWER_PRM_TOGGLE) {
      ac.togglePower();
    } else if (data[0] == SPOKE_CMD_POWER_PRM_OFF) {
      success = ac.powerOff();
    } else if (data[0] == SPOKE_CMD_POWER_PRM_ON) {
      success = ac.powerOn();
    }
  } else if (cmd == SPOKE_CMD_STATUS) {
    DHT11_readSensor();
    uint16_t acStatus = ac.status();
    uint8_t reply[5] = { \
      SPOKE_RESP_STATUS,
      DHT11_humidity(),
      (uint8_t) DHT11_temperature(),
      (uint8_t) (acStatus >> 8),
      (uint8_t)  (acStatus & 0xF)
    };
    NRF24_send(reply, 5);
  }

  if (sendReply) {
    uint8_t reply[1];
    if (success) {
      reply[0] = SPOKE_RESP_OK;
    } else {
      reply[0] = SPOKE_RESP_FAIL;
    }
    NRF24_send(reply, 1);
  }

  if (success) {
    DiagLEDS_set(LED_GRN);
    Serial.println("Command succeeded.");
  } else {
    DiagLEDS_set(LED_RED);
    Serial.println("Command failed?");
  }
}

void mainTest(uint8_t input) {
  NRF24_test(input);
  IRSend_test();
  DHT11_test();
  EEPROM.test(input);
  DiagLEDS_test(input);
}
