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

   // Read up to len bytes
  uint8_t buf[SERIAL_BUFFER_LENTH];

  uint8_t len = Serial.read();
  if (len == 0 || len > SERIAL_BUFFER_LENTH) {
    buf[0] = 0xFF;
  } else {
    uint8_t i;
    for (i = 0; i < len; i++) {
      buf[i] = Serial.read();
    }
  }

  // Data packet format:
  // SSSL[DDDDDDDDDDDDDDDDDD]
  // S - status
  //  0 - processing successful
  //  1 - send successful
  //  2 - spoke resonse received
  // L - data length
  // D - data

  if (buf[0] == SERIAL_CMD_PIPE) {
    Serial.print(SERIAL_CMD_OK);
    SerialInterface_pipeBufferToSpoke(&buf[1], len - 1);
  } else if (buf[0] == SERIAL_CMD_ENV_LOG) {
    Serial.print(SERIAL_CMD_OK);
    Serial.print(SERIAL_SEND_OK);
    Serial.print(SERIAL_RESP_RECEIVED);
    Serial.print(buf[1]);
    uint8_t data[255];
    EnvironmentalLogging_readLog(data, buf[1]);
    uint8_t i;
    for (i = 0; i < buf[1]; i++) {
      Serial.print(data[i]);
    }
  } else {
    Serial.print(SERIAL_CMD_FAILED);
    Serial.print((uint8_t) 0x00);
    Serial.print((uint8_t) 0x00);
    Serial.print((uint8_t) 0x00);
  }
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
      Serial.print((uint8_t) 0x00);
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
