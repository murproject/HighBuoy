#pragma once

#include "sMQTTBroker.h"
#include <mutex>

class ApiBroker : public sMQTTBroker {
public:
  static ApiBroker& instance();

  void start(uint port);
  bool isConnected();
  void loop();
  void publish(const char *topic, const char *msg);

  std::pair<const std::string&, const std::string&> getLastMessage();
  void resetMessageFlag();
  bool isNewMessage();

private:
  ApiBroker();
  ~ApiBroker();

  ApiBroker(ApiBroker const&) = delete;
  ApiBroker& operator=(ApiBroker const&) = delete;

  bool onEvent(sMQTTEvent *event) override;

  std::mutex mutex;

  std::string topic;
  std::string msg;

  bool _started = false;
  bool newMessageReceived = false;
};
