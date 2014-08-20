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
#ifdef EL_DEBUG
    Serial.println("Failed to send request");
#endif
    return;
  }

  // Wait for reply
  NRF24_rxMode();

  uint16_t i = 0;
  while(!NRF24_dataAvailable()){
    if (i++ > EL_REPLY_TIMEOUT_MS) {
#ifdef EL_DEBUG
      Serial.println("Timeout");
#endif
      return;
    }
    _delay_ms(1);
  }

  uint8_t data[SPOKE_ENV_LEN];
  NRF24_fetch(data, SPOKE_ENV_LEN);

  if (data[0] != SPOKE_RESP_ENV) {
#ifdef EL_DEBUG
    Serial.println("Bad response");
    return;
#endif
  }

#ifdef EL_DEBUG
  Serial.print("Logging to EEPROM at");
  Serial.puth(EL_headByteAddr >> 8);
  Serial.print(" ");
  Serial.puth(EL_headByteAddr & 0xFF);
  Serial.println("");
#endif

  // Send two bytes to flash at EL_headByteAddr
  uint8_t dataToWrite[2] = { data[SPOKE_ENV_HUMIDITY_IDX], data[SPOKE_ENV_TEMPERATURE_IDX] };
  DataFlash_write(EL_FLASH_PAGE_ADDR, EL_headByteAddr, dataToWrite, 2, false);

  // Increment EL_headByteAddr by two bytes & wrap if needed & write it back
  EL_headByteAddr += 2;
  if ((EL_headByteAddr) >= EL_MAX_DATA_BYTE_ADDR) {
    EL_headByteAddr = EL_MIN_DATA_BYTE_ADDR;
  }

#ifdef EL_DEBUG
  Serial.print("Writing EEPROM head addr to ");
  Serial.puth(EL_headByteAddr >> 8);
  Serial.print(" ");
  Serial.puth(EL_headByteAddr & 0xFF);
  Serial.println("");
#endif

  uint8_t headBytes[2] = { EL_headByteAddr >> 8, EL_headByteAddr & 0xFF };
  DataFlash_write(EL_FLASH_PAGE_ADDR, EL_HEAD_BYTE_ADDR, headBytes, 2, false);
}

void EnvironmentalLogging_readLog(uint8_t* buf, uint16_t len) {
  // Read up to last (len / 2) readings
  // EL_ANALYZE_BUF_LEN is EL_ANALYZE_READINGS * 2 because each reading is 2 bytes

  if (EL_headByteAddr >= len) {
    // In this case, we can just the last len bytes straight in
#ifdef EL_DEBUG
    Serial.print("Straight read from ");
    Serial.putb(EL_headByteAddr);
    Serial.print(" for ");
    Serial.putb(len);
    Serial.println(" bytes");
#endif
    DataFlash_read(EL_FLASH_PAGE_ADDR, EL_headByteAddr - len, buf, len);
  } else {
    // Here we need wrap. Example (assuming EL_ANALYZE_READINGS is 10)
    // We're going to analyze 10 readings (20 bytes)
    // Lets say head address is 10. This means we need to get the last 10
    // then the first 10 bytes

    // Compute the indexes and read the tail straight in to the buffer
    uint16_t tailBytesToRead = abs(EL_headByteAddr - len);
    uint16_t tailAddrToRead = (EL_MAX_DATA_BYTE_ADDR - tailBytesToRead);
#ifdef EL_DEBUG
    Serial.print("Tail read from ");
    Serial.putb(tailAddrToRead);
    Serial.print(" for ");
    Serial.putb(tailBytesToRead);
    Serial.println(" bytes");
#endif
    DataFlash_read(EL_FLASH_PAGE_ADDR, tailAddrToRead, buf, tailBytesToRead);


    // Read the head in to a temporary buffer then copy it to the end of the main buffer
    uint8_t headBuf[EL_headByteAddr];

    // FIXME this probably breaks when EL_MIN_DATA_BYTE_ADDR is not 0
    uint16_t headBytesToRead = (EL_headByteAddr);
    // END FIXME

    uint16_t headAddrToRead = EL_MIN_DATA_BYTE_ADDR;
#ifdef EL_DEBUG
    Serial.print("Head read from ");
    Serial.putb(headAddrToRead);
    Serial.print(" for ");
    Serial.putb(headBytesToRead);
    Serial.println(" bytes");
#endif
    DataFlash_read(EL_FLASH_PAGE_ADDR, headAddrToRead, headBuf, headBytesToRead);

    memcpy(&buf[tailBytesToRead], headBuf, headBytesToRead);
  }
}