#include "hub.h"
#include "SerialInterface.h"

#include <util/delay.h>
#include <stdlib.h>
#include <Serial.h>
#include <NRF24L01.h>
#include <SpokeCommands.h>
#include <AirConditioner.h>

void SerialInterface_start() {
  while (1) {
    // Loop while no data is available
    while (!Serial.available()) {
      _delay_ms(1);
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
    } else {
      Serial.print(SERIAL_CMD_FAILED);
    }

    Serial.println("");
  }
}

void SerialInterface_pipeBufferToSpoke(uint8_t *buf, uint8_t len) {
  // Reply structure: CMD_OK/FAIL SEND_OK/FAIL RESP_OK/TO [SIZE DATA...] \n
  // Worth noting that the transaction may go OK but the spoke data is SPOKE_RESP_FAIL

  if (NRF24_send(buf, len)) {
    Serial.print(SERIAL_SEND_OK);
  } else {
    Serial.print(SERIAL_SEND_FAIL);
  }

  // Even if we failed to send, still listen for a reply just in case
  NRF24_rxMode();

  uint16_t i = 0;
  while(!NRF24_dataAvailable()){
    if (i++ > SERIAL_REPLY_TIMEOUT_MS) {
      Serial.print(SERIAL_RESP_TIMEOUT);
    }
    _delay_ms(1);
  }


  if (NRF24_dataAvailable()) {
    uint8_t data[5];
    uint8_t bytesReceived = NRF24_fetch(data, 5);

    Serial.print(SERIAL_RESP_RECEIVED);
    Serial.print(bytesReceived);
    for(i = 0; i > bytesReceived; i++) {
      Serial.print(data[i]);
    }
  }
}
