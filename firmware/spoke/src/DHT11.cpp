#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

#include "DHT11.h"
#include "pins.h"
#include "time.h"

int8_t DHT11__temperature = 0;
int8_t DHT11__humidity = 0;

uint8_t DHT11_humidity() {
  return DHT11__humidity;
}

int8_t DHT11_temperature() {
  return DHT11__temperature;
}

uint8_t DHT11_readSensor() {
  uint8_t bytes[5] = { 0, 0, 0, 0, 0 };
  uint8_t bitsToClockIn = 40;
  uint8_t bitIdx = 7;
  uint8_t byteIdx = 0;

  DHT11_ToOutput();
  DHT11_Low();
  _delay_ms(18);
  DHT11_High();
  _delay_us(40);
  DHT11_ToInput();

  // Input should go high then low
  if (!DHT11__waitFor(true) || !DHT11__waitFor(false)) {
    return DHT11_TIMEOUT_ERR;
  }

  for (int i = 0; i < bitsToClockIn; i++) {
    // Wait for high or timeout
    if (!DHT11__waitFor(true)) {
      return DHT11_TIMEOUT_ERR;
    }

    unsigned long t = Time_micros();

    // Wait for low or timeout
    if (!DHT11__waitFor(false)) {
      return DHT11_TIMEOUT_ERR;
    }

    if ((Time_micros() - t) > 40) {
      bytes[byteIdx] |= _BV(bitIdx);
    }

    if (bitIdx == 0) {
      bitIdx = 7;
      byteIdx++;
    } else {
      bitIdx--;
    }
  }

  DHT11__temperature = bytes[2];
  DHT11__humidity = bytes[0];

  if (bytes[4] != bytes[0] + bytes[2]) {
    return DHT11_TIMEOUT_ERR;
  }

  DHT11_ToOutput();
  DHT11_High();

  return DHT11_OK;
}

double DHT11_dewPoint() {
  // WARNING.
  // Using this requires floating point math, and the associated library
  // It adds 1,534 bytes to the output
  double b = 17.271;
  double c = 237.7;
  double tmp = (b * DHT11__temperature) / (c + DHT11__temperature) + log(DHT11__humidity * 0.01);
  return (c * tmp) / (b - tmp);
}

bool DHT11__waitFor(bool waitFor) {
  unsigned int loopCnt = 10000;

  while(DHT11_Read() != (uint8_t) waitFor) {
    if (loopCnt-- == 0) {
      return false;
    }
  }

  return true;
}