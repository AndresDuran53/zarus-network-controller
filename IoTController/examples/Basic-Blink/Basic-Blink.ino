#include "IoTController.h"

int ledPin = 0;
String deviceType = "blink-device";
String deviceToken = "zzz999";
uint8_t consoleLevel = 3;
boolean toggleStatus = false;

void makeItBlink() {
  digitalWrite(ledPin, toggleStatus);
  toggleStatus = !toggleStatus;
  IoTController::print("Blinking...");
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, false);
  IoTController::addTimer(1000,makeItBlink);
  IoTController::setup(deviceType,consoleLevel,deviceToken);
  IoTController::init();
}

void loop() {
  IoTController::loop();
}
