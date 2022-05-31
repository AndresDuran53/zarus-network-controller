[![License](https://img.shields.io/badge/license-CC%20BY--NC--SA%204.0-green)](https://creativecommons.org/licenses/by-nc-sa/4.0/deed.es)
![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/AndresDuran53/zarus-network-controller?sort=semver)
![GitHub Tag (latest SemVer)](https://img.shields.io/github/v/tag/AndresDuran53/zarus-network-controller?sort=semver)

<img  width="480" src="https://raw.githubusercontent.com/AndresDuran53/zarus-network-controller/master/iot_banner.png">

# IoT Network Controller for ESP8266 and ESP32
Easy-To-Use library to implement just the code that you need.

Look at the [Zarus IoT Hub](https://github.com/AndresDuran53/zarus-iot-app) to get an easy way to connect to the devices without an IP.

This library works with the ESP8266 and ESP32, and manages the connections of the device. This way the device can create an Access Point (AP mode) and connect to another Access Point (Station Mode) without re-writing again the code for each project.

It's able to manage:
* AccesPoint and Stacion Mode simultaneously.
* MQTT connection.
* Local storage in EEPROM with indexing.
* Loops async without delay().
* Controls methods over a HTTP created with Bootstrap.
* Modifies stored data like credentials over HTTP.
* Adds device-type and an Unique identifier to avoid conflict.
* Configures all this with almost no code and production-friendly

## Quick Start
### Libraries and tools needed
Tools for uploading files into the ESP device:
* ESP8266: [ESP8266fs](https://github.com/esp8266/arduino-esp8266fs-plugin/releases) | ESP32: [ESP32FS](https://github.com/me-no-dev/arduino-esp32fs-plugin/releases)

Libraries:
* [ESPAsyncWebServer v1.2.3](https://github.com/me-no-dev/ESPAsyncWebServer)
* [NTPClient v3.2.0](https://github.com/arduino-libraries/NTPClient)
* [PubSubClient v2.8.0](https://github.com/knolleary/pubsubclient)
* [ArduinoJson v6.18.5](https://arduinojson.org/?utm_source=meta&utm_medium=library.properties)
* ESP8266: [ESP8266LittleFS v2.6.0](https://github.com/esp8266/arduino-esp8266fs-plugin/) | ESP32: [LittleFS v1.0.6](https://github.com/lorol/LITTLEFS)
* ESP8266: [ESPAsyncTCP v1.2.2](https://github.com/me-no-dev/ESPAsyncTCP) | ESP32: [AsyncTCP v1.1.1](https://github.com/me-no-dev/AsyncTCP)
* ESP8266: [ESP8266mDNS](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS) | ESP32: [ESPmDNS](https://github.com/espressif/arduino-esp32)

### Methods

* **Setup:**
   ```C++
    static void setup(String deviceType,
          uint8_t consoleLevel = 0,
          String deviceToken = DEVICE_TOKEN);
    ```
  * Description:
  Setup the init values that we want to give it to the device, set the logger, data and timers
  * Parameters:
    * deviceType (String):
    * consoleLevel (Unsigned Integer 8 Bytes):
    * tokenDevice (String):

* **Init:**
   ```C++
    static void init();
    ```
  * Description:
  Configure the connections with the data that we already passed on the setup.

* **Loop:**
   ```C++
    static void loop();
    ```
  * Description:
  Use an internal loop to run the timers, receive data and check the connections,

* **AddCommonData:**
   ```C++
    static void addCommonData(String name, String commandName,
          int valueLenght, String defaultValue,
          String valueType, void (setFunction)(String value));
    ```
  * Description:
  Add data that we want to manipulate in the EEPROM and assing an internal index to be able to find it, also receive a method that is the one that we want to execute everytime that the values change.
  * Parameters:
    * name (String):
    * commandName (String):
    * valueLenght (Integer):
    * defaultValue (String):
    * valueType (String):
    * setFunction (Function Pointer):

* **AddTimer:**
   ```C++
    static void addTimer(int intervalInMillis, void (actionToExecute)());
    ```
  * Description:
  Add a timer to avoid using delay() in our code, every logic that we want to execute in the normal loop the arduino, we are going to create a method and pass it here with a interval in milliseconds.
  * Parameters:
    * intervalInMillis (Integer):
    * actionToExecute (Lambda):

* **Print:**
   ```C++
    void print(String message);
    ```
  * Description:
  Use it to print messages in the same format with the internal logger.
  * Parameters:
    * message (String):



### Example Blink

```C++
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
```

----

## Implementations
- [x] ESP8266 integration
- [x] Access Point at boot
- [x] Easy connections with others AP
- [x] EEPROM used to stored values
- [x] Allows to create custom EEPROM variables
- [x] Reconnections timers
- [x] Allows to add custom non-blocking timers
- [x] MQTT Integration
- [x] Logger integrated
- [x] Update SPIFF due is deprecated
- [x] Make it compatible with ESP32
- [ ] Allows more types in EEPROM other than String

## License
Zarus IoT Controller is an open source code. All files are licenced under Creative Commons [Reconocimiento-NoComercial-CompartirIgual 4.0 Internacional](https://creativecommons.org/licenses/by-nc-sa/4.0/deed.es)
