#include "MqttController.h"

MqttController::MqttController() {
}

void MqttController::init(String serverName, String deviceType, String deviceId, String username, String password) {
  Logger::log("Init MQTT Controller...", Logger::INFO_LOG);
  this->serverName = serverName;
  this->port = 1883;
  this->deviceId = deviceId;
  this->username = username;
  this->password = password;
  this->deviceType = deviceType;
  updateTopics();
  setup_mqtt();
}

void MqttController::setup_mqtt() {
  Logger::log("SetUp MQTT config...", Logger::DEBUG_LOG);
  char mqtt_server_chars[32];
  serverName.toCharArray(mqtt_server_chars, 32);
  mqtt_client.setServer(mqtt_server_chars, port);
  mqtt_client.setClient(wifiClient);
  mqtt_client.setCallback([this](char* topic, byte * payload, unsigned int length) {
    MqttController::callback(topic, payload, length);
  });
  connect_mqtt();
}

boolean MqttController::connect_mqtt()
{
  if(username == "" || password == "") return false;
  Logger::log("Trying MQTT server connection: " + serverName, Logger::INFO_LOG);
  Logger::log("USER: " + username + "| PASS: " + password, Logger::DEBUG_LOG);
  char mqtt_clientname_chars[16];
  char mqtt_username_chars[32];
  char mqtt_password_chars[32];
  deviceId.toCharArray(mqtt_clientname_chars, 16);
  username.toCharArray(mqtt_username_chars, 32);
  password.toCharArray(mqtt_password_chars, 32);
  if (mqtt_client.connect(mqtt_clientname_chars, mqtt_username_chars, mqtt_password_chars)) {
    char deviceSubcribeTopic_chars[80];
    deviceSubcribeTopic.toCharArray(deviceSubcribeTopic_chars, 80);
    mqtt_client.subscribe(deviceSubcribeTopic_chars);
    return true;
  }
  return false;
}

String MqttController::payloadToString(byte* payload, unsigned int length)
{
  String mensajeRecibido = "";
  for (int i = 0; i < length; i++) {
    mensajeRecibido += (char)payload[i];
  }
  return mensajeRecibido;
}

boolean MqttController::isMqttConnected() {
  boolean isConnect = mqtt_client.connected();
  if (!isConnect) {
    Logger::log("Mqtt is not connected", Logger::DEBUG_LOG);
  }
  return isConnect;
}

boolean MqttController::reconnect_mqtt()
{
  mqtt_client.disconnect();
  delay(100);
  PubSubClient mqtt_client(wifiClient);
  setup_mqtt();
}

boolean MqttController::send_mqtt_acction(String payload)
{
  if (isMqttConnected()) {
    char payload_chars[128];
    char deviceAcctionTopic_chars[80];
    payload.toCharArray(payload_chars, 128);
    deviceAcctionTopic.toCharArray(deviceAcctionTopic_chars, 80);
    mqtt_client.publish(deviceAcctionTopic_chars, payload_chars);
    return true;
  } else {
    return false;
  }
}

boolean MqttController::send_mqtt_notification(String payload)
{
  if (isMqttConnected()) {
    char payload_chars[128];
    char deviceNotifyTopic_chars[80];
    payload.toCharArray(payload_chars, 128);
    deviceNotifyTopic.toCharArray(deviceNotifyTopic_chars, 80);
    mqtt_client.publish(deviceNotifyTopic_chars, payload_chars);
    return true;
  } else {
    return false;
  }
}

boolean MqttController::send_mqtt_available()
{
  if (isMqttConnected()) {
    char deviceAvailableTopic_chars[80];
    deviceAvailableTopic.toCharArray(deviceAvailableTopic_chars, 80);
    mqtt_client.publish(deviceAvailableTopic_chars, "online");
    return true;
  } else {
    return false;
  }
}

void MqttController::mqttLoop() {
  if (isMqttConnected()) {
    mqtt_client.loop();
  } else {
    reconnect_mqtt();
  }
}

void MqttController::stringToJson(String message) {
  DynamicJsonDocument jsonMessage(1024);
  deserializeJson(jsonMessage, message);
  JsonObject json = jsonMessage.as<JsonObject>();
  String parameter = json["parameter"];
  String value = json[parameter];
  String cidValue = json["CID"];
  Logger::log("Parameter:" + parameter + " | Value:" + value, Logger::DEBUG_LOG);
  boolean receivedChecker = MqttController::paramChanger(parameter, value);
  MqttController::sendResponse(cidValue, receivedChecker);
}

void MqttController::callback(char* topic, byte* payload, unsigned int length) {
  String receivedMessage = payloadToString(payload, length);
  Logger::log("MQTT Message recieved: " + receivedMessage, Logger::INFO_LOG);
  stringToJson(receivedMessage);
}

boolean MqttController::paramChanger(String argumentName, String argumentValue) {
  boolean wasExecuted = (*MqttController::setValues)(argumentName, argumentValue);
  Logger::log(argumentName + "=" + argumentValue + ":"+String(wasExecuted), Logger::DEBUG_LOG);
  return wasExecuted;
}

void MqttController::sendResponse(String cidValue, boolean isSuccess) {
  String jsonResponse = "{\"CID\":\"" + cidValue + "\",\"Code\":\"" + isSuccess + "\"}";
  MqttController::send_mqtt_notification(jsonResponse);
}

void MqttController::updateTopics() {
  Logger::log("Updating MQTT Topics...", Logger::DEBUG_LOG);
  String baseTopic = deviceType + "/" + deviceId;
  this->deviceSubcribeTopic = baseTopic + "/sub";
  this->deviceAcctionTopic = baseTopic + "/act";
  this->deviceNotifyTopic = baseTopic + "/not";
  this->deviceAvailableTopic = baseTopic + "/available";

}

void MqttController::setServerName(String serverName) {
  this->serverName = serverName;
}

void MqttController::setUsername(String username) {
  this->username = username;
}

void MqttController::setPassword(String password) {
  this->password = password;
}

void MqttController::setDeviceId(String deviceId) {
  this->deviceId = deviceId;
  updateTopics();
}

void MqttController::setSetValues(boolean (setFunction)(String commandName, String value)) {
  this->setValues = setFunction;
}
void MqttController::setGetValue(String (setFuntion)(String commandName)) {
  this->getValue = setFuntion;
}

String MqttController::getServerName() {
  return serverName;
}

String MqttController::getUsername() {
  return username;
}

String MqttController::getPassword() {
  return password;
}

String MqttController::getDeviceId() {
  return deviceId;
}
