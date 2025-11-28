#include "Potentiometer.hpp"

using namespace Pinout::Potentiometer;

Potentiometer::Potentiometer() = default;
Potentiometer::~Potentiometer() = default;

Potentiometer& Potentiometer::instance() {
  static Potentiometer instance;
  return instance;
}

void Potentiometer::begin() {
  pinMode(pinPot, INPUT);
}

void Potentiometer::update() {
  int raw = analogRead(pinPot);
  std::scoped_lock lock(mutex);
  raw_pot = raw;
  int mapped_values = map(
    raw,
    minRange,
    maxRange,
    -100, 100
  );
  percent_pot = constrain(mapped_values, -100, 100);
}

void Potentiometer::print() {
  std::scoped_lock lock(mutex);
  Serial.printf(
    "Raw potentiometer: %i,\tPotentiometer: %i\n",
    raw_pot,
    percent_pot
  );
}

int Potentiometer::getRaw() {
  std::scoped_lock lock(mutex);
  return raw_pot;
}

int Potentiometer::getPercent() {
  std::scoped_lock lock(mutex);
  return percent_pot;
}