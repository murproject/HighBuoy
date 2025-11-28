#pragma once

#include "WifiControl.hpp"

WifiControl::WifiControl() = default;
WifiControl::~WifiControl() = default;

WifiControl& WifiControl::instance() {
  static WifiControl instance;
  return instance;
}

void WifiControl::setIPAddress(const IPAddress& local, const IPAddress& gw, const IPAddress& sn) {
  default_IP = local;
  gateway = gw;
  subnet = sn;
  ipConfigured = true;
}

void WifiControl::setupWiFi(std::string ssid, std::string pass, bool softAP) {
  WiFi.setTxPower(WIFI_POWER_20dBm);

  if (softAP) {
    std::string address = ssid + shortMacAddress;

    if (!ipConfigured) {
      default_IP = IPAddress(192, 168, 4, 1);
      gateway = IPAddress(192, 168, 4, 1);
      subnet = IPAddress(255, 255, 255, 0);
    }

    WiFi.softAPConfig(default_IP, gateway, subnet);
    WiFi.softAP(address.c_str(), pass.c_str());

    Serial.printf("IP address: %s\n", WiFi.softAPIP().toString().c_str());

    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
      this->handleWiFiEvent(event, info);
    });
  } else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
  }

  auto wifiPower = WiFi.getTxPower();
  Serial.printf("Current WiFi power. Raw: %d, converted: %.2f dBm\n",
                wifiPower,
                static_cast<float>(wifiPower) * 0.25f);
}

void WifiControl::handleWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
      Serial.println("Somebody connected to the ESP32 SoftAP");
      break;
    default:
      break;
  }
}

void WifiControl::fillMacAddr() {
  uint8_t macAddr[6] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
  };

  esp_efuse_mac_get_default(macAddr);

  sprintf(mac_address,
          "%02x:%02x:%02x:%02x:%02x:%02x",
          macAddr[0], macAddr[1],
          macAddr[2], macAddr[3],
          macAddr[4], macAddr[5]);

  Serial.printf("Mac address: %s\n", mac_address);

  shortMacAddress = std::string(mac_address).substr(6, 17);
}

std::string WifiControl::getMacAddress() {
  return mac_address;
}