#include <stdint.h>
#include <pins.h>
#include <Serial.h>
#include <util/delay.h>

#define LED_RED _BV(0)
#define LED_YLW _BV(1)
#define LED_GRN _BV(2)

#define LED_R   (LED_RED)
#define LED_RY  (LED_RED | LED_YLW)
#define LED_RG  (LED_RED | LED_GRN)
#define LED_G   (LED_RED | LED_YLW)
#define LED_GR  (LED_GRN | LED_RED)
#define LED_GY  (LED_GRN | LED_YLW)
#define LED_Y   (LED_YLW)
#define LED_YR  (LED_YLW | LED_RED)
#define LED_YG  (LED_YLW | LED_GRN)
#define LED_OFF 0
#define LED_ALL (LED_RED | LED_YLW | LED_GRN)

void DiagLEDS_init() {
  RED_LED_ToOutput();
  YLW_LED_ToOutput();
  GRN_LED_ToOutput();
}

void DiagLEDS_set(uint8_t mode) {
  (mode & LED_RED) ? RED_LED_High() : RED_LED_Low();
  (mode & LED_YLW) ? YLW_LED_High() : YLW_LED_Low();
  (mode & LED_GRN) ? GRN_LED_High() : GRN_LED_Low();
}

void DiagLEDS_test(uint8_t input) {
  Serial.print("\r\n -- DIAG LED TEST -- \r\n");

  DiagLEDS_set(input - 60);
  _delay_ms(100);
  DiagLEDS_set(LED_OFF);
}