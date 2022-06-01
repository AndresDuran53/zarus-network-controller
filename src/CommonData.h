#ifndef CommonData_h
#define CommonData_h

#include <Arduino.h>
#include "Logger.h"
#include "MemoryController.h"

class CommonData {
  private:
    uint8_t index;
    String name;
    String commandName;
    String value;
    uint8_t valueLenght;
    int memoryCellIndex;
    String defaultValue;
    String valueType;

  public:
    static CommonData* commonDataInstances[200];
    static uint8_t indexCounter;
    static int nextMemoryCellAvailable;
    static void createCommonData(String name, String commandName, uint8_t valueLenght,
                                 String defaultValue, String valueType, void (setFunction)(String value));
    static String getValueFromCommonDataByCommandName(String commandName);
    static int getIndexFromCommonDataByCommandName(String commandName);
    static boolean setValueByCommandName(String commandName, String value);
    static void getDataFromEeprom();
    static boolean checkIfCommandNameExist(String commandName);
    CommonData();
    CommonData(String name, String commandName, uint8_t valueLenght,
               String defaultValue, String valueType, void (setFunction)(String value));
    void (*setFunction)(String value);
    void setValue(String value);
    int getIndex();
    String getName();
    String getCommandName();
    String getValue();
    int getValueLenght();
    int getMemoryCellIndex();
    String getDefaultValue();
    String getValueType();

};
#endif
