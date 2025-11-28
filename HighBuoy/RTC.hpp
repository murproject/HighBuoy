#pragma once

#include "Pinout.hpp"
#include "RTClib.h"
#include <mutex>

class RTC {
public:
  static RTC& instance();

  struct Data {
    unsigned long uptime{ 0 };
    unsigned long start_time{ 0 };
    unsigned long mission_start_time{ 0 };
    unsigned long mission_end_time{ 0 };
    unsigned long killswitch_timer{ 0 };
  };

  void begin();
  void update();
  Data& getData();

  String getTimestamp();

  void adjustTimeManually(uint8_t hour, uint8_t min, uint8_t sec);
  void printUptime();

  bool isKillswitchActivated();
  void updateKillswitch();

private:
  RTC();
  ~RTC();

  RTC(RTC const&) = delete;
  RTC& operator=(RTC const&) = delete;

  void setTimestamp(const String& new_timestamp);
  String formatTime(int value);

  RTC_DS3231 rtc;
  bool started = false;

  int time_until_shutdown = 900;
  String timestamp = "";
  Data data;
  std::mutex mutex;
};
