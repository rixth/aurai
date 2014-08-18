#include <util/delay.h>
#include <stdlib.h>

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
  Serial.println("Aurai hub command line.");

  while (1) {
    uint8_t payload[32] = { 0 };
    uint8_t payloadLength = 0;
    uint8_t autosend = true;

    Serial.println("Select command:");
    Serial.println("");
    Serial.println("[P] Power control");
    Serial.println("[T] Temperature settings");
    Serial.println("[M] Mode control");
    Serial.println("[F] Fan speed control");
    Serial.println("[S] Get status from Spoke");
    Serial.println("[X] Exit command line");

    uint8_t cmd = Serial.read();

    if (cmd == 'p') {
      Serial.println("Choose subcommand:");
      Serial.println(" [T] toggle");
      Serial.println(" [0] power");
      Serial.println(" [1] power");
      cmd = Serial.read();
      if (cmd == 't') {
        Serial.print("Sending power toggle: ");
        payload[0] = SPOKE_CMD_POWER_PRM_TOGGLE;
      } else {
        Serial.println("Unknown option.");
      }
    } else if (cmd == 't') {
      Serial.println("Choose subcommand:");
      Serial.println(" [U] target up");
      Serial.println(" [D] target down");
      Serial.println(" [E] set exact target");
      cmd = Serial.read();
      if (cmd == 'u') {
        Serial.print("Setting temperature up: ");
        payload[0] = SPOKE_CMD_TEMPERATURE_PRM_UP;
      } else if (cmd == 'd') {
        Serial.print("Setting temperature down: ");
        payload[0] = SPOKE_CMD_TEMPERATURE_PRM_DOWN;
      } else if (cmd == 'e') {
        Serial.print("Enter exact temperature desired [2 digits]: ");
        const char digits[2] = { Serial.read(), Serial.read() };
        uint8_t target = atoi(digits);
        Serial.print("Setting temperature to ");
        Serial.putb(target);
        Serial.print(": ");
        payload[0] = SPOKE_CMD_TEMPERATURE_EXACT;
        payload[1] = target;
        payloadLength = 2;
      } else {
        Serial.println("Unknown option.");
      }
    } else if (cmd == 'm') {
      Serial.println("Choose subcommand:");
      Serial.println(" [N] next mode (cycle)");
      Serial.println(" [C] cool");
      Serial.println(" [E] energy saver");
      Serial.println(" [D] dry");
      Serial.println(" [F] fan only");
      cmd = Serial.read();
      if (cmd == 'n') {
        Serial.print("Setting next mode: ");
        payload[0] = SPOKE_CMD_MODE_PRM_NEXT;
      } else if (cmd == 'c') {
        Serial.print("Setting mode to cool: ");
        payload[0] = SPOKE_CMD_MODE_PRM_COOL;
      } else if (cmd == 'e') {
        Serial.print("Setting mode to energy saver: ");
        payload[0] = SPOKE_CMD_MODE_PRM_ENERGY_SAVER;
      } else if (cmd == 'd') {
        Serial.print("Setting mode to dry: ");
        payload[0] = SPOKE_CMD_MODE_PRM_DRY;
      } else if (cmd == 'f') {
        Serial.print("Setting mode to fan only: ");
        payload[0] = SPOKE_CMD_MODE_PRM_FAN;
      } else {
        Serial.println("Unknown mode.");
      }
    } else if (cmd == 'f') {
      Serial.println("Choose subcommand:");
      Serial.println(" [N] next fan speed (cycle)");
      Serial.println(" [L] low");
      Serial.println(" [M] medium");
      Serial.println(" [H] high");
      cmd = Serial.read();
      if (cmd == 'n') {
        Serial.print("Setting next fan speed: ");
        payload[0] = SPOKE_CMD_FAN_SPEED_PRM_NEXT;
      } else if (cmd == 'h') {
        Serial.print("Setting fan speed to high");
        payload[0] = SPOKE_CMD_FAN_SPEED_PRM_HIGH;
      } else if (cmd == 'm') {
        Serial.print("Setting fan speed to medium: ");
        payload[0] = SPOKE_CMD_FAN_SPEED_PRM_MED;
      } else if (cmd == 'l') {
        Serial.print("Setting fan speed to to low: ");
        payload[0] = SPOKE_CMD_FAN_SPEED_PRM_LOW;
      } else {
        Serial.println("Unknown fan speed.");
      }
    } else if (cmd == 's') {
      autosend = false;
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
    } else if (cmd == 'x') {
      Serial.println("Exiting...");
      return;
    } else {
      Serial.println("Unknown command.");
    }

    if (autosend) {
      if (NRF24_send(payload, payloadLength)) {
        Serial.println("sent.");
      } else {
        Serial.println("failed.");
      }
    }
  }
}

void mainTest(uint8_t input) {
  NRF24_test(input);
  Flash_test(input);
  DiagLEDS_test(input);
  EEPROM.test(input);
}
