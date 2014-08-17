#include <util/delay.h>

#include <pins.h>
#include <SPI.h>
#include <DiagLEDS.h>
#include <Serial.h>
#include <Flash.h>
#include <EEPROM.h>
#include <NRF24L01.h>
#include <SpokeCommands.h>

#include <hub.h>

int main() {
  Flash_init();
  Serial.begin();
  SPI.begin();
  DiagLEDS_init();
  NRF24_init();

  while (1) {
    Serial.println("Aurai HUB. Press b to boot, any other key to test.");
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
  // Configure radio
  NRF24_flush();
  NRF24_configure();
  NRF24_setTxAddr((uint8_t *)"spoke", 5);
  NRF24_setRxAddr((uint8_t *)"hub__", 5);

  // Transmit a packet
  Serial.print("Sending power toggle: ");
  uint8_t payload[1] = { SPOKE_CMD_POWER_PRM_TOGGLE };
  if (NRF24_send(payload, 1)) {
    Serial.println("sent.");
  } else {
    Serial.println("failed.");
  }

  // Transmit another
  Serial.print("Sending status request packet: ");
  payload[0] = SPOKE_CMD_STATUS;
  if (NRF24_send(payload, 1)) {
    Serial.println("sent.");
  } else {
    Serial.println("failed.");
  }

  Serial.print("Waiting for report... ");

  NRF24_rxMode();

  int i = 0;
  while(!NRF24_dataAvailable()){
    if (i++ > 1000) {
      Serial.println(" timeout");
      return;
    }
    _delay_ms(1);
  }

  uint8_t data[1];
  NRF24_fetch(data, 1);

  Serial.print("Humidity: ");
  Serial.putb(data[0]);
  Serial.println("%");
}

void mainTest(uint8_t input) {
  NRF24_test(input);
  Flash_test(input);
  DiagLEDS_test(input);
  EEPROM.test(input);
}
