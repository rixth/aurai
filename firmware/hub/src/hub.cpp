#include <avr/interrupt.h>
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
#include "EnvironmentalLogger.h"

#include <hub.h>

volatile uint8_t cycleCount = 0;
volatile uint32_t secondsSinceBoot = 0;

ISR(TIMER0_COMPA_vect) {
  cycleCount++;
  if (cycleCount == 36) {
    secondsSinceBoot++;
    cycleCount = 0;
  }
}

int main() {
  Flash_init();
  Serial.begin();
  SPI.begin();
  DiagLEDS_init();
  NRF24_init();
  sei();

  while (1) {
    Serial.println("Aurai HUB.");
    Serial.println(" [X] Command line");
    Serial.println(" [S] Binary serial interface");
    Serial.println(" [B] Boot");
    Serial.println(" [?] Enter test mode");
    Serial.print("Will auto-choose [S] in 3 seconds... ");

    uint8_t cmd = 's';

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
      Serial.println("Booting!");
    }

    if (cmd == 'x') {
      commonStart();
      DiagLEDS_set(LED_YLW);
      CommandLine_start();
    } else if (cmd == 's' || cmd == 'b') {
      Serial.println("Starting Serial interface!");
      commonStart();
      DiagLEDS_set(LED_GRN);
      boot();
    } else {
      // mainTest(cmd);
    }
  }

  return 0;
}

void boot() {
  while(1) {
    SerialInterface_start();
  }
}

void timerInit() {
  OCR0A  = 217; // will trigger ~36hz at 8mhz
  TIMSK0 = _BV(OCIE0A);
  TCCR0A = _BV(WGM01);
  TCCR0B = _BV(CS02) | _BV(CS00);
}

void commonStart() {
  timerInit();
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
