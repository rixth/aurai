#include <avr/sfr_defs.h>

#define SPOKE_CMD_POWER            _BV(3)  // 0x8
#define SPOKE_CMD_POWER_PRM_TOGGLE (SPOKE_CMD_POWER | 0) // 0x8
#define SPOKE_CMD_POWER_PRM_ON     (SPOKE_CMD_POWER | 1) // 0x9
#define SPOKE_CMD_POWER_PRM_OFF    (SPOKE_CMD_POWER | 2) // 0xa

#define SPOKE_CMD_MODE                  _BV(4) // 0x10
#define SPOKE_CMD_MODE_PRM_NEXT         (SPOKE_CMD_MODE | 0) // 0x10
#define SPOKE_CMD_MODE_PRM_COOL         (SPOKE_CMD_MODE | 1) // 0x11
#define SPOKE_CMD_MODE_PRM_ENERGY_SAVER (SPOKE_CMD_MODE | 2) // 0x12
#define SPOKE_CMD_MODE_PRM_DRY          (SPOKE_CMD_MODE | 3) // 0x13
#define SPOKE_CMD_MODE_PRM_FAN          (SPOKE_CMD_MODE | 4) // 0x14

#define SPOKE_CMD_FAN_SPEED          (_BV(3) | _BV(4)) // 0x18
#define SPOKE_CMD_FAN_SPEED_PRM_NEXT (SPOKE_CMD_FAN_SPEED | 0) // 0x18
#define SPOKE_CMD_FAN_SPEED_PRM_LOW  (SPOKE_CMD_FAN_SPEED | 1) // 0x19
#define SPOKE_CMD_FAN_SPEED_PRM_MED  (SPOKE_CMD_FAN_SPEED | 2) // 0x1a
#define SPOKE_CMD_FAN_SPEED_PRM_HIGH (SPOKE_CMD_FAN_SPEED | 3) // 0x1b

#define SPOKE_CMD_TEMPERATURE          _BV(5) // 0x20
#define SPOKE_CMD_TEMPERATURE_PRM_UP   (SPOKE_CMD_TEMPERATURE | 0) // 0x20
#define SPOKE_CMD_TEMPERATURE_PRM_DOWN (SPOKE_CMD_TEMPERATURE | 1) // 0x21

#define SPOKE_CMD_TEMPERATURE_EXACT (_BV(3) | _BV(5)) // 0x28

#define SPOKE_CMD_RESET  (_BV(4) | _BV(6)) // 0x50
#define SPOKE_CMD_STATUS (_BV(4) | _BV(5)) // 0x30
#define SPOKE_CMD_ENV    (_BV(3) | _BV(6)) // 0x48

#define SPOKE_STATUS_LEN             3
#define SPOKE_STATUS_AC_MSB_IDX      1
#define SPOKE_STATUS_AC_LSB_IDX      2

#define SPOKE_ENV_LEN             3
#define SPOKE_ENV_HUMIDITY_IDX    1
#define SPOKE_ENV_TEMPERATURE_IDX 2

#define SPOKE_RESP_OK     _BV(0) // 0x1
#define SPOKE_RESP_FAIL   _BV(1) // 0x2
#define SPOKE_RESP_STATUS _BV(2) // 0x4
#define SPOKE_RESP_ENV    _BV(3) // 0x8
