#include "ApiBroker.hpp"

ApiBroker::ApiBroker() = default;
ApiBroker::~ApiBroker() = default;

ApiBroker& ApiBroker::instance() {
  static ApiBroker instance;
  return instance;
}

void ApiBroker::start(uint port) {
  if (_started) {
    return;
  }

  if (sMQTTBroker::init(port)) {
    Serial.println("MQTT broker started");
    _started = true;
  } else {
    Serial.println("Failed to start MQTT broker");
    _started = false;
  }
}

bool ApiBroker::isConnected() {
  return _started;
}

void ApiBroker::loop() {
  sMQTTBroker::update();
}

void ApiBroker::publish(const char *topic, const char *msg) {
  sMQTTBroker::publish(topic, msg, 1, true);
}

bool ApiBroker::onEvent(sMQTTEvent *event) {
  switch (event->Type()) {
    case NewClient_sMQTTEventType:
      {
        sMQTTNewClientEvent *e = (sMQTTNewClientEvent *)event;
        e->Login();
        e->Password();
      }
      break;
    case LostConnect_sMQTTEventType:
      WiFi.reconnect();
      break;
    case UnSubscribe_sMQTTEventType:
    case Subscribe_sMQTTEventType:
      {
        sMQTTSubUnSubClientEvent *e = (sMQTTSubUnSubClientEvent *)event;
        sMQTTClient *client;
      }
      break;
    case Public_sMQTTEventType:
      {
        sMQTTPublicClientEvent *e = (sMQTTPublicClientEvent *)event;
        std::scoped_lock lock(mutex);
        topic = e->Topic();
        msg = e->Payload();

        newMessageReceived = true;
      }
      break;
  }
  return true;
}

std::pair<const std::string&, const std::string&> ApiBroker::getLastMessage() {
  std::scoped_lock lock(mutex);
  return { topic, msg };
}

void ApiBroker::resetMessageFlag() {
  std::scoped_lock lock(mutex);
  newMessageReceived = false;
}

bool ApiBroker::isNewMessage() {
  std::scoped_lock lock(mutex);
  return newMessageReceived;
}