#pragma once

#include "Wire.h"

namespace Pinout {
  namespace I2C {
    namespace Bus0 {
      constexpr uint8_t busNum = 0;
      constexpr uint8_t pinSDA = 11;
      constexpr uint8_t pinSCL = 12;
      constexpr long int freq = 100000;
      constexpr long int timeout = 10000;
      inline TwoWire wire = TwoWire(busNum);
    }

    namespace Bus1 {
      constexpr uint8_t busNum = 1;
      constexpr uint8_t pinSDA = 13;
      constexpr uint8_t pinSCL = 14;
      constexpr long int freq = 100000;
      constexpr long int timeout = 10000;
      inline TwoWire wire = TwoWire(busNum);
    }
  }
  namespace DepthSensor {
    inline TwoWire &wire = I2C::Bus0::wire;
  }

  namespace Potentiometer {
    constexpr int pinPot = 17; 
    constexpr int minRange = 155;
    constexpr int maxRange = 3900;
  }

  namespace RTC {
    inline TwoWire &wire = I2C::Bus1::wire;
  }

  namespace Motor {
      constexpr int pinA = 16;
      constexpr int pinB = 15;
  }

  namespace ADC {
      constexpr int pinVoltmeter = 1;
      constexpr int fromMinVolt = 765;
      constexpr int fromMaxVolt = 1172;
      constexpr int toMinVolt = 80;
      constexpr int toMaxVolt = 120;

      constexpr int pinAmpermeter = 2;
      constexpr int fromMinAmp = 0;
      constexpr int fromMaxAmp = 1087;
      constexpr int toMinAmp = 1;
      constexpr int toMaxAmp = 101;

      constexpr int customADC1 = 4;
      constexpr int customADC2 = 5;
  }

  namespace LedStrip {
      constexpr int pinData = 48;
      constexpr int ledsCount = 1;
  }
}