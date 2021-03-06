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
  Serial.begin();
  SPI.begin();
  DiagLEDS_init();
  NRF24_init();

  while (1) {
    DiagLEDS_set(LED_ALL);
    _delay_ms(100);
    DiagLEDS_set(0);

    Serial.println("Aurai SPOKE.");
    Serial.println(" [B] Boot");
    Serial.println(" [?] Enter test mode");
    Serial.print("Will auto-choose [B] in 3 seconds... ");

    uint8_t cmd = 'b';

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
    } else {
      Serial.println("");
    }

    if (cmd == 'b') {
      Serial.println("Booting!");
      boot();
    } else {
      // mainTest(cmd);
    }
  }

  return 0;
}

void boot() {
  initializeRadio();
  ac.restoreStateFromMemory();
  DiagLEDS_set(LED_GRN);
  while (!Serial.available()) {
    if (NRF24_dataAvailable()) {
      processIncomingData();
      ac.saveStateToMemory();
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

  bool success = false;
  bool sendReply = true;

  if (cmd == SPOKE_CMD_POWER) {
    if (data[0] == SPOKE_CMD_POWER_PRM_TOGGLE) {
      ac.togglePower();
      success = true;
    } else if (data[0] == SPOKE_CMD_POWER_PRM_OFF) {
      success = ac.powerOff();
    } else if (data[0] == SPOKE_CMD_POWER_PRM_ON) {
      success = ac.powerOn();
    }
  } else if (cmd == SPOKE_CMD_TEMPERATURE) {
    if (data[0] == SPOKE_CMD_TEMPERATURE_PRM_UP) {
      success = ac.targetTempUp();
    } else if (data[0] == SPOKE_CMD_TEMPERATURE_PRM_DOWN) {
      success = ac.targetTempDown();
    }
  } else if (cmd == SPOKE_CMD_TEMPERATURE_EXACT) {
    if (len == 2) {
      success = ac.setTargetTemp(data[1]);
    }
  } else if (cmd == SPOKE_CMD_FAN_SPEED) {
    if (data[0] == SPOKE_CMD_FAN_SPEED_PRM_NEXT) {
      success = ac.nextFanSpeed();
    } else if (data[0] == SPOKE_CMD_FAN_SPEED_PRM_LOW) {
      success = ac.setFanSpeed(AC_FAN_SPD_LOW);
    } else if (data[0] == SPOKE_CMD_FAN_SPEED_PRM_MED) {
      success = ac.setFanSpeed(AC_FAN_SPD_MED);
    } else if (data[0] == SPOKE_CMD_FAN_SPEED_PRM_HIGH) {
      success = ac.setFanSpeed(AC_FAN_SPD_HIGH);
    }
  } else if (cmd == SPOKE_CMD_MODE) {
    if (data[0] == SPOKE_CMD_MODE_PRM_NEXT) {
      success = ac.nextMode();
    } else if (data[0] == SPOKE_CMD_MODE_PRM_COOL) {
      success = ac.setMode(AC_MODE_COOL);
    } else if (data[0] == SPOKE_CMD_MODE_PRM_ENERGY_SAVER) {
      success = ac.setMode(AC_MODE_ENERGY_SAVER);
    } else if (data[0] == SPOKE_CMD_MODE_PRM_DRY) {
      success = ac.setMode(AC_MODE_DRY);
    } else if (data[0] == SPOKE_CMD_MODE_PRM_FAN) {
      success = ac.setMode(AC_MODE_FAN);
    }
  } else if (cmd == SPOKE_CMD_RESET) {
    ac.reset();
    success = true;
  } else if (cmd == SPOKE_CMD_STATUS) {
    uint16_t acStatus = ac.status();
    uint8_t reply[SPOKE_STATUS_LEN] = { \
      SPOKE_RESP_STATUS,
      (uint8_t) (acStatus >> 8),
      (uint8_t) (acStatus & 0xFF)
    };

    Serial.print("Sending status: ");
    if (NRF24_send(reply, SPOKE_STATUS_LEN)) {
      Serial.println("ok");
    } else {
      Serial.println("failed");
    }
    success = true;
  } else if (cmd == SPOKE_CMD_ENV) {
    DHT11_readSensor();
    uint8_t reply[SPOKE_ENV_LEN] = { \
      SPOKE_RESP_ENV,
      DHT11_humidity(),
      (uint8_t) DHT11_temperature(),
    };

    Serial.print("Sending environment: ");
    if (NRF24_send(reply, SPOKE_ENV_LEN)) {
      Serial.println("ok");
    } else {
      Serial.println("failed");
    }
    success = true;
  }

  if (sendReply) {
    uint16_t acStatus = ac.status();
    uint8_t reply[SPOKE_STATUS_LEN] = { \
      SPOKE_RESP_OK,
      (uint8_t) (acStatus >> 8),
      (uint8_t) (acStatus & 0xFF)
    };

    if (success) {
      Serial.print("Sending success reply: ");
    } else {
      Serial.print("Sending failure reply: ");
      reply[0] = SPOKE_RESP_FAIL;
    }

    if (NRF24_send(reply, SPOKE_STATUS_LEN)) {
      Serial.println("ok");
    } else {
      Serial.println("failed");
    }
  }

  DiagLEDS_set(success ? LED_GRN : LED_RED);
  if (!sendReply) {
    Serial.println(success ? "Command succeeded." : "Command failed.");
  }
}

void mainTest(uint8_t input) {
  NRF24_test(input);
  IRSend_test();
  DHT11_test();
  EEPROM.test(input);
  DiagLEDS_test(input);
}
