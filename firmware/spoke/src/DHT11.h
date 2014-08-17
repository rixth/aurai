#include <stdint.h>

#ifndef DHT11_h
#define DHT11_h

#define DHT11_OK            0
#define DHT11_CHECKSUM_ERR  1
#define DHT11_TIMEOUT_ERR   2

uint8_t DHT11_readSensor();
uint8_t DHT11_humidity();
int8_t DHT11_temperature();
double DHT11_dewPoint();
int8_t DHT11__waitFor(bool waitFor);

void DHT11_test();

#endif
