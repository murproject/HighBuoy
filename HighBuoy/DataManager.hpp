#pragma once

#include "ArduinoJson.h"
#include "Potentiometer.hpp"
#include "DepthRegulator.hpp"
#include "WifiControl.hpp"
#include "LedControl.hpp"
#include "ApiBroker.hpp"
#include "ADC.hpp"
#include <mutex>

class DataManager {
public:
  static DataManager &instance();

  void storeData(void *args);
  void taskReport(void *args);
  void processCommands(void *args);

  void sendReport();

  void storeDepth();
  void sendDepth();

private:
  DataManager();
  ~DataManager();

  DataManager(DataManager const &) = delete;
  DataManager &operator=(DataManager const &) = delete;

  unsigned short Crc16(unsigned char *pcBlock, unsigned short len);
  void sendResponse(const std::string &topic, const JsonDocument &doc);
  void handleControlCommand(const JsonDocument &doc);
  void handleStartCommand(const JsonDocument &doc);
  void handleSetPID(const JsonDocument &doc);
  void handleGetPID();
  void handleSetBuoyancy(const JsonDocument &doc);
  void handleGetBuoyancy();
  void handleSetTime(const JsonDocument &doc);

  static const size_t bufferSize = 1024;
  char telemetry_msg[bufferSize] = { '\0' };
  StaticJsonDocument<bufferSize> telemetry_doc;

  static const size_t bufferSizeDepth = 15360;
  char mission_msg[bufferSizeDepth] = { '\0' };
  StaticJsonDocument<bufferSizeDepth> mission_doc;

  JsonArray missionArray;

  const char *telemetry_topic = "telemetry";
  const char *mission_topic = "mission_data";

  uint sampling_freq = 1000;

  bool isDataSendRequired = false;
  bool isDataResetRequired = false;

  std::string mac_addr = "";
};
