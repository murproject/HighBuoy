#pragma once

#include "Pinout.hpp"
#include <Adafruit_NeoPixel.h>
#include <string>

#include <mutex>

class LedControl {
public:
  static LedControl& instance();

  void begin();
  void setColor(const std::string& new_color);
  std::string getColor();

private:
  void setRGB(int R, int G, int B);

  LedControl();
  ~LedControl();

  LedControl(LedControl const&) = delete;
  LedControl& operator=(LedControl const&) = delete;

  Adafruit_NeoPixel pixels;

  std::string color_name = "";
  std::mutex mutex;
};
