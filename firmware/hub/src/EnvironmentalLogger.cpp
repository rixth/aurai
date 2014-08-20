#include <stdint.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <Serial.h>

#include "pins.h"
#include "EnvironmentalLogger.h"
#include "SpokeCommands.h"
#include "NRF24L01.h"
#include "EEPROM.h"
#include "Flash.h"

// These are just shims
void DataFlash_read(uint16_t pageAddr, uint16_t byteAddr, uint8_t *buf, uint8_t len) {
  Flash_read(byteAddr, buf, len);
}

// These are just shims
void DataFlash_write(uint16_t pageAddr, uint16_t byteAddr, uint8_t *val, uint8_t len, uint8_t erase) {
  Flash_write(byteAddr, val, len);
}

uint16_t EL_headByteAddr = EL_MIN_DATA_BYTE_ADDR;
uint32_t nextDataCollectionAt = EL_LOG_INTERVAL;

void EnvironmentalLogging_start() {
  // Get current pointer
  uint8_t headBytes[2] = { 0, 10 };
  DataFlash_write(EL_FLASH_PAGE_ADDR, EL_HEAD_BYTE_ADDR, headBytes, 2, 0);
  DataFlash_read(EL_FLASH_PAGE_ADDR, EL_HEAD_BYTE_ADDR, headBytes, 2);
  EL_headByteAddr = (headBytes[0] << 8) | headBytes[1];
}

void EnvironmentalLogging_logIfNeeded(uint32_t secondsSinceBoot) {
  if (secondsSinceBoot >= nextDataCollectionAt) {
    nextDataCollectionAt = secondsSinceBoot + EL_LOG_INTERVAL;
    EnvironmentalLogging_log();
  }
}

void EnvironmentalLogging_log() {
  // Send data request to spoke
  uint8_t payload[1] = { SPOKE_CMD_ENV };
  if (!NRF24_send(payload, 1)) {
    Serial.println("Failed to send request");
    return;
  }

  // Wait for reply
  NRF24_rxMode();

  uint16_t i = 0;
  while(!NRF24_dataAvailable()){
    if (i++ > EL_REPLY_TIMEOUT_MS) {
      Serial.println("Timeout");
      return;
    }
    _delay_ms(1);
  }

  uint8_t data[SPOKE_ENV_LEN];
  NRF24_fetch(data, SPOKE_ENV_LEN);

  if (data[0] != SPOKE_RESP_ENV) {
    Serial.println("Bad response");
    return;
  }

  Serial.print("Logging to EEPROM at");
  Serial.puth(EL_headByteAddr >> 8);
  Serial.print(" ");
  Serial.puth(EL_headByteAddr & 0xFF);
  Serial.println("");

  // Send two bytes to flash at EL_headByteAddr
  uint8_t dataToWrite[2] = { data[SPOKE_ENV_HUMIDITY_IDX], data[SPOKE_ENV_TEMPERATURE_IDX] };
  DataFlash_write(EL_FLASH_PAGE_ADDR, EL_headByteAddr, dataToWrite, 2, false);

  // Increment EL_headByteAddr by two bytes & wrap if needed & write it back
  EL_headByteAddr += 2;
  if ((EL_headByteAddr) >= EL_MAX_DATA_BYTE_ADDR) {
    EL_headByteAddr = EL_MIN_DATA_BYTE_ADDR;
  }

  Serial.print("Writing EEPROM head addr to");
  Serial.puth(EL_headByteAddr >> 8);
  Serial.print(" ");
  Serial.puth(EL_headByteAddr & 0xFF);
  Serial.println("");

  uint8_t headBytes[2] = { EL_headByteAddr >> 8, EL_headByteAddr & 0xFF };
  DataFlash_write(EL_FLASH_PAGE_ADDR, EL_HEAD_BYTE_ADDR, headBytes, 2, false);
}

void EnvironmentalLogging_readLog(uint8_t* buf, uint16_t len) {
  // Read up to last (len / 2) readings
  // EL_ANALYZE_BUF_LEN is EL_ANALYZE_READINGS * 2 because each reading is 2 bytes

  // printf("Head byte addr is %d\n", EL_headByteAddr);

  if (EL_headByteAddr >= len) {
    // In this case, we can just the last len bytes straight in
    // printf("Straight read from %d for %d bytes\n", EL_headByteAddr, len);
    Serial.print("Straight read from ");
    Serial.putb(EL_headByteAddr);
    Serial.print(" for ");
    Serial.putb(len);
    Serial.println(" bytes");
    DataFlash_read(EL_FLASH_PAGE_ADDR, EL_headByteAddr - len, buf, len);
  } else {
    // Here we need wrap. Example (assuming EL_ANALYZE_READINGS is 10)
    // We're going to analyze 10 readings (20 bytes)
    // Lets say head address is 10. This means we need to get the last 10
    // then the first 10 bytes

    // Compute the indexes and read the tail straight in to the buffer
    uint16_t tailBytesToRead = abs(EL_headByteAddr - len);
    uint16_t tailAddrToRead = (EL_MAX_DATA_BYTE_ADDR - tailBytesToRead);
    // printf("Tail read from %d for %d bytes\n", tailAddrToRead, tailBytesToRead);
    // Serial.print("Tail read from ");
    // Serial.putb(tailAddrToRead);
    // Serial.print(" for ");
    // Serial.putb(tailBytesToRead);
    // Serial.println(" bytes");
    DataFlash_read(EL_FLASH_PAGE_ADDR, tailAddrToRead, buf, tailBytesToRead);


    // Read the head in to a temporary buffer then copy it to the end of the main buffer
    uint8_t headBuf[EL_headByteAddr];

    // FIXME this probably breaks when EL_MIN_DATA_BYTE_ADDR is not 0
    uint16_t headBytesToRead = (EL_headByteAddr);
    // END FIXME

    uint16_t headAddrToRead = EL_MIN_DATA_BYTE_ADDR;
    // printf("Head read from %d for %d bytes\n", headAddrToRead, headBytesToRead);
    // Serial.print("Head read from ");
    // Serial.putb(headAddrToRead);
    // Serial.print(" for ");
    // Serial.putb(headBytesToRead);
    // Serial.println(" bytes");
    DataFlash_read(EL_FLASH_PAGE_ADDR, headAddrToRead, headBuf, headBytesToRead);

    memcpy(&buf[tailBytesToRead], headBuf, headBytesToRead);
  }
}