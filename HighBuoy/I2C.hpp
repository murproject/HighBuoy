#pragma once

#include "Pinout.hpp"
#include "Wire.h"
#include <Arduino.h>

class I2C {
public:
  static I2C& instance();

  void begin();
  void scan(TwoWire& wire);
  void scanAll();
  
  private:
    I2C();
    ~I2C();

    I2C(I2C const&) = delete;
    I2C& operator=(I2C const&) = delete;
};