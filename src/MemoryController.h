#ifndef MemoryController_h
#define MemoryController_h

#include <Arduino.h>
#include <EEPROM.h>
#include "Logger.h"

class MemoryController {
  private:
    static int EepromSize;
    static int indexMemoryChecker;
  public:
    static void init(int EepromSize);
    static String readStringFromEEPROM(int initAddress, int bytesToRead);
    static byte readCharData(int initAddress);
    static void saveCharToEEPROM(int initAddress, char charToSave);
    static void saveStringToEEPROM(int initAddress, String stringToSave);
    static void deleteEEPROMMemory();
    static void verifyMemoryClean();
    static boolean isMemoryClean();
};
#endif
