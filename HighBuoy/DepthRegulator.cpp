#include "DepthRegulator.hpp"

DepthRegulator::DepthRegulator() : regulator(data.P.load(), data.I.load(), data.D.load()) {
  updatePID();
}

DepthRegulator::~DepthRegulator() = default;

DepthRegulator& DepthRegulator::instance() {
  static DepthRegulator instance;
  return instance;
}

void DepthRegulator::updatePID() {
  PidRegulator::PidConfig newConfig{
    data.P.load(),
    data.I.load(),
    data.D.load()
  };
  regulator.setConfig(newConfig);
}

bool DepthRegulator::goToSurface() {
  if (Potentiometer::instance().getPercent() < 98) {
    Motor::instance().setPower(100);
    return false;
  }
  Motor::instance().setPower(0);
  return true;
}

void DepthRegulator::setTargetBuoyancy(int target) {
  int current = Potentiometer::instance().getPercent();
  float error = target - current;
  float p = 10;
  float powerToSet = (error * p);
  powerToSet = round(powerToSet);
  powerToSet = constrain(powerToSet, -100.0, +100.0);

  if (abs(powerToSet) <= deadzone) {
    powerToSet = 0;
  }
  Motor::instance().setPower(static_cast<int>(powerToSet));
}

void DepthRegulator::applyPID() {
  float error = DepthSensor::instance().getData().depth - data.target_depth;
  PidRegulator::PidOutput output = regulator.apply(error);
  double power = output.p_part + output.i_part + output.d_part;

  setTargetBuoyancy(power + data.zero_buoyancy.load());
}
