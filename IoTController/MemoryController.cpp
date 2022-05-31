#include <Arduino.h>
#include "MemoryController.h"
#include <EEPROM.h>

int MemoryController::EepromSize = 1024;
int MemoryController::indexMemoryChecker = 1023;

void MemoryController::init(int EepromSize) {
  EepromSize = EepromSize;
  indexMemoryChecker = EepromSize - 1;
}

String MemoryController::readStringFromEEPROM(int initAddress, int bytesToRead) {
  EEPROM.begin(EepromSize);
  byte value;
  String result = "";
  for (int i = 0; i < bytesToRead; ++i)
  {
    delay(10);
    result += char(EEPROM.read(initAddress + i));
    delay(10);
    if (char(EEPROM.read(initAddress + 1 + i)) == '\0') {
      i = bytesToRead;
    }
  }

  EEPROM.end();
  return result;
}

//Funcion que retorna un Char de la EEPROM
byte MemoryController::readCharData(int initAddress) {
  EEPROM.begin(EepromSize);
  byte value;
  value = EEPROM.read(initAddress);
  EEPROM.end();
  return value;
}

//Funcion que guarda un Char en la EEPROM
void MemoryController::saveCharToEEPROM(int initAddress, char charToSave) {
  EEPROM.begin(EepromSize);
  EEPROM.write(initAddress, charToSave);
  EEPROM.commit();
  delay(10);
  EEPROM.end();
}

//Funcion que guarda un String en la EEPROM
void MemoryController::saveStringToEEPROM(int initAddress, String stringToSave) {
  EEPROM.begin(EepromSize);
  int stringLenght = stringToSave.length();
  char this_char[stringLenght + 1];
  stringToSave.toCharArray(this_char, (stringLenght + 1));

  for (int i = 0; i < stringLenght; i++) {
    EEPROM.write(initAddress + i, stringToSave[i]);
    delay(10);
  }
  EEPROM.write(initAddress + stringLenght, '\0');
  EEPROM.commit();
  delay(10);
  EEPROM.end();
}

//Funcion que borra la totalidad de la memoria EEPROM
void MemoryController::deleteEEPROMMemory() {
  EEPROM.begin(EepromSize);
  for (int i = 0; i < EepromSize; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  delay(10);
  EEPROM.end();
}

boolean MemoryController::isMemoryClean() {
  String value = readStringFromEEPROM(indexMemoryChecker, 1);
  if (value.toInt() == 5) {
    return true;
  }
  return false;
}

void MemoryController::verifyMemoryClean() {
  //deleteEEPROMMemory(); //Uncomment to always delete all EEPROM
  Logger::log("Verifying if EEPROM is clean...", Logger::DEBUG_LOG);
  if (!isMemoryClean()) {
    Logger::log("Cleaning Memory", Logger::INFO_LOG);
    deleteEEPROMMemory();
    saveStringToEEPROM(indexMemoryChecker, "5");
  }
  Logger::log("EEPROM is clean", Logger::DEBUG_LOG);
}
