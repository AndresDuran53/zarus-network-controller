#include "CommonData.h"

uint8_t CommonData::indexCounter = 0;
int CommonData::nextMemoryCellAvailable = 0;
CommonData* CommonData::commonDataInstances[200];

CommonData::CommonData() {

}

CommonData::CommonData(String name, String commandName, uint8_t valueLenght,
                       String defaultValue, String valueType, void (setFunction)(String value)) {
  this->index = indexCounter;
  indexCounter++;
  this->name = name;
  this->commandName = commandName;
  this->value = "";
  this->valueLenght = valueLenght;
  this->memoryCellIndex = nextMemoryCellAvailable;
  nextMemoryCellAvailable += valueLenght + 1;
  this->defaultValue = defaultValue;
  this->valueType = valueType;
  this->setFunction = setFunction;
}

void CommonData::createCommonData(String name, String commandName, uint8_t valueLenght,
                                  String defaultValue, String valueType, void (setFunction)(String value)) {
  uint8_t counter = indexCounter;
  Logger::log("New instance of CommonData: " + name, Logger::DEBUG_LOG);
  commonDataInstances[counter] = new CommonData(name, commandName, valueLenght, defaultValue, valueType, setFunction);
}

void CommonData::getDataFromEeprom() {
  Logger::log("Getting EEPROM data...", Logger::INFO_LOG);
  MemoryController::init(1024);
  MemoryController::verifyMemoryClean();

  for (uint8_t i = 0; i < indexCounter; i++) {
    String dataName = commonDataInstances[i]->getName();
    String defaultValue = commonDataInstances[i]->getDefaultValue();
    String savedValue = MemoryController::readStringFromEEPROM(commonDataInstances[i]->getMemoryCellIndex(),
                        commonDataInstances[i]->getValueLenght());
    Logger::log("Saved value for " + dataName + ": " + savedValue, Logger::DEBUG_LOG);

    if (savedValue == "" && defaultValue != "") {
      savedValue = defaultValue;
    }
    commonDataInstances[i]->setValue(savedValue);
  }
}

void CommonData::setValue(String value) {
  this->value = value;
  MemoryController::saveStringToEEPROM(memoryCellIndex, value);
  Logger::log("Setting new " + name, Logger::INFO_LOG);
  (*CommonData::setFunction)(value);
}

String CommonData::getValueFromCommonDataByCommandName(String commandName) {
  for (uint8_t i = 0; i < indexCounter; i++) {
    String auxCommandName = commonDataInstances[i]->getCommandName();
    if (auxCommandName == commandName) {
      String auxValue = commonDataInstances[i]->getValue();
      return auxValue;
    }
  }
  return "";
}

int CommonData::getIndexFromCommonDataByCommandName(String commandName) {
  for (uint8_t i = 0; i < indexCounter; i++) {
    String auxCommandName = commonDataInstances[i]->getCommandName();
    if (auxCommandName == commandName) {
      return i;
    }
  }
  return -1;
}

boolean CommonData::setValueByCommandName(String commandName, String value) {
  int index = getIndexFromCommonDataByCommandName(commandName);
  if (index < 0) {
    return false;
  }
  commonDataInstances[index]->setValue(value);
  return true;
}

boolean CommonData::checkIfCommandNameExist(String commandName) {
  int index = getIndexFromCommonDataByCommandName(commandName);
  if (index < 0) {
    return false;
  }
  return true;
}

int CommonData::getIndex() {
  return this->index;
}

String CommonData::getName() {
  return this->name;
}

String CommonData::getCommandName() {
  return this->commandName;
}

String CommonData::getValue() {
  return this->value;
}

int CommonData::getValueLenght() {
  return this->valueLenght;
}

int CommonData::getMemoryCellIndex() {
  return memoryCellIndex;
}

String CommonData::getDefaultValue() {
  return this->defaultValue;
}

String CommonData::getValueType() {
  return this->valueType;
}
