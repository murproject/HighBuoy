#include "DataManager.hpp"

DataManager::DataManager() = default;
DataManager::~DataManager() = default;

DataManager &DataManager::instance() {
  static DataManager instance;
  return instance;
}

void DataManager::storeData(void *args) {
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  while (true) {
    storeDepth();
    sendDepth();

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(sampling_freq));
  }
}

void DataManager::storeDepth() {
  if (isDataResetRequired) {
    mission_doc.clear();
    JsonObject root = mission_doc.to<JsonObject>();
    missionArray = root.createNestedArray("mission");
    isDataResetRequired = false;
  }

  if (DepthRegulator::instance().getData().mission_started.load()) {
    if (missionArray.isNull()) {
      missionArray = mission_doc.to<JsonArray>();
    }

    if (measureJson(mission_doc) < (bufferSizeDepth - 100)) {
      JsonArray subArray = missionArray.createNestedArray();
      subArray.add(RTC::instance().getData().uptime - RTC::instance().getData().mission_start_time);
      subArray.add(DepthSensor::instance().getData().formatted_depth);
      subArray.add(DepthSensor::instance().getData().formatted_temp);
    }
  }
}

void DataManager::sendDepth() {
  if (isDataSendRequired) {
    JsonArray missionArray = mission_doc["mission"].as<JsonArray>();
    size_t size = serializeJson(missionArray, mission_msg, sizeof(mission_msg));

    uint16_t checksum = Crc16(reinterpret_cast<uint8_t *>(mission_msg), size);
    mission_doc["checksum"] = checksum;

    serializeJson(mission_doc, mission_msg);
    ApiBroker::instance().publish(mission_topic, mission_msg);

    Serial.println("Send mission data");
    isDataSendRequired = false;
  }
}

void DataManager::taskReport(void *args) {
  mac_addr = WifiControl::instance().getMacAddress();
  while (true) {
    const bool connected = ApiBroker::instance().isConnected();
    if (connected) {
      sendReport();
    } else {
      Serial.println("Not connected!");
    }
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}

void DataManager::sendReport() {
  telemetry_doc.clear();

  JsonObject msg = telemetry_doc.createNestedObject("msg");

  msg["mac_address"] = mac_addr;
  msg["timestamp"] = RTC::instance().getTimestamp();
  msg["color"] = LedControl::instance().getColor();
  msg["uptime"] = RTC::instance().getData().uptime;
  msg["potentiometer"] = Potentiometer::instance().getPercent();
  msg["depth"] = DepthSensor::instance().getData().formatted_depth;
  msg["pressure"] = DepthSensor::instance().getData().formatted_pressure;
  msg["temperature"] = DepthSensor::instance().getData().formatted_temp;
  msg["voltmeter"] = ADC::instance().getData().voltmeter;
  msg["ampermeter"] = ADC::instance().getData().ampermeter;
  telemetry_doc["checksum"] = 0;

  size_t msg_size = serializeJson(msg, telemetry_msg, sizeof(telemetry_msg));
  uint16_t checksum = Crc16(reinterpret_cast<uint8_t *>(telemetry_msg), msg_size);
  telemetry_doc["checksum"] = checksum;

  serializeJson(telemetry_doc, telemetry_msg);
  ApiBroker::instance().publish(telemetry_topic, telemetry_msg);
}

unsigned short DataManager::Crc16(unsigned char *pcBlock, unsigned short len) {
  unsigned short crc = 0xFFFF;
  unsigned char i;

  while (len--) {
    crc ^= *pcBlock++ << 8;

    for (i = 0; i < 8; i++)
      crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
  }
  return crc;
}

void DataManager::processCommands(void *args) {
  while (true) {
    if (ApiBroker::instance().isNewMessage()) {
      auto [topic, msg] = ApiBroker::instance().getLastMessage();
      Serial.print("Received: ");
      Serial.println(topic.c_str());
      Serial.println(msg.c_str());

      RTC::instance().updateKillswitch();

      JsonDocument doc;
      deserializeJson(doc, msg);

      sendResponse(topic, doc);

      if (topic == "control") handleControlCommand(doc);
      else if (topic == "start") handleStartCommand(doc);
      else if (topic == "get_data") isDataSendRequired = true;
      else if (topic == "finish") DepthRegulator::instance().getData().mission_started.store(false);
      else if (topic == "set_pid") handleSetPID(doc);
      else if (topic == "get_pid") handleGetPID();
      else if (topic == "set_buoyancy") handleSetBuoyancy(doc);
      else if (topic == "get_buoyancy") handleGetBuoyancy();
      else if (topic == "set_time") handleSetTime(doc);

      ApiBroker::instance().resetMessageFlag();
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void DataManager::sendResponse(const std::string &topic, const JsonDocument &doc) {
  StaticJsonDocument<512> response_doc;
  response_doc["topic"] = topic;
  response_doc["msg"] = doc;

  char buffer[512];
  serializeJson(response_doc, buffer, sizeof(buffer));
  ApiBroker::instance().publish("response", buffer);
}


void DataManager::handleControlCommand(const JsonDocument &doc) {
  Motor::instance().getData().debug_mode.store(true);

  if (doc["led"].is<const char *>()) {
    LedControl::instance().setColor(doc["led"].as<const char *>());
  }
  if (doc["power"].is<int>()) {
    Motor::instance().getData().power.store(doc["power"].as<int>());
  }
  if (doc["time"].is<int>()) {
    Motor::instance().getData().end_time.store(RTC::instance().getData().uptime + doc["time"].as<int>());
  }
}

void DataManager::handleStartCommand(const JsonDocument &doc) {
  isDataResetRequired = true;
  if (doc["target_depth"].is<float>()) {
    DepthRegulator::instance().getData().target_depth.store(doc["target_depth"].as<float>());
  }
  if (doc["freq"].is<uint>() && doc["freq"].as<uint>() >= 1) {
    sampling_freq = doc["freq"].as<uint>() * 1000; 
  }
  if (doc["time"].is<int>()) {
    RTC::instance().getData().mission_start_time = RTC::instance().getData().uptime;
    RTC::instance().getData().mission_end_time = RTC::instance().getData().uptime + doc["time"].as<int>();
  }
  DepthRegulator::instance().getData().mission_started.store(true);
  Serial.println("Mission started");
}

void DataManager::handleSetPID(const JsonDocument &doc) {
  if (doc["p"].is<float>()) {
    DepthRegulator::instance().getData().P.store(doc["p"].as<float>());
  }
  if (doc["i"].is<float>()) {
    DepthRegulator::instance().getData().I.store(doc["i"].as<float>());
  }
  if (doc["d"].is<float>()) {
    DepthRegulator::instance().getData().D.store(doc["d"].as<float>());
  }
  DepthRegulator::instance().updatePID();
}


void DataManager::handleGetPID() {
  StaticJsonDocument<512> pid_reg;
  pid_reg["p"] = DepthRegulator::instance().getData().P.load();
  pid_reg["i"] = DepthRegulator::instance().getData().I.load();
  pid_reg["d"] = DepthRegulator::instance().getData().D.load();

  char buffer[512];
  serializeJson(pid_reg, buffer, sizeof(buffer));
  ApiBroker::instance().publish("pid", buffer);
}

void DataManager::handleSetBuoyancy(const JsonDocument &doc) {
  if (doc["val"].is<int>()) {
    DepthRegulator::instance().getData().zero_buoyancy.store(doc["val"].as<int>());
  }
}

void DataManager::handleGetBuoyancy() {
  StaticJsonDocument<512> buoancy;
  buoancy["val"] = DepthRegulator::instance().getData().zero_buoyancy.load();

  char buffer[512];
  serializeJson(buoancy, buffer, sizeof(buffer));
  ApiBroker::instance().publish("zero_buoyancy", buffer);
}

void DataManager::handleSetTime(const JsonDocument &doc) {
  uint8_t h, m, s;
  if (doc["h"].is<uint8_t>()) {
    h = doc["h"].as<uint8_t>();
  }
  if (doc["m"].is<uint8_t>()) {
    m = doc["m"].as<uint8_t>();
  }
  if (doc["s"].is<uint8_t>()) {
    s = doc["s"].as<uint8_t>();
  }

  RTC::instance().adjustTimeManually(h, m, s);
}
