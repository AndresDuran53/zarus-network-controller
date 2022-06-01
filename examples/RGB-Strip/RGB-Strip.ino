#include "IoTController.h"

#define RED_PIN                 12
#define GREEN_PIN               14
#define BLUE_PIN                13
#define MAX_BRIGHTNESS          1023

//-----Variables Definition-----//
int actualBrightness = MAX_BRIGHTNESS;
String actualHexValue = "#000000";

void configGPIO() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

void startLights() {
  setColorFromRGB(255, 0, 0);
  delay(250);
  setColorFromRGB(0, 255, 0);
  delay(250);
  setColorFromRGB(0, 0, 255);
  delay(250);
  setColorFromRGB(255, 255, 255);
  delay(250);
  setColorFromRGB(0, 0, 0);
}

void setColorFromRGB(int r, int g, int b) {
  Logger::log("Setting RGB Value: " + String(r) + String(g) + String(b), Logger::INFO_LOG);
  r = map(r, 0, 255, 0, actualBrightness);
  g = map(g, 0, 255, 0, actualBrightness);
  b = map(b, 0, 255, 0, actualBrightness);
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}

void setColorFromHex(String hexValue) {
  actualHexValue = hexValue;
  Logger::log("Setting Hex Value: " + hexValue, Logger::DEBUG_LOG);
  int number = (int) strtol( &hexValue[1], NULL, 16);
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;
  setColorFromRGB(r, g, b);
}

void setActualBrightness(String newValue) {
  int newValueMapped = map(newValue.toInt(), 0, 100, 0, MAX_BRIGHTNESS);
  actualBrightness = newValueMapped;
  setColorFromHex(actualHexValue);
}

void setup() {
  IoTController::setup("rgb-light",3);
  IoTController::addCommonData("red_color_value", "rgbh", 9, "#000000", "String", [](String hexValue) {setColorFromHex(hexValue);});
  IoTController::addCommonData("actual_brightness", "lhbr", 9, String(actualBrightness), "int", [](String newValue) {setActualBrightness(newValue);});
  IoTController::init();
  configGPIO();
  startLights();
}

void loop() {
  IoTController::loop();
}
