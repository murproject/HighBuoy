#include "Motor.hpp"

using namespace Pinout::Motor;

Motor::Motor() = default;
Motor::~Motor() = default;

Motor& Motor::instance() {
  static Motor instance;
  return instance;
}

void Motor::begin() {
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
}

void Motor::update() {
  if (data.debug_mode) {
    applyPower(data.end_time.load(), data.power.load());
  }
}

void Motor::setPower(int8_t power) {
  uint8_t calc_power = map(abs(power), 0, 100, 0, 255);
  uint8_t powerA = 0;
  uint8_t powerB = 0;

  if (power < 0) {
    powerA = calc_power;
    powerB = 0;
  } else {
    powerA = 0;
    powerB = calc_power;
  }

  analogWrite(pinA, powerA);
  analogWrite(pinB, powerB);
}

void Motor::applyPower(uint end_time, int8_t power) {
  uint current_time = RTC::instance().getData().uptime;

  if (current_time < end_time) {
    setPower(power);
  } else {
    data.power.store(0);
    setPower(0);
  }
}