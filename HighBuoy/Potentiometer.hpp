#pragma once

#include "Pinout.hpp"
#include <Arduino.h>

#include <mutex>

class Potentiometer {
public:
  static Potentiometer& instance();

  void begin();
  void update();
  void print();

  int getRaw();
  int getPercent();

private:
  Potentiometer();
  ~Potentiometer();

  Potentiometer(Potentiometer const&) = delete;
  Potentiometer& operator=(Potentiometer const&) = delete;

  int raw_pot = 0;
  int percent_pot = 0;
  std::mutex mutex;
};
