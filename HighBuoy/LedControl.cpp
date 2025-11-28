#include "LedControl.hpp"

using namespace Pinout::LedStrip;

LedControl::LedControl()
  : pixels(ledsCount, pinData, NEO_GRB + NEO_KHZ800) {}

LedControl::~LedControl() = default;

LedControl& LedControl::instance() {
  static LedControl instance;
  return instance;
}

void LedControl::begin() {
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(150);
}

void LedControl::setRGB(int R, int G, int B) {
  for (int i = 0; i < ledsCount; i++) {
    pixels.setPixelColor(i, pixels.Color(R, G, B));
    pixels.show();
  }
}

void LedControl::setColor(const std::string& new_color) {
  {
    std::scoped_lock lock(mutex);
    if (color_name == new_color) {
      return;
    }
    color_name = new_color;
  }

  if (new_color == "red") {
    setRGB(255, 0, 0);
  } else if (new_color == "green") {
    setRGB(0, 255, 0);
  } else if (new_color == "blue") {
    setRGB(0, 0, 255);
  } else if (new_color == "purple") {
    setRGB(255, 0, 255);
  } else if (new_color == "yellow") {
    setRGB(255, 255, 0);
  } else {
    setRGB(0, 0, 0);
  }
}

std::string LedControl::getColor() {
  std::scoped_lock lock(mutex);
  return color_name;
}