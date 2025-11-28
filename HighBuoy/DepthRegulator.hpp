#pragma once

#include "Motor.hpp"
#include "src/PID/PidRegulator.h"
#include "Potentiometer.hpp"
#include "DepthSensor.hpp"


class DepthRegulator {
public:
  static DepthRegulator& instance();

  struct Data {
    std::atomic<bool> mission_started{ false };
    std::atomic<float> target_depth{ 0.0f };
    std::atomic<float> P{ 150.0 };
    std::atomic<float> I{ 0.0 };
    std::atomic<float> D{ 0.0 };
    std::atomic<int> zero_buoyancy{ 10 };
  };

  Data& getData() {
    return data;
  }

  void updatePID();
  bool goToSurface();
  void applyPID();

private:
  DepthRegulator();
  ~DepthRegulator();

  DepthRegulator(DepthRegulator const&) = delete;
  DepthRegulator& operator=(DepthRegulator const&) = delete;

  void setTargetBuoyancy(int target);

  PidRegulator regulator;

  Data data;
  uint8_t deadzone = 30;
};