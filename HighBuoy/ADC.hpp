#pragma once

#include "Pinout.hpp"
#include <Arduino.h>
#include <mutex>

class ADC {
public:
  static ADC& instance();

  struct Data {
    int raw_voltmeter{ 0 };
    int raw_ampermeter{ 0 };

    int custom_adc_1{ 0 };
    int custom_adc_2{ 0 };

    float voltmeter{ 0.0f };
    float ampermeter{ 0.0f };
  };

  void begin();
  void update();
  Data& getData();
  void printADC();
  void printExtraPins();

private:
  ADC();
  ~ADC();

  ADC(ADC const&) = delete;
  ADC& operator=(ADC const&) = delete;

  Data data;

  std::mutex mutex;
};