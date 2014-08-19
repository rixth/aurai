#include <IRSend.h>
#include <EEPROM.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
#include "AirConditioner.h"

AirConditioner::AirConditioner() {
  _initialize(AC_DEFAULT_STATE_ADDRESS);
}

AirConditioner::AirConditioner(uint8_t stateMemoryAddress) {
  _initialize(stateMemoryAddress);
}

void AirConditioner::_initialize(uint8_t stateMemoryAddress) {
  _stateMemoryAddress = stateMemoryAddress;
  reset();
}

void AirConditioner::_sendCmd(unsigned long cmd) {
  IRSend_send(cmd, 32);
}

uint16_t AirConditioner::status() {
  uint16_t st = 0;

  st = (st | (uint16_t) _on);
  st = (st << 2) | (uint16_t) _mode;
  st = (st << 2) | (uint16_t) _fanSpeed;
  st = (st << 8) | (uint16_t) _targetTemp;

  return st;
}

void AirConditioner::reset() {
  _targetTemp = AC_TEMP_DEFAULT;
  _fanSpeed = AC_FAN_SPD_DEFAULT;
  _mode = AC_MODE_DEFAULT;
  _on = false;
}

bool AirConditioner::isOn() {
  return !!_on;
}

uint8_t AirConditioner::currentMode() {
  return _mode;
}

uint8_t AirConditioner::currentFanSpeed() {
  return _fanSpeed;
}

uint8_t AirConditioner::currentTargetTemp() {
  return _targetTemp;
}


void AirConditioner::togglePower() {
  _sendCmd(AC_BTN_POWER);
  _on = !_on;
}

bool AirConditioner::powerOn() {
  if (_on) {
    return false;
  }
  togglePower();
  return true;
}

bool AirConditioner::powerOff() {
  if (!_on) {
    return false;
  }
  togglePower();
  return true;
}

bool AirConditioner::setMode(uint8_t targetMode) {
  if (!_on ||
    (targetMode != AC_MODE_COOL &&
    targetMode != AC_MODE_ENERGY_SAVER &&
    targetMode != AC_MODE_FAN &&
    targetMode != AC_MODE_DRY)) {
    return false;
  }

  while(_mode != targetMode) {
    nextMode();
    _delay_ms(50);
  }

  return true;
}

bool AirConditioner::nextMode() {
  if (!_on) {
    return false;
  }

  _sendCmd(AC_BTN_MODE);
  if (++_mode > AC_MODE_DRY) {
    _mode = AC_MODE_COOL;
  }

  return true;
}

bool AirConditioner::setFanSpeed(uint8_t targetFanSpeed) {
  if (!_on ||
    _mode == AC_MODE_DRY ||
    (targetFanSpeed != AC_FAN_SPD_LOW &&
    targetFanSpeed != AC_FAN_SPD_MED &&
    targetFanSpeed != AC_FAN_SPD_HIGH)) {
    return false;
  }

  while(_fanSpeed != targetFanSpeed) {
    nextFanSpeed();
    _delay_ms(50);
  }

  return true;
}

bool AirConditioner::nextFanSpeed() {
  if (_on && _mode != AC_MODE_DRY) {
    _sendCmd(AC_BTN_FAN_SPD);
    if (++_fanSpeed > AC_FAN_SPD_HIGH) {
      _fanSpeed = AC_FAN_SPD_LOW;
    }
    return true;
  } else {
    return false;
  }
}

bool AirConditioner::setTargetTemp(uint8_t target) {
  if (!_on ||
    _mode == AC_MODE_FAN ||
    target > AC_TEMP_MAX ||
    target < AC_TEMP_MIN ||
    target == _targetTemp) {
    return false;
  }

  uint8_t timesToSend = abs(_targetTemp - target);

  for (uint8_t i = 0; i < timesToSend; i++) {
    if (target < _targetTemp) {
      _sendCmd(AC_BTN_TEMP_DN);
    } else {
      _sendCmd(AC_BTN_TEMP_UP);
    }
    _delay_ms(50);
  }

  _targetTemp = target;

  return true;
}

bool AirConditioner::targetTempUp() {
  return setTargetTemp(_targetTemp + 1);
}

bool AirConditioner::targetTempDown() {
  return setTargetTemp(_targetTemp - 1);
}

void AirConditioner::clearPowerTimer() {
  togglePower();
  _delay_ms(50);
  togglePower();
  _timerHours = 0;
}

bool AirConditioner::setPowerOnTimer(uint8_t hours) {
  if (hours > 0 && hours < AC_TIMER_MAX_HOURS) {
    return false;
  }

  if (_on) {
    powerOff();
  } else {
    clearPowerTimer();
  }

  for(uint8_t i = 0; i < hours; i++) {
    _delay_ms(50);
    _sendCmd(AC_BTN_TIMER);
  }

  _timerHours = hours;

  return true;
}

bool AirConditioner::setPowerOffTimer(uint8_t hours) {
  if (hours > 0 && hours < AC_TIMER_MAX_HOURS) {
    return false;
  }

  if (!_on) {
    powerOn();
  } else {
    clearPowerTimer();
  }

  for(uint8_t i = 0; i < hours; i++) {
    _delay_ms(50);
    _sendCmd(AC_BTN_TIMER);
  }

  _timerHours = hours;

  return true;
}

void AirConditioner::saveStateToMemory() {
  EEPROM.write(_stateMemoryAddress + 0, (char) _on);
  EEPROM.write(_stateMemoryAddress + 1, (char) _targetTemp);
  EEPROM.write(_stateMemoryAddress + 2, (char) _mode);
  EEPROM.write(_stateMemoryAddress + 3, (char) _fanSpeed);
  EEPROM.write(_stateMemoryAddress + 4, (char) _timerHours);
}

void AirConditioner::restoreStateFromMemory() {
   _on = EEPROM.read(_stateMemoryAddress + 0);
   _targetTemp = EEPROM.read(_stateMemoryAddress + 1);
   _mode = EEPROM.read(_stateMemoryAddress + 2);
   _fanSpeed = EEPROM.read(_stateMemoryAddress + 3);
   _timerHours = EEPROM.read(_stateMemoryAddress + 4);
}
