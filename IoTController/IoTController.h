#ifndef IoTController_h
#define IoTController_h

#include <Arduino.h>
#include "Logger.h"
#include "CommonData.h"
#include "RedController.h"
#include "MqttController.h"
#include "Timer.h"

#define DEVICE_TOKEN                      "ABC123"

#define DEVICE_NAME                       "IotDevice"
#define DEVICE_PASSWORD                   "password123"

#define RESTART_DEVICE_COMMAND_NAME       "rdccm"

#define DEVICE_NAME_COMMAND_NAME          "rcdid"
#define DEVICE_PASSWORD_COMMAND_NAME      "rcdpa"
#define RED_NAME_COMMAND_NAME             "rcsid"
#define RED_PASSWORD_COMMAND_NAME         "rcpid"

#define MQTT_SERVER_NAME                  "mcsna"
#define MQTT_USER_NAME                    "mcuna"
#define MQTT_USER_PASS                    "mcupa"
#define MQTT_DEVICE_ID                    "mcdid"

class IoTController {
  private:
    static RedController redController;
    static MqttController mqttController;
    static String deviceHasToRestart;
    static String deviceUniqueName;
    static String deviceToken;

  public:
    static uint8_t defaultConsoleLevel;
    static String deviceType;
    static void setupLogger();
    static void setDeviceToRestart(String restartDevice);
    static void restartDeviceDueCommand();
    static void fillCommonData();
    static void configRedController();
    static void configMqttController();
    static boolean sendMqttMessage(String message);
    static void checkReceivedMqttData();
    static String getRequests(String url);
    static void setTimers();
    static void addTimer(int intervalInMillis, void (actionToExecute)());
    static void addCommonData(String name, String commandName, int valueLenght, String defaultValue,
                              String valueType, void (setFunction)(String value));
    static void setConsoleLevel(uint8_t consoleLevel);
    static void print(String message);
    static void setup(String deviceType, uint8_t consoleLevel = 0, String deviceToken = DEVICE_TOKEN);
    static void init();
    static void loop();

};
#endif
