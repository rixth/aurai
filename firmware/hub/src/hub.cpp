#include <util/delay.h>

#include <pins.h>
#include <SPI.h>
#include <DiagLEDS.h>
#include <Serial.h>
#include <Flash.h>
#include <EEPROM.h>
#include <NRF24L01.h>
#include <SpokeCommands.h>
#include "CommandLine.h"
#include "SerialInterface.h"

#include <hub.h>

int main() {
  Flash_init();
  Serial.begin();
  SPI.begin();
  DiagLEDS_init();
  NRF24_init();

  while (1) {
    Serial.println("Aurai HUB.");
    Serial.println(" [C] Command line");
    Serial.println(" [S] Binary serial interface");
    Serial.println(" [B] Boot");
    Serial.println(" [?] Enter test mode");
    Serial.print("Will auto-choose [S] in 3 seconds... ");

    uint8_t cmd;

    uint16_t i = 0;
    while (++i < 3000) {
      if (Serial.available()) {
        cmd = Serial.read();
        break;
      }
      _delay_ms(1);
    };

    if (!cmd) {
      Serial.println(" timeout!");
      cmd = 's';
    }

    if (cmd == 'c') {
      commonStart();
      CommandLine_start();
    } else if (cmd == 's') {
      commonStart();
      SerialInterface_start();
    } else if (cmd == 'b') {
      commonStart();
      boot();
    } else {
      mainTest(cmd);
    }
  }

  return 0;
}

void boot() {
  Serial.println("Self-hosted interface is not yet implemented.");
}

void commonStart() {
  initializeRadio();
}

void initializeRadio() {
  NRF24_flush();
  NRF24_configure();
  NRF24_setTxAddr((uint8_t *)"spoke", 5);
  NRF24_setRxAddr((uint8_t *)"hub__", 5);
}

void mainTest(uint8_t input) {
  NRF24_test(input);
  Flash_test(input);
  DiagLEDS_test(input);
  EEPROM.test(input);
}
