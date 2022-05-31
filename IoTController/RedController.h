#ifndef RedController_h
#define RedController_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Logger.h"

//#include LittleFS
#include <FS.h>

//Including WebServer libraries
#ifdef ESP32
#include <LITTLEFS.h>
#include <WiFi.h>
//#include <WebServer.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#define LittleFS LITTLEFS
#elif defined(ESP8266)
#include <LittleFS.h>
#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

//Real Time Libraries
#include <NTPClient.h>
#include <WiFiUdp.h>

//#include <WiFiClientSecure.h>


class RedController {
  private:
    static AsyncWebServer server;
    static DNSServer dnsServer;
    boolean (*isValidArgument)(String commandName);
    boolean (*setValues)(String argumentName, String argumentValue);
    String (*getValue)(String commandName);
    const long UTC_OFFSET_IN_SECONDS = -6 * (60 * 60);
    const char* fingerprint = "F2 AD 29 9C 34 48 DD 8D F4 CF 52 32 F6 57 33 68 2E 81 C1 90";
    String ssid;
    String password;
    String deviceSSID;
    String devicePassword;
    boolean shouldConnectToAccessPoint;
    boolean hasToResetWifi;
    String mdnsName;
    MDNSResponder mdns;
    WiFiUDP ntpUDP;
    IPAddress myIP;
    int connectionAttemptInterval;
    unsigned long lastConnectionAttempt = 0;
    int checkConnectionInterval;
    unsigned long lastCheckConnection = 0;
    String* allCommandNames;
    void configServer();
    void handleNotFound(AsyncWebServerRequest *request);
    String getContentType(String filename);
    bool handleFileRead(AsyncWebServerRequest *request);
    void handleGetRedInformation(AsyncWebServerRequest *request);
    void handleSetValues(AsyncWebServerRequest *request);
  public:
    boolean wasConnectedToAccessPoint;
    RedController();
    void init(String deviceSSID, String devicePassword, String redSSID, String redPassword);
    void setup_wifi();
    void updateIfShouldConnectToAccessPoint();
    void connectToAccessPoint();
    boolean isConnectedToAccessPoint();
    void update_timeNTP(int dateTime[]);
    void handleConnection();
    void handleClient();
    void setValue(String value);
    void setSsid(String ssid);
    void setPassword(String password);
    void setDeviceSSID(String ssid);
    void setDevicePassword(String password);
    void setDeviceCredentials(String deviceId, String password);
    void setShouldConnectToAccessPoint(boolean shouldConnectToAP);
    void setIsValidArgument(boolean (setFunction)(String commandName));
    void setSetValues(boolean (setFunction)(String argumentName, String argumentValue));
    void setGetValue(String (setFuntion)(String commandName));
    String getSsid();
    String getDeviceSSID();
    boolean getShouldConnectToAccessPoint();
    String getPassword();
    String getDevicePassword();
    String getIp();
    String getRequests(String url);
};
#endif
