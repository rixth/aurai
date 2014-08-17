#ifndef AirConditioner_h
#define AirConditioner_h

#define AC_MODE_COOL         0
#define AC_MODE_ENERGY_SAVER 1
#define AC_MODE_FAN          2
#define AC_MODE_DRY          3
#define AC_MODE_DEFAULT      AC_MODE_COOL

#define AC_FAN_SPD_LOW     0
#define AC_FAN_SPD_MED     1
#define AC_FAN_SPD_HIGH    2
#define AC_FAN_SPD_DEFAULT AC_FAN_SPD_LOW

#define AC_TEMP_MIN     60
#define AC_TEMP_MAX     86
#define AC_TEMP_DEFAULT 70

#define AC_TIMER_MAX_HOURS 24

#define AC_BTN_POWER   0x8166817E
#define AC_BTN_TEMP_UP 0x8166A15E
#define AC_BTN_TEMP_DN 0x816651AE
#define AC_BTN_FAN_SPD 0x81669966
#define AC_BTN_TIMER   0x8166F906
#define AC_BTN_MODE    0x8166D926

#define AC_DEFAULT_STATE_ADDRESS 0x01

class AirConditioner {
public:
  AirConditioner();
  AirConditioner(uint8_t stateMemoryAddress);

  unsigned long status();
  void reset();

  void togglePower();
  bool powerOn();
  bool powerOff();

  bool setMode(uint8_t mode);
  bool nextMode();

  bool setFanSpeed(uint8_t fanSpeed);
  bool nextFanSpeed();

  bool setTargetTemp(uint8_t target);
  bool targetTempUp();
  bool targetTempDown();

  void clearPowerTimer();
  bool setPowerOnTimer(uint8_t hours);
  bool setPowerOffTimer(uint8_t hours);

  void saveStateToMemory();
  void restoreStateFromMemory();

  bool isOn();
  uint8_t currentMode();
  uint8_t currentFanSpeed();
  uint8_t currentTargetTemp();

private:
  void _initialize(uint8_t stateMemoryAddress);

  void _sendCmd(unsigned long cmd);

  uint8_t _stateMemoryAddress;

  uint8_t _targetTemp;
  uint8_t _fanSpeed;
  uint8_t _mode;
  uint8_t _timerHours;
  bool _on;
};

#endif

