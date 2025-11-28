#include "RTC.hpp"

using namespace Pinout::RTC;

RTC::RTC() = default;
RTC::~RTC() = default;

RTC& RTC::instance() {
  static RTC instance;
  return instance;
}

void RTC::begin() {
  started = rtc.begin(&wire);

  if (started) {
    Serial.println("RTC started.");
    DateTime now = rtc.now();
    data.start_time = now.unixtime();

  } else {
    Serial.println("Error: RTC start failed.");
  }
}

void RTC::update() {
  if (!started) {
    Serial.println("Error: RTC not initialized.");
    return;
  }
  DateTime now = rtc.now();
  {
    std::scoped_lock lock(mutex);
    data.uptime = now.unixtime() - data.start_time;
  }

  String timestamp =
    formatTime(now.hour()) + ":" + formatTime(now.minute()) + ":" + formatTime(now.second());

  setTimestamp(timestamp);
}

RTC::Data& RTC::getData() {
  std::scoped_lock lock(mutex);
  return data;
}

void RTC::setTimestamp(const String& new_timestamp) {
  std::scoped_lock lock(mutex);
  timestamp = new_timestamp;
}

String RTC::getTimestamp() {
  std::scoped_lock lock(mutex);
  return timestamp;
}

String RTC::formatTime(int value) {
  if (value < 10) {
    return "0" + String(value);
  } else {
    return String(value);
  }
}

void RTC::adjustTimeManually(uint8_t hour, uint8_t min, uint8_t sec) {
  hour = constrain(hour, 0, 23);
  min = constrain(min, 0, 59);
  sec = constrain(sec, 0, 59);

  DateTime now(2025, 7, 22, hour, min, sec); 
  rtc.adjust(now);

  std::scoped_lock lock(mutex);
  data.start_time = now.unixtime();
  data.killswitch_timer = 0;
  data.uptime = 0;

}

void RTC::printUptime() {
  std::scoped_lock lock(mutex);
  Serial.printf(
    "Timestamp: %s,\tUptime: %i\n", 
    timestamp, 
    data.uptime);
}

bool RTC::isKillswitchActivated() {
  std::scoped_lock lock(mutex);
  return data.uptime - data.killswitch_timer > time_until_shutdown;
}

void RTC::updateKillswitch() {
  std::scoped_lock lock(mutex);
  data.killswitch_timer = data.uptime;
}
