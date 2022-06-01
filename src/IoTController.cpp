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
    CommonData::setValueByCommandName(RESTART_DEVICE_COMMAND_NAME, "0");
    ESP.restart();
  }
}

void IoTController::fillCommonData() {
  Logger::log("Filling Common data...", Logger::INFO_LOG);
  CommonData::createCommonData("restart_device", RESTART_DEVICE_COMMAND_NAME, 2, "0", "String", [](String restartDevice) {
    setDeviceToRestart(restartDevice);
  });
  //Net Values
  CommonData::createCommonData("red_deviceSsid", DEVICE_NAME_COMMAND_NAME, 32, deviceUniqueName, "String", [](String deviceSsid) {
    redController.setDeviceSSID(deviceSsid);
  });
  CommonData::createCommonData("red_devicePswd", DEVICE_PASSWORD_COMMAND_NAME, 32, DEVICE_PASSWORD, "String", [](String devicePassword) {
    redController.setDevicePassword(devicePassword);
  });
  CommonData::createCommonData("red_ssid", RED_NAME_COMMAND_NAME, 32, "", "String", [](String redSsid) {
    redController.setSsid(redSsid);
  });
  CommonData::createCommonData("red_pswd", RED_PASSWORD_COMMAND_NAME, 32, "", "String", [](String redPassword) {
    redController.setPassword(redPassword);
  });
  //MQTT Values
  CommonData::createCommonData("mqtt_Server", MQTT_SERVER_NAME, 32, "", "String", [](String ServerName) {
    mqttController.setServerName(ServerName);
  });
  CommonData::createCommonData("mqtt_User", MQTT_USER_NAME, 16, "", "String", [](String username) {
    mqttController.setUsername(username);
  });
  CommonData::createCommonData("mqtt_Pswd", MQTT_USER_PASS, 16, "", "String", [](String password) {
    mqttController.setPassword(password);
  });
  CommonData::createCommonData("mqtt_deviceId", MQTT_DEVICE_ID, 8, deviceToken, "String", [](String deviceid) {
    mqttController.setDeviceId(deviceid);
  });
}

void IoTController::configRedController() {
  redController.init(CommonData::getValueFromCommonDataByCommandName(DEVICE_NAME_COMMAND_NAME),
                     CommonData::getValueFromCommonDataByCommandName(DEVICE_PASSWORD_COMMAND_NAME),
                     CommonData::getValueFromCommonDataByCommandName(RED_NAME_COMMAND_NAME),
                     CommonData::getValueFromCommonDataByCommandName(RED_PASSWORD_COMMAND_NAME));
  redController.setIsValidArgument(CommonData::checkIfCommandNameExist);
  redController.setSetValues(CommonData::setValueByCommandName);
  redController.setGetValue(CommonData::getValueFromCommonDataByCommandName);
}

void IoTController::configMqttController() {
  mqttController.init(CommonData::getValueFromCommonDataByCommandName(MQTT_SERVER_NAME),
                      deviceType,
                      CommonData::getValueFromCommonDataByCommandName(MQTT_DEVICE_ID),
                      CommonData::getValueFromCommonDataByCommandName(MQTT_USER_NAME),
                      CommonData::getValueFromCommonDataByCommandName(MQTT_USER_PASS));
  mqttController.setSetValues(CommonData::setValueByCommandName);
  mqttController.setGetValue(CommonData::getValueFromCommonDataByCommandName);
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

void IoTController::addCommonData(String name, String commandName, int valueLenght, String defaultValue, String valueType, void (setFunction)(String value)) {
  CommonData::createCommonData(name, commandName, valueLenght, defaultValue, valueType, setFunction);
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
  CommonData::getDataFromEeprom();
  configRedController();
  configMqttController();
}

void IoTController::loop() {
  redController.handleClient();
  Timer::executeLoops();
}
