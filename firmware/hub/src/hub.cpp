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

  enterCommandLine();
}

void enterCommandLine() {
  // P - power
  //  T - toggle
  //  1 - on
  //  0 - off
  // T - temp
  //  U - up
  //  D - down
  //  E - exact
  // M - mode
  //  N - next (cycle)
  //  E - energy saver
  //  F - fan
  //  D - dry
  // F - fan speed
  //  N - next (cycle)
  //  L - low
  //  M - medium
  //  H - high
  // S - status report
  // X - exit

  Serial.println("Aurai hub command line.");

  while (1) {
    Serial.println("-- Select command:");
    Serial.println("[P] Power control");
    Serial.println("[T] Temperature settings");
    Serial.println("[M] Mode control");
    Serial.println("[F] Fan speed control");
    Serial.println("[S] Get status from Spoke");
    Serial.println("[X] Exit command line");

    uint8_t cmd = Serial.read();

    if (cmd == 'p') {
      Serial.println(" [T] toggle");
      Serial.println(" [0] power");
      Serial.println(" [1] power");
      cmd = Serial.read();
      if (cmd == 't') {
        Serial.print("Sending power toggle: ");
        uint8_t payload[1] = { SPOKE_CMD_POWER_PRM_TOGGLE };
        if (NRF24_send(payload, 1)) {
          Serial.println("sent.");
        } else {
          Serial.println("failed.");
        }
      } else {
        Serial.println("Unknown option.");
      }
    } else if (cmd == 't') {
      Serial.println(" [U] target up");
      Serial.println(" [D] target down");
      Serial.println(" [E] set exact target");
    } else if (cmd == 'm') {
      Serial.println(" [N] next mode (cycle)");
      Serial.println(" [C] cool");
      Serial.println(" [E] energy saver");
      Serial.println(" [D] dry");
      Serial.println(" [F] fan only");
    } else if (cmd == 'f') {
      Serial.println(" [N] next fan speed (cycle)");
      Serial.println(" [L] low");
      Serial.println(" [M] medium");
      Serial.println(" [H] high");
    } else if (cmd == 's') {
      Serial.print("Sending status request packet: ");
      uint8_t payload[1] = { SPOKE_CMD_STATUS };
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
    } else if (cmd == 'x') {
      Serial.println("Exiting...");
      return;
    } else {
      Serial.println("Unknown command.");
    }
  }
}

void mainTest(uint8_t input) {
  NRF24_test(input);
  Flash_test(input);
  DiagLEDS_test(input);
  EEPROM.test(input);
}
