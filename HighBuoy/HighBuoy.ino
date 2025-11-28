#include "I2C.hpp"
#include "DepthSensor.hpp"
#include "RTC.hpp"
#include "LedControl.hpp"
#include "Motor.hpp"
#include "ADC.hpp"
#include "ApiBroker.hpp"
#include "DataManager.hpp"
#include "DepthRegulator.hpp"
#include "Potentiometer.hpp"
#include "WifiControl.hpp"

// #define DEBUG

const std::string ssid = "HIGH_BUOY_";
const std::string password = "password";
const int mqttPort = 1999;

enum Priority {
    Low         = 3,
    Medium      = 4,
    High        = 5,
};

void setup() {
  Serial.begin(115200);
  LedControl::instance().begin();
  LedControl::instance().setColor("blue");

  WifiControl::instance().fillMacAddr();
  WifiControl::instance().setupWiFi(ssid, password);

  ApiBroker::instance().start(mqttPort);

  Motor::instance().begin();
  Potentiometer::instance().begin();
  Potentiometer::instance().update();
  ADC::instance().begin();
  I2C::instance().begin();
  RTC::instance().begin();

  if (Potentiometer::instance().getPercent() < 98) {
    Motor::instance().setPower(100);
    delay(4000);
  }

  DepthSensor::instance().begin();

  xTaskCreate([](void *arg) { DataManager::instance().taskReport(arg); },      "taskReport",      5120, NULL, Priority::Low, NULL);
  xTaskCreate([](void *arg) { DataManager::instance().processCommands(arg); }, "processCommands", 5120, NULL, Priority::Low, NULL);
  xTaskCreate([](void *arg) { DataManager::instance().storeData(arg); },       "storeData",       5120, NULL, Priority::Low, NULL);
  xTaskCreate(mission,      "mission",      5120, NULL, Priority::Medium, NULL);
  xTaskCreate(updateBroker, "updateBroker", 5120, NULL, Priority::Low,    NULL);
  xTaskCreate(killswitch,   "killswitch",   5120, NULL, Priority::High,   NULL);

  delay(100);

  LedControl::instance().setColor("green");
}

void loop() {}

void mission(void *args) {
  TickType_t xLastWakeTime;
  TickType_t xFrequency = 40;

  xLastWakeTime = xTaskGetTickCount();

  while (true) {
    updateSensors();

    if (DepthRegulator::instance().getData().mission_started.load() && !RTC::instance().isKillswitchActivated()) {
      Motor::instance().getData().debug_mode.store(false);

      if (RTC::instance().getData().uptime < RTC::instance().getData().mission_end_time) {
        LedControl::instance().setColor("red");
        DepthRegulator::instance().applyPID();
      } else {
        LedControl::instance().setColor("yellow");

        if (DepthRegulator::instance().goToSurface() && DepthSensor::instance().getData().depth < 0.05) { 
          LedControl::instance().setColor("green");
          DepthRegulator::instance().getData().mission_started.store(false);
          Serial.println("Mission ended");
        }
      }
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void updateBroker(void *args) {
  while (true) {
    ApiBroker::instance().loop();

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void killswitch(void *args) {
  while (true) {
    if (RTC::instance().isKillswitchActivated()) {
      Motor::instance().getData().debug_mode.store(false);
      DepthRegulator::instance().goToSurface();
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void updateSensors() {
  DepthSensor::instance().update();
  RTC::instance().update();
  Potentiometer::instance().update();
  ADC::instance().update();
  Motor::instance().update();
#ifdef DEBUG
  printAllData();
#endif
}

void printAllData() {
  RTC::instance().printUptime();
  DepthSensor::instance().printDepth();
  ADC::instance().printADC();
  ADC::instance().printExtraPins();
  Potentiometer::instance().print();
}
