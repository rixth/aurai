#define EL_LOG_INTERVAL 180 // seconds

#define EL_FLASH_PAGE_ADDR 0x00
#define EL_HEAD_BYTE_ADDR  126

#define EL_MAX_DATA_POINTS    50
#define EL_TOTAL_DATA_LENGTH  (EL_MAX_DATA_POINTS * 2)
#define EL_MIN_DATA_BYTE_ADDR 0
#define EL_MAX_DATA_BYTE_ADDR (EL_TOTAL_DATA_LENGTH + EL_MIN_DATA_BYTE_ADDR)

#define EL_REPLY_TIMEOUT_MS   1000

#define EL_ANALYZE_READINGS 10
#define EL_ANALYZE_BUF_LEN  (EL_ANALYZE_READINGS * 2)

#define EL_HUMIDITY_STABLE  _BV(1)
#define EL_HUMIDITY_RISING  _BV(2)
#define EL_HUMIDITY_FALLING _BV(3)

#define EL_TEMPERATURE_STABLE  _BV(4)
#define EL_TEMPERATURE_RISING  _BV(5)
#define EL_TEMPERATURE_FALLING _BV(6)

void EnvironmentalLogging_start();
void EnvironmentalLogging_logIfNeeded(uint32_t secondsSinceBoot);
void EnvironmentalLogging_log();
void EnvironmentalLogging_readLog(uint8_t* buf, uint16_t len);