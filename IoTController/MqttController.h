#ifndef MqttController_h
#define MqttController_h

#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Logger.h"


class MqttController {
  private:
    boolean (*setValues)(String commandName, String value);
    String (*getValue)(String commandName);
    String serverName;
    int port;
    String deviceId;
    String deviceType;
    String username;
    String password;
    String deviceSubcribeTopic;
    String deviceAcctionTopic;
    String deviceNotifyTopic;
    String deviceAvailableTopic;
    WiFiClient wifiClient;
    PubSubClient mqtt_client;
    String payloadToString(byte* payload, unsigned int length);
    boolean connect_mqtt();
    void stringToJson(String mensaje);
    void callback(char* topic, byte* payload, unsigned int length);
  public:
    MqttController();
    void init(String serverName, String deviceType, String deviceId, String username, String password);
    void setup_mqtt();
    boolean isMqttConnected();
    boolean reconnect_mqtt();
    boolean send_mqtt_acction(String payload);
    boolean send_mqtt_notification(String payload);
    boolean send_mqtt_available();
    void mqttLoop();
    boolean paramChanger(String argument, String value);
    void sendResponse(String cidValue, boolean isSuccess);
    void updateTopics();
    void setServerName(String serverName);
    void setUsername(String username);
    void setPassword(String password);
    void setDeviceId(String deviceId);
    void setSetValues(boolean (setFunction)(String commandName, String value));
    void setGetValue(String (setFuntion)(String commandName));
    String getServerName();
    String getUsername();
    String getPassword();
    String getDeviceId();
};
#endif
