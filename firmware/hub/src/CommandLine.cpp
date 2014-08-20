#include "hub.h"
#include "CommandLine.h"
#include "EnvironmentalLogger.h"

#include <util/delay.h>
#include <stdlib.h>
#include <Serial.h>
#include <NRF24L01.h>
#include <SpokeCommands.h>
#include <AirConditioner.h>

void CommandLine_start() {
  initializeRadio();

  Serial.println("Aurai hub command line.");

  while (1) {
    Serial.println("Select command:");
    Serial.println("");
    Serial.println("[P] Power control");
    Serial.println("[T] Temperature settings");
    Serial.println("[M] Mode control");
    Serial.println("[F] Fan speed control");
    Serial.println("[S] Get status from Spoke");
    Serial.println("[E] Get environment report from Spoke");
    Serial.println("[R] Reset AC state machine");
    Serial.println("[D] Dump enviromental log");
    Serial.println("[X] Exit command line");

    uint8_t cmd = Serial.read();

    if (cmd == 'p') {
      CommandLine_subcommandPower();
    } else if (cmd == 't') {
      CommandLine_subcommandTemperature();
    } else if (cmd == 'm') {
      CommandLine_subcommandMode();
    } else if (cmd == 'f') {
      CommandLine_subcommandFanSpeed();
    } else if (cmd == 's') {
      CommandLine_subcommandStatus();
    } else if (cmd == 'e') {
      CommandLine_subcommandEnvironment();
    } else if (cmd == 'r') {
      CommandLine_subcommandReset();
    } else if (cmd == 'd') {
      CommandLine_subcommandDumpLog();
    } else if (cmd == 'x') {
      Serial.println("Exiting...");
      return;
    } else {
      Serial.println("Unknown command.");
    }
  }
}

void CommandLine_subcommandPower() {
  Serial.println("");
  Serial.println("Choose subcommand:");
  Serial.println(" [T] toggle");
  Serial.println(" [0] power off");
  Serial.println(" [1] power on");
  uint8_t cmd = Serial.read();

  if (cmd == 't') {
    Serial.print("Sending power toggle: ");
    uint8_t payload[1] = { SPOKE_CMD_POWER_PRM_TOGGLE };
    CommandLine__handleBasicCommand(payload, 1);
    CommandLine__waitForResponse();
  } else if (cmd == '1') {
    Serial.print("Sending power on: ");
    uint8_t payload[1] = { SPOKE_CMD_POWER_PRM_ON };
    CommandLine__handleBasicCommand(payload, 1);
    CommandLine__waitForResponse();
  } else if (cmd == '0') {
    Serial.print("Sending power off: ");
    uint8_t payload[1] = { SPOKE_CMD_POWER_PRM_OFF };
    CommandLine__handleBasicCommand(payload, 1);
    CommandLine__waitForResponse();
  } else {
    Serial.println("Unknown option.");
  }
}

void CommandLine_subcommandTemperature() {
  Serial.println("Choose subcommand:");
  Serial.println(" [U] target up");
  Serial.println(" [D] target down");
  Serial.println(" [E] set exact target");
  uint8_t cmd = Serial.read();

  if (cmd == 'u') {
    Serial.print("Setting temperature up: ");
    uint8_t payload[1] = { SPOKE_CMD_TEMPERATURE_PRM_UP };
    CommandLine__handleBasicCommand(payload, 1);
    CommandLine__waitForResponse();
  } else if (cmd == 'd') {
    Serial.print("Setting temperature down: ");
    uint8_t payload[1] = { SPOKE_CMD_TEMPERATURE_PRM_DOWN };
    CommandLine__handleBasicCommand(payload, 1);
    CommandLine__waitForResponse();
  } else if (cmd == 'e') {
    Serial.print("Enter exact temperature desired [2 digits]: ");
    const char digits[2] = { Serial.read(), Serial.read() };
    uint8_t target = atoi(digits);
    Serial.println("");
    Serial.print("Setting temperature to ");
    Serial.putb(target);
    Serial.print(": ");
    uint8_t payload[2] = { SPOKE_CMD_TEMPERATURE_EXACT, target };
    CommandLine__handleBasicCommand(payload, 2);
    CommandLine__waitForResponse();
  } else {
    Serial.println("Unknown option.");
  }
}

void CommandLine_subcommandMode() {
  Serial.println("Choose subcommand:");
  Serial.println(" [N] next mode (cycle)");
  Serial.println(" [C] cool");
  Serial.println(" [E] energy saver");
  Serial.println(" [D] dry");
  Serial.println(" [F] fan only");
  uint8_t cmd = Serial.read();

  if (cmd == 'n') {
    Serial.print("Setting next mode: ");
    uint8_t payload[1] = { SPOKE_CMD_MODE_PRM_NEXT };
    CommandLine__handleBasicCommand(payload, 1);
    CommandLine__waitForResponse();
  } else if (cmd == 'c') {
    Serial.print("Setting mode to cool: ");
    uint8_t payload[1] = { SPOKE_CMD_MODE_PRM_COOL };
    CommandLine__handleBasicCommand(payload, 1);
    CommandLine__waitForResponse();
  } else if (cmd == 'e') {
    Serial.print("Setting mode to energy saver: ");
    uint8_t payload[1] = { SPOKE_CMD_MODE_PRM_ENERGY_SAVER };
    CommandLine__handleBasicCommand(payload, 1);
    CommandLine__waitForResponse();
  } else if (cmd == 'd') {
    Serial.print("Setting mode to dry: ");
    uint8_t payload[1] = { SPOKE_CMD_MODE_PRM_DRY };
    CommandLine__handleBasicCommand(payload, 1);
    CommandLine__waitForResponse();
  } else if (cmd == 'f') {
    Serial.print("Setting mode to fan only: ");
    uint8_t payload[1] = { SPOKE_CMD_MODE_PRM_FAN };
    CommandLine__handleBasicCommand(payload, 1);
    CommandLine__waitForResponse();
  } else {
    Serial.println("Unknown mode.");
  }
}

void CommandLine_subcommandFanSpeed() {
  Serial.println("Choose subcommand:");
  Serial.println(" [N] next fan speed (cycle)");
  Serial.println(" [L] low");
  Serial.println(" [M] medium");
  Serial.println(" [H] high");
  uint8_t cmd = Serial.read();

  if (cmd == 'n') {
    Serial.print("Setting next fan speed: ");
    uint8_t payload[1] = { SPOKE_CMD_FAN_SPEED_PRM_NEXT };
    CommandLine__handleBasicCommand(payload, 1);
    CommandLine__waitForResponse();
  } else if (cmd == 'h') {
    Serial.print("Setting fan speed to high: ");
    uint8_t payload[1] = { SPOKE_CMD_FAN_SPEED_PRM_HIGH };
    CommandLine__handleBasicCommand(payload, 1);
    CommandLine__waitForResponse();
  } else if (cmd == 'm') {
    Serial.print("Setting fan speed to medium: ");
    uint8_t payload[1] = { SPOKE_CMD_FAN_SPEED_PRM_MED };
    CommandLine__handleBasicCommand(payload, 1);
    CommandLine__waitForResponse();
  } else if (cmd == 'l') {
    Serial.print("Setting fan speed to to low: ");
    uint8_t payload[1] = { SPOKE_CMD_FAN_SPEED_PRM_LOW };
    CommandLine__handleBasicCommand(payload, 1);
    CommandLine__waitForResponse();
  } else {
    Serial.println("Unknown fan speed.");
  }
}

void CommandLine_subcommandDumpLog() {
  Serial.println("Starting logger");
  EnvironmentalLogging_start();
  uint8_t data[EL_TOTAL_DATA_LENGTH];
  Serial.println("Getting log data");
  EnvironmentalLogging_readLog(data, EL_TOTAL_DATA_LENGTH);
  Serial.println("Got logger data");
  uint16_t i;
  for (i = 0; i < EL_TOTAL_DATA_LENGTH; i += 2) {
    Serial.print("H: ");
    Serial.putb(data[i]);
    Serial.print("% T: ");
    Serial.putb(data[i + 1]);
    Serial.println("C");
  }
  Serial.println("Done");
}

void CommandLine_subcommandReset() {
  Serial.print("Sending reset packet: ");
  uint8_t payload[1] = { SPOKE_CMD_RESET };
  CommandLine__handleBasicCommand(payload, 1);
  CommandLine__waitForResponse();
  Serial.println("Please reset the AC to:");
  Serial.println(" - mode: cool");
  Serial.println(" - temperature: 70");
  Serial.println(" - fan speed: low");
  Serial.println(" - turn the AC off");
}

void CommandLine_subcommandStatus() {
  Serial.print("Sending status request packet: ");
  uint8_t payload[1] = { SPOKE_CMD_STATUS };
  CommandLine__handleBasicCommand(payload, 1);

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

  Serial.println("");

  uint8_t data[SPOKE_STATUS_LEN];
  uint8_t len = NRF24_fetch(data, SPOKE_STATUS_LEN);

  if (data[0] != SPOKE_RESP_STATUS) {
    Serial.print("Bad status response. Got ");
    Serial.putb(len);
    Serial.print(" bytes: ");
    for (i = 0; i < len; i++) {
      Serial.putb(data[i]);
      Serial.print(" - ");
    }
    Serial.println("");
    return;
  }

  uint16_t acStatus = (data[SPOKE_STATUS_AC_MSB_IDX] << 8) | data[SPOKE_STATUS_AC_LSB_IDX];

  Serial.print("AC: ");
  Serial.println(AC_STATUS_ON(acStatus) ? "on" : "off");
  Serial.print("Mode: ");
  if (AC_STATUS_MODE(acStatus) == AC_MODE_COOL) {
    Serial.println("cool");
  } else if (AC_STATUS_MODE(acStatus) == AC_MODE_ENERGY_SAVER) {
    Serial.println("energy saver");
  } else if (AC_STATUS_MODE(acStatus) == AC_MODE_FAN) {
    Serial.println("fan only");
  } else if (AC_STATUS_MODE(acStatus) == AC_MODE_DRY) {
    Serial.println("dry");
  } else {
    Serial.println("unknown");
  }

  Serial.print("Fan speed: ");
  if (AC_STATUS_FAN(acStatus) == AC_FAN_SPD_LOW) {
    Serial.println("low");
  } else if (AC_STATUS_FAN(acStatus) == AC_FAN_SPD_MED) {
    Serial.println("medium");
  } else if (AC_STATUS_FAN(acStatus) == AC_FAN_SPD_HIGH) {
    Serial.println("high");
  } else {
    Serial.println("unknown");
  }

  Serial.print("Temperature target: ");
  Serial.putb(AC_STATUS_TEMP(acStatus));
  Serial.println("");
}

void CommandLine_subcommandEnvironment() {
  Serial.print("Sending environment status request packet: ");
  uint8_t payload[1] = { SPOKE_CMD_ENV };
  CommandLine__handleBasicCommand(payload, 1);

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

  Serial.println("");

  uint8_t data[SPOKE_ENV_LEN];
  uint8_t len = NRF24_fetch(data, SPOKE_ENV_LEN);

  if (data[0] != SPOKE_RESP_ENV) {
    Serial.print("Bad environment status response. Got ");
    Serial.putb(len);
    Serial.print(" bytes: ");
    for (i = 0; i < len; i++) {
      Serial.putb(data[i]);
      Serial.print(" - ");
    }
    Serial.println("");
    return;
  }

  Serial.print("Humidity: ");
  Serial.putb(data[SPOKE_ENV_HUMIDITY_IDX]);
  Serial.println("%");

  Serial.print("Temperature: ");
  Serial.putb(data[SPOKE_ENV_TEMPERATURE_IDX]);
  Serial.println("c");
}

void CommandLine__handleBasicCommand(uint8_t* payload, uint8_t len) {
  if (NRF24_send(payload, len)) {
    Serial.println("sent.");
  } else {
    Serial.println("failed.");
  }
}

void CommandLine__waitForResponse() {
  Serial.print("Waiting for response: ");

  NRF24_rxMode();

  int i = 0;
  while(!NRF24_dataAvailable()){
    if (i++ > 1000) {
      Serial.println("timeout");
      return;
    }
    _delay_ms(1);
  }

  uint8_t data[10];
  NRF24_fetch(data, 10);

  if (data[0] == SPOKE_RESP_OK) {
    Serial.println("ok");
  } else if (data[0] == SPOKE_RESP_FAIL) {
    Serial.println("failed");
  } else {
    Serial.print("unknown reply: ");
    Serial.puth(data[0]);
    Serial.println("");
  }
}
