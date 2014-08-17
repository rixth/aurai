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
      // mainTest(input);
    }
  }

  return 0;
}

void boot() {
  // Configure radio
  NRF24_flush();
  NRF24_configure();
  NRF24_setRxAddr((uint8_t *)"spoke", 5);
  NRF24_setTxAddr((uint8_t *)"hub__", 5);
  NRF24_rxMode();

  while (!Serial.available()) {
    if (NRF24_dataAvailable()) {
      processIncomingData();
    }
    // TODO sleep here
  }
  Serial.read();
}

void processIncomingData() {
  uint8_t data[1];
  NRF24_fetch(data, 1);

  Serial.print("Received: ");
  Serial.puth(data[0]);
  Serial.print("\r\n");

  // The first 5 bits are the command so shift the rest off
  uint8_t cmd = data[0] >> 3 << 3;

  bool success;

  if (cmd == SPOKE_CMD_POWER) {
    if (data[0] == SPOKE_CMD_POWER_PRM_TOGGLE) {
      ac.togglePower();
      success = true;
    } else if (data[0] == SPOKE_CMD_POWER_PRM_OFF) {
      success = ac.powerOff();
    } else if (data[0] == SPOKE_CMD_POWER_PRM_ON) {
      success = ac.powerOn();
    }
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
