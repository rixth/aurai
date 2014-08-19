#define SERIAL_BUFFER_LENTH 11

#define SERIAL_REPLY_TIMEOUT_MS 1000

// These come from the serial interface command processing
#define SERIAL_CMD_PIPE     0x01
#define SERIAL_CMD_FAILED   0x02
#define SERIAL_CMD_OK       0x03

// These come from the radio hardware on the hub
#define SERIAL_SEND_OK      0x01
#define SERIAL_SEND_FAIL    0x02

// Did we hear back from the spoke?
#define SERIAL_RESP_TIMEOUT  0x03
#define SERIAL_RESP_RECEIVED 0x04

void SerialInterface_start();
void SerialInterface_pipeBufferToSpoke(uint8_t *buf, uint8_t len);
