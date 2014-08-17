#include <stdint.h>

#define LED_GRN _BV(0)
#define LED_RED _BV(1)
#define LED_YLW _BV(2)

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

void DiagLEDS_init();
void DiagLEDS_set(uint8_t mode);
void DiagLEDS_test(uint8_t input);