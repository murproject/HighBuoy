#pragma once

#include "WiFiGeneric.h"
#include "WiFi.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include <string>

class WifiControl {
public:
  static WifiControl& instance();

  void setIPAddress(const IPAddress& local, const IPAddress& gateway, const IPAddress& subnet);
  void setupWiFi(std::string ssid, std::string pass, bool softAP = true);
  void handleWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);
  void fillMacAddr();
  std::string getMacAddress();

private:
  WifiControl();
  ~WifiControl();

  WifiControl(WifiControl const&) = delete;
  WifiControl& operator=(WifiControl const&) = delete;

  char mac_address[1024] = { '\0' };
  std::string shortMacAddress;

  IPAddress default_IP;
  IPAddress gateway;
  IPAddress subnet;
  bool ipConfigured = false;
};