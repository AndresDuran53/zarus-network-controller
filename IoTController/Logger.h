#ifndef Logger_h
#define Logger_h

#include <Arduino.h>

class Logger {
  private:
  public:
    const static uint8_t NONE_LOG = 0;
    const static uint8_t PRINT_LOG = 1;
    const static uint8_t INFO_LOG = 2;
    const static uint8_t DEBUG_LOG = 3;
    static uint8_t systemLogLevel;

    static String generateDebugLevelTag(uint8_t levelTag);
    static void log(String message, uint8_t messageLevel);

};
#endif
