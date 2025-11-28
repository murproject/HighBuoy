#pragma once

#include "Pinout.hpp"
#include "src/MS5837/MS5837.h"
#include <Arduino.h>
#include <mutex>

class DepthSensor {
public:
  static DepthSensor& instance();

  struct Data {
    float depth{ 0.0f };
    float pressure{ 0.0f };
    float altitude{ 0.0f };
    float temperature{ 0.0f };

    float formatted_depth{ 0.0f };
    int formatted_temp{ 0 };
    int formatted_pressure{ 0 };
  };

  void begin();
  void update();
  Data& getData();
  void printDepth();

private:
  DepthSensor();
  ~DepthSensor();

  DepthSensor(DepthSensor const&) = delete;
  DepthSensor& operator=(DepthSensor const&) = delete;

  void calibrateDepthSensor();

  MS5837 sensor;
  bool started = false;
  Data data;
  float all_depth_values = 0.0;
  float depth_error = 0.0;
  int depth_calib_samples = 100;

  std::mutex mutex;
};