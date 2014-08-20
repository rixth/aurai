#include "hub.h"
#include "SerialInterface.h"

#include <util/delay.h>
#include <stdlib.h>
#include <Serial.h>
#include <DiagLEDS.h>
#include <NRF24L01.h>
#include <SpokeCommands.h>
#include <AirConditioner.h>
#include <EnvironmentalLogger.h>

void SerialInterface_start() {
  // Return if no data is available
  if (!Serial.available()) {
    return;
  }

  uint8_t cmd = Serial.read();

  // Read up to \n
  uint8_t buf[SERIAL_BUFFER_LENTH];
  uint8_t i;
  for (i = 0; i < SERIAL_BUFFER_LENTH; i++) {
    uint8_t byte = Serial.read();
    if (byte == '\n') {
      break;
    }
    buf[i] = byte;
  }

  if (cmd == SERIAL_CMD_PIPE) {
    Serial.print(SERIAL_CMD_OK);
    SerialInterface_pipeBufferToSpoke(buf, i);
  } else if (cmd == SERIAL_CMD_ENV_LOG) {
    Serial.print(SERIAL_CMD_OK);
    Serial.print(SERIAL_SEND_OK);
    Serial.print(SERIAL_RESP_ENV_LOG);
    Serial.print(buf[0]);
    uint8_t data[255];
    EnvironmentalLogging_readLog(data, buf[0]);
    for (i = 0; i < buf[0]; i++) {
      Serial.print(data[i]);
    }
  } else {
    Serial.print(SERIAL_CMD_FAILED);
  }

  Serial.println("");
}

void SerialInterface_pipeBufferToSpoke(uint8_t *buf, uint8_t len) {
  // Reply structure: CMD_OK/FAIL SEND_OK/FAIL RESP_OK/TO [SIZE DATA...] \n
  // Worth noting that the transaction may go OK but the spoke data is SPOKE_RESP_FAIL

  DiagLEDS_set(LED_GRN);

  if (NRF24_send(buf, len)) {
    DiagLEDS_set(LED_GRN | LED_YLW);
    Serial.print(SERIAL_SEND_OK);
  } else {
    DiagLEDS_set(LED_RED);
    Serial.print(SERIAL_SEND_FAIL);
  }

  // Even if we failed to send, still listen for a reply just in case
  NRF24_rxMode();

  uint16_t i = 0;
  while(!NRF24_dataAvailable()){
    if (i++ > SERIAL_REPLY_TIMEOUT_MS) {
      Serial.print(SERIAL_RESP_TIMEOUT);
      DiagLEDS_set(LED_RED | LED_YLW);
      return;
    }
    _delay_ms(1);
  }


  if (NRF24_dataAvailable()) {
    DiagLEDS_set(LED_GRN | LED_YLW | LED_RED);

    uint8_t data[5];
    uint8_t bytesReceived = NRF24_fetch(data, 5);

    Serial.print(SERIAL_RESP_RECEIVED);
    Serial.print(bytesReceived);
    for(i = 0; i < bytesReceived; i++) {
      Serial.print(data[i]);
    }
  }
}
