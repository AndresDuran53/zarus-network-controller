#include "IoTController.h"

RedController IoTController::redController;
MqttController IoTController::mqttController;
String IoTController::deviceHasToRestart = "0";
String IoTController::deviceType = "iot-device";
String IoTController::deviceUniqueName = String(DEVICE_TOKEN);
String IoTController::deviceToken = String(DEVICE_TOKEN);
uint8_t IoTController::defaultConsoleLevel = Logger::PRINT_LOG;

void IoTController::setupLogger() {
  Serial.begin(115200);
  Serial.println();
  Logger::systemLogLevel = defaultConsoleLevel;
  Logger::log("Starting Systems...", Logger::INFO_LOG);
  Logger::log("The Logger level is: " + String(Logger::systemLogLevel), Logger::INFO_LOG);
}

void IoTController::setDeviceToRestart(String restartDevice) {
  deviceHasToRestart = restartDevice;
}

void IoTController::restartDeviceDueCommand() {
  if (deviceHasToRestart == "1") {
    Logger::log("Restarting Device Safety", Logger::INFO_LOG);
    changeStoredValue(RESTART_DEVICE_COMMAND_NAME, "0");
    ESP.restart();
  }
}

void IoTController::fillCommonData() {
  Logger::log("Filling Common data...", Logger::INFO_LOG);
  createStoredData("restart_device", RESTART_DEVICE_COMMAND_NAME, 2, "0", "String",
      [](String restartDevice) {
        setDeviceToRestart(restartDevice);
  });
  //Net Values
  createStoredData("red_deviceSsid", DEVICE_NAME_COMMAND_NAME, 32, deviceUniqueName, "String",
      [](String deviceSsid) {
        redController.setDeviceSSID(deviceSsid);
  });
  createStoredData("red_devicePswd", DEVICE_PASSWORD_COMMAND_NAME, 32, DEVICE_PASSWORD, "String",
      [](String devicePassword) {
        redController.setDevicePassword(devicePassword);
  });
  createStoredData("red_ssid", RED_NAME_COMMAND_NAME, 32, "", "String",
      [](String redSsid) {
        redController.setSsid(redSsid);
  });
  createStoredData("red_pswd", RED_PASSWORD_COMMAND_NAME, 32, "", "String",
      [](String redPassword) {
        redController.setPassword(redPassword);
  });
  //MQTT Values
  createStoredData("mqtt_Server", MQTT_SERVER_NAME, 32, "", "String",
      [](String ServerName) {
        mqttController.setServerName(ServerName);
  });
  createStoredData("mqtt_User", MQTT_USER_NAME, 16, "", "String",
      [](String username) {
        mqttController.setUsername(username);
  });
  createStoredData("mqtt_Pswd", MQTT_USER_PASS, 16, "", "String",
      [](String password) {
        mqttController.setPassword(password);
  });
  createStoredData("mqtt_deviceId", MQTT_DEVICE_ID, 8, deviceToken, "String",
      [](String deviceid) {
        mqttController.setDeviceId(deviceid);
  });
}

void IoTController::configRedController() {
  redController.init(getStoredValue(DEVICE_NAME_COMMAND_NAME),
                     getStoredValue(DEVICE_PASSWORD_COMMAND_NAME),
                     getStoredValue(RED_NAME_COMMAND_NAME),
                     getStoredValue(RED_PASSWORD_COMMAND_NAME));
}

void IoTController::configMqttController() {
  mqttController.init(getStoredValue(MQTT_SERVER_NAME),
                      deviceType,
                      getStoredValue(MQTT_DEVICE_ID),
                      getStoredValue(MQTT_USER_NAME),
                      getStoredValue(MQTT_USER_PASS));
}

boolean IoTController::sendMqttMessage(String message) {
  if (redController.isConnectedToAccessPoint()) {
    Logger::log("json Payload: " + message, Logger::INFO_LOG);
    return mqttController.send_mqtt_notification(message);
  }
  return false;
}

void IoTController::checkReceivedMqttData() {
  if (redController.wasConnectedToAccessPoint && mqttController.getServerName()!="") {
    mqttController.mqttLoop();
    mqttController.send_mqtt_available();
  }
}

String IoTController::getRequests(String url){
  return redController.getRequests(url);
}

void IoTController::setTimers() {
  //restarDeviceCheckerTimer
  Timer::createTimer(1000, restartDeviceDueCommand);
  //connectionTimer
  Timer::createTimer(5000, []() {
    redController.handleConnection();
  });
  //mqttPayloadCheckerTimer
  Timer::createTimer(500, checkReceivedMqttData);
}

void IoTController::addTimer(int intervalInMillis, void (actionToExecute)()) {
  Timer::createTimer(intervalInMillis, actionToExecute);
}

void IoTController::createStoredData(String name, String commandName,
                                      int valueLenght, String defaultValue,
                                      String valueType,
                                      void (setFunction)(String value)) {
  DataController::createCommonData(name, commandName, valueLenght, defaultValue,
                                  valueType, setFunction);
}

boolean IoTController::changeStoredValue(String commandName, String value){
  DataController::setValueByCommandName(commandName, value, "0");
}

String IoTController::getStoredValue(String commandName){
  return DataController::getValueByCommandName(commandName);
}

void IoTController::print(String message) {
  Logger::log(message, Logger::PRINT_LOG);
}

void IoTController::setConsoleLevel(uint8_t consoleLevel) {
  defaultConsoleLevel = consoleLevel;
}

void IoTController::setup(String pDeviceType, uint8_t consoleLevel, String pDeviceToken) {
  if (consoleLevel >= 0)setConsoleLevel(consoleLevel);
  deviceType = pDeviceType;
  deviceUniqueName = String(deviceType) + "-" + pDeviceToken;
  deviceToken = pDeviceToken;
  setupLogger();
  fillCommonData();
  setTimers();
}

void IoTController::init() {
  DataController::getDataFromEeprom();
  DataController::setSendValueUpdate(sendMqttMessage);
  configRedController();
  configMqttController();
}

void IoTController::loop() {
  redController.handleClient();
  Timer::executeLoops();
}
