#include "I2C.hpp"

using namespace Pinout::I2C;

I2C::I2C() = default;
I2C::~I2C() = default;

I2C& I2C::instance() {
  static I2C instance;
  return instance;
}

void I2C::begin() {
  static bool isInitialized = false;

  if (!isInitialized) {
    Bus0::wire.begin(Bus0::pinSDA, Bus0::pinSCL, Bus0::freq);
    Bus0::wire.setTimeout(Bus0::timeout);

    Bus1::wire.begin(Bus1::pinSDA, Bus1::pinSCL, Bus1::freq);
    Bus1::wire.setTimeout(Bus1::timeout);

    Serial.println("I2C bus initialized");
  }
}

void I2C::scan(TwoWire &wire) {
  byte error, address;
  int nDevices = 0;

  Serial.println("I2C scanner started.");
  for (address = 0x01; address < 0x7f; address++) {
    wire.beginTransmission(address);
    error = wire.endTransmission();
    if (error == 0) {
      Serial.printf("- I2C device found at address 0x%02X\n", address);
      nDevices++;
    } else if (error != 2) {
      Serial.printf("- Error %d at address 0x%02X\n", error, address);
    }
  }

  if (nDevices == 0) {
    Serial.println("- No devices found on this I2C bus");
  }

  Serial.println();
}

void I2C::scanAll() {
  Serial.println();

  Serial.printf("Scanning for I2C devices on bus %d\n", Bus0::busNum);
  scan(Bus0::wire);

  Serial.printf("Scanning for I2C devices on bus %d\n", Bus1::busNum);
  scan(Bus1::wire);
}
