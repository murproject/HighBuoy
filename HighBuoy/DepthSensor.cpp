#include "DepthSensor.hpp"

using namespace Pinout::DepthSensor;

DepthSensor::DepthSensor()
  : sensor(&wire) {}

DepthSensor::~DepthSensor() = default;

DepthSensor& DepthSensor::instance() {
  static DepthSensor instance;
  return instance;
}

void DepthSensor::begin() {
  started = sensor.init();

  if (started) {
    Serial.println("Depth sensor started.");
  } else {
    Serial.println("Init failed!");
    Serial.println("Are SDA/SCL connected correctly?");
    Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
    Serial.println("\n\n\n");
    delay(5000);
  }

  sensor.setModel(MS5837::MS5837_30BA);
  sensor.setFluidDensity(997);

  calibrateDepthSensor();
}

void DepthSensor::update() {
  if (!started) {
    Serial.println("Error: Depth sensor not initialized.");
    return;
  }

  sensor.read();

  std::scoped_lock lock(mutex);
  data.depth = sensor.depth() - depth_error;
  data.pressure = sensor.pressure();
  data.altitude = sensor.altitude();
  data.temperature = sensor.temperature();

  data.formatted_depth = std::round(data.depth * 100.0) / 100.0;
  data.formatted_pressure = static_cast<int>(data.pressure);
  data.formatted_temp = static_cast<int>(data.temperature);
}

DepthSensor::Data& DepthSensor::getData(){
  std::scoped_lock lock(mutex);
  return data;
}

void DepthSensor::printDepth() {
  std::scoped_lock lock(mutex);
  Serial.printf(
    "Depth: %f,\tTemperature: %f,\tPressure: %f\n",
    data.depth,
    data.temperature,
    data.pressure);
}

void DepthSensor::calibrateDepthSensor() {
  Serial.println("Depth sensor calibration started");

  int counter = 0;

  while (counter < depth_calib_samples) {
    counter += 1;
    sensor.read();
    all_depth_values += sensor.depth();
  }

  depth_error = all_depth_values / depth_calib_samples;

  Serial.println("Depth sensor calibration finished");
}