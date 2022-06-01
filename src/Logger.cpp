#include "Logger.h"

uint8_t Logger::systemLogLevel = Logger::NONE_LOG;

String Logger::generateDebugLevelTag(uint8_t levelTag) {
  String resultTag = "[UNDEFINED] ";
  switch (levelTag) {
    case PRINT_LOG:
      resultTag = "[PRINT] ";
      break;
    case INFO_LOG:
      resultTag = "[INFO_LOG]  ";
      break;
    case DEBUG_LOG:
      resultTag = "[DEBUG_LOG] ";
      break;
    default:
      resultTag = "[UNKNOW]    ";
  }
  return resultTag;
}

void Logger::log(String message, uint8_t messageLevel) {
  String messageLevelTag = Logger::generateDebugLevelTag(messageLevel);
  if (systemLogLevel >= messageLevel) {
    Serial.print(messageLevelTag);
    Serial.print("| ");
    Serial.println(message);
  }
}
