#ifndef DataController_h
#define DataController_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include "CommonData.h"

class DataController {
  private:
    static boolean sendValueUpdateSeted;
  public:
    static void getDataFromEeprom();
    static void createCommonData(String name, String commandName,
                                uint8_t valueLenght, String defaultValue,
                                String valueType, void (setFunction)(String value));
    static boolean isValidCommandName(String commandName);
    static String getValueByCommandName(String commandName);
    static boolean setValueByCommandName(String commandName, String value,
                                         String cid = "UNKNOWN");
    static void setSendValueUpdate(boolean (setFunction)(String message));
    static boolean (*sendValueUpdate)(String message);
};
#endif
