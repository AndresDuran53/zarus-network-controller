#include "DataController.h"

boolean DataController::sendValueUpdateSeted = false;
boolean (*DataController::sendValueUpdate)(String message) = NULL;

void DataController::getDataFromEeprom(){
  CommonData::getDataFromEeprom();
}

void DataController::createCommonData(String name, String commandName,
                            uint8_t valueLenght, String defaultValue,
                            String valueType, void (setFunction)(String value)){
  CommonData::createCommonData(name,commandName,valueLenght,defaultValue,
                                valueType, setFunction);
}

boolean DataController::isValidCommandName(String commandName){
  return CommonData::checkIfCommandNameExist(commandName);
}

String DataController::getValueByCommandName(String commandName){
  return CommonData::getValueFromCommonDataByCommandName(commandName);
}

boolean DataController::setValueByCommandName(String commandName, String value, String cid){
  boolean success = CommonData::setValueByCommandName(commandName,value);
  if(success && sendValueUpdateSeted){
    String jsonResponse = "{\"CID\":\""+cid+"\",\""+commandName+"\":\""+value+"\"}";
    (*DataController::sendValueUpdate)(jsonResponse);
  }
  return success;
}

void DataController::setSendValueUpdate(boolean (setFunction)(String message)) {
  sendValueUpdateSeted = true;
  sendValueUpdate = setFunction;
}
