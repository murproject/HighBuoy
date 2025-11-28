#include <mutex>
#include "ADC.hpp"

using namespace Pinout::ADC;

ADC::ADC() = default;
ADC::~ADC() = default;

ADC& ADC::instance() {
  static ADC instance;
  return instance;
}

void ADC::begin() {
  pinMode(pinVoltmeter, INPUT);
  pinMode(pinAmpermeter, INPUT);

  pinMode(customADC1, INPUT);
  pinMode(customADC2, INPUT);

  analogRead(pinAmpermeter);
  analogSetPinAttenuation(pinAmpermeter, ADC_6db);
}

void ADC::update() {
  std::scoped_lock lock(mutex);
  data.raw_voltmeter  = analogRead(pinVoltmeter);
  data.raw_ampermeter = analogRead(pinAmpermeter);

  int constrained_volts = constrain(data.raw_voltmeter, fromMinVolt, fromMaxVolt);

  data.voltmeter  = map(constrained_volts, fromMinVolt, fromMaxVolt, toMinVolt, toMaxVolt) / 10.0;
  data.ampermeter = map(data.raw_ampermeter, fromMinAmp, fromMaxAmp, toMinAmp, toMaxAmp) / 100.0;

  data.custom_adc_1 = analogRead(customADC1);
  data.custom_adc_2 = analogRead(customADC2);
}

ADC::Data& ADC::getData() {
  std::scoped_lock lock(mutex);
  return data;
}

void ADC::printADC() {
  std::scoped_lock lock(mutex);
  Serial.printf(
    "Voltmeter: %f, Raw voltmeter: %i, Ampermeter: %f, Raw ampermeter: %i\n",
    data.voltmeter,
    data.raw_voltmeter,
    data.ampermeter,
    data.raw_ampermeter);
}

void ADC::printExtraPins() {
  std::scoped_lock lock(mutex);
  Serial.printf(
    "custom_adc_1: %i, custom_adc_2: %i\n",
    data.custom_adc_1,
    data.custom_adc_2);
}
