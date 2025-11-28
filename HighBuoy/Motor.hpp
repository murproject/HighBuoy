#pragma once

#include "Potentiometer.hpp"
#include "Pinout.hpp"
#include "RTC.hpp"
#include <atomic>

class Motor {
public:
  static Motor& instance();

  struct Data {
    std::atomic<int8_t> power{ 0 };
    std::atomic<uint> end_time{ 0 };
    std::atomic<bool> debug_mode{ true };
  };

  Data& getData() {
    return data;
  }

  void begin();
  void update();
  void setPower(int8_t power);
  void applyPower(uint end_time, int8_t power);

private:
  Motor();
  ~Motor();

  Motor(Motor const&) = delete;
  Motor& operator=(Motor const&) = delete;

  Data data;
};