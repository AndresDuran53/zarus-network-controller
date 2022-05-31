#include "RedController.h"


AsyncWebServer RedController::server(80);
DNSServer RedController::dnsServer;

RedController::RedController() {

}

void RedController::init(String deviceSSID, String devicePassword, String redSSID, String redPassword) {
  this->deviceSSID = deviceSSID;
  this->devicePassword = devicePassword;
  this->ssid = redSSID;
  this->password = redPassword;
  this->lastConnectionAttempt = 0;
  this->connectionAttemptInterval = 10000;
  this->checkConnectionInterval = 1000;
  this->wasConnectedToAccessPoint = false;
  this->hasToResetWifi = false;
  updateIfShouldConnectToAccessPoint();
  mdnsName = deviceSSID;
  setup_wifi();
}

void RedController::setup_wifi() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.hostname(deviceSSID);
  WiFi.softAP(deviceSSID.c_str(), devicePassword.c_str());
  myIP = WiFi.softAPIP();
  //dnsServer.start(53, "*", myIP);
  configServer();
}

void RedController::updateIfShouldConnectToAccessPoint() {
  this->shouldConnectToAccessPoint = ssid != "";
}

void RedController::connectToAccessPoint() {
  WiFi.begin(ssid.c_str(), password.c_str());  //Nos conectamos a un AccessPoint con acceso a Internet
}

boolean RedController::isConnectedToAccessPoint() {
  boolean ipSeted = (WiFi.localIP().toString()!="(IP unset)" && WiFi.localIP().toString() != "0.0.0.0");;
  boolean actualStatus = (WiFi.status() == WL_CONNECTED || ipSeted);
  if (actualStatus && !wasConnectedToAccessPoint) {
    Logger::log("Connected to : " + ssid + " | IP: "+ getIp(), Logger::DEBUG_LOG);
    if (MDNS.begin(deviceSSID.c_str())) {
      Logger::log("MDNS started at URL: " + deviceSSID + ".local", Logger::DEBUG_LOG);
    }
  }
  wasConnectedToAccessPoint = actualStatus;
  return actualStatus;
}

void RedController::handleConnection() {
  if (shouldConnectToAccessPoint && !isConnectedToAccessPoint()) {
    connectToAccessPoint();
    Logger::log("Trying to Connect to: " + ssid, Logger::DEBUG_LOG);
  }
}

void RedController::configServer() {
  server.on("/generate_204", HTTP_GET,
        [this](AsyncWebServerRequest *request){request->send(LittleFS, "/redSettings.html");}
  );
  //Iphone
  server.on("/hotspot-detect.html", HTTP_GET,
        [this](AsyncWebServerRequest *request){request->send(LittleFS, "/redSettings.html");}
  );
  //Microsoft
  server.on("/fwlink", HTTP_GET,
        [this](AsyncWebServerRequest *request){request->send(LittleFS, "/redSettings.html");}
  );
  server.on("/getRedInformation", HTTP_GET,
        [this](AsyncWebServerRequest *request){RedController::handleGetRedInformation(request);}
  );
  server.on("/setValues", HTTP_GET,
        [this](AsyncWebServerRequest *request){RedController::handleSetValues(request);}
  );
  server.on("/setValues", HTTP_POST,
        [this](AsyncWebServerRequest *request){RedController::handleSetValues(request);}
  );
  server.onNotFound(
        [this](AsyncWebServerRequest *request){RedController::handleNotFound(request);}
  );
  if (!LittleFS.begin())
  {
    Logger::log("An Error has occurred while mounting LittleFS", Logger::DEBUG_LOG);
  } else {
    Logger::log("LittleFS Mount succesfull", Logger::DEBUG_LOG);
    server.serveStatic("/", LittleFS, "/index.html");
  }
  server.begin();
}

void RedController::handleNotFound(AsyncWebServerRequest *request) {
  if (!handleFileRead(request)) {
    request->send(LittleFS, "/redSettings.html");
  }
}

String RedController::getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css") || filename.endsWith(".css.gz")) return "text/css";
  else if (filename.endsWith(".js") || filename.endsWith(".js.gz")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".svg")) return "image/svg+xml";
  return "text/plain";
}

bool RedController::handleFileRead(AsyncWebServerRequest *request) {
  String path = request->url();
  Logger::log("handleFileRead: " + path, Logger::DEBUG_LOG);
  if (path.endsWith("/")) path = "/index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (LittleFS.exists(path)) {                          // If the file exists
    //File file = LittleFS.open(path, "r");               // Open it
    //size_t sent = server.streamFile(file, contentType); // And send it to the client
    //file.close();                                       // Then close the file again
    request->send(LittleFS, path);
    return true;
  }
  else{
    String pathWithGz = path + ".gz";
    if(LittleFS.exists(pathWithGz))
    {
      Logger::log("contentType: " + contentType, Logger::DEBUG_LOG);
      AsyncWebServerResponse* response = request->beginResponse(LittleFS, pathWithGz, contentType);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
      return true;
    }
  }
  Logger::log("\tFile Not Found", Logger::DEBUG_LOG);
  return false;                                         // If the file doesn't exist, return false
}

void RedController::handleGetRedInformation(AsyncWebServerRequest *request) {
  String jsonResponse = "{\"deviceSSID\":\"" + deviceSSID + "\",\"ssid\":\"" + ssid + "\"}";
  request->send(200, "text/json", jsonResponse);
}

void RedController::handleSetValues(AsyncWebServerRequest *request) {
  String valuesChanged = "";
  for (uint8_t i = 0; i < request->params(); i++) {
    AsyncWebParameter* parameterAux = request->getParam(i);
    String argumentName = parameterAux->name();
    String argumentValue = parameterAux->value();
    if(valuesChanged!="") valuesChanged += ",";
    if ((*RedController::isValidArgument)(argumentName)) {
      boolean wasExecuted = (*RedController::setValues)(argumentName, argumentValue);
      if (wasExecuted) {
        valuesChanged += "{\"value\":\""+argumentName+"\",\"changed\":\"true\"}";
      }
      else{
        valuesChanged += "{\"value\":\""+argumentName+"\",\"changed\":\"false\"}";
      }
    }
    else {
      valuesChanged += "{\"value\":\""+argumentName+"\",\"changed\":\"false\"}";
    }
  }
  String jsonResponse = "{\"values\":["+valuesChanged+"]}";
  request->send(200, "text/json", jsonResponse);
  if (hasToResetWifi) {
    Logger::log("Has To Reset the Wifi to update values", Logger::INFO_LOG);
    hasToResetWifi = false;
    if((*RedController::isValidArgument)("rdccm")){
      boolean wasExecuted = (*RedController::setValues)("rdccm", "1");
    }
  }
}

#ifdef ESP32

void RedController::handleClient() {
  //dnsServer.processNextRequest();
}

#else

void RedController::handleClient() {
  //server.handleClient();
  //dnsServer.processNextRequest();
  if (wasConnectedToAccessPoint) {
    MDNS.update();
  }
}
#endif

void RedController::update_timeNTP(int dateTime[]) {
  NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC_OFFSET_IN_SECONDS);
  timeClient.begin();
  timeClient.update();
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  dateTime[0] = ti->tm_year + 1900;
  dateTime[1] = ti->tm_mon + 1;
  dateTime[2] = ti->tm_mday;
  dateTime[3] = ti->tm_hour;
  dateTime[4] = ti->tm_min;
  dateTime[5] = ti->tm_sec;
}

void RedController::setShouldConnectToAccessPoint(boolean shouldConnectToAP) {
  this->shouldConnectToAccessPoint = shouldConnectToAccessPoint;
}

void RedController::setSsid(String ssid) {
  this->ssid = ssid;
  this->hasToResetWifi = true;
  updateIfShouldConnectToAccessPoint();
}

void RedController::setPassword(String password) {
  this->password = password;
  this->hasToResetWifi = true;
}

void RedController::setDeviceSSID(String deviceSsid) {
  Logger::log("Changing Device Name: " + deviceSsid, Logger::DEBUG_LOG);
  this->deviceSSID = deviceSsid;
}

void RedController::setDevicePassword(String devicePassword) {
  this->devicePassword = devicePassword;
}

void RedController::setDeviceCredentials(String deviceSsid, String devicePassword) {
  boolean anyChange = false;
  if (deviceSsid != "") {
    setDeviceSSID(deviceSsid);
    anyChange = true;
  }
  if (devicePassword != "") {
    setDevicePassword(devicePassword);
    anyChange = true;
  }
  //if (anyChange) ESP.restart();
}

void RedController::setIsValidArgument(boolean (setFunction)(String commandName)) {
  this->isValidArgument = setFunction;
}

void RedController::setSetValues(boolean (setFunction)(String argumentName, String argumentValue)) {
  this->setValues = setFunction;
}

void RedController::setGetValue(String (setFuntion)(String commandName)) {
  this->getValue = setFuntion;
}

boolean RedController::getShouldConnectToAccessPoint() {
  return shouldConnectToAccessPoint;
}

String RedController::getSsid() {
  return ssid;
}

String RedController::getDeviceSSID() {
  return deviceSSID;
}

String RedController::getPassword() {
  return password;
}

String RedController::getDevicePassword() {
  return devicePassword;
}

String RedController::getIp() {
  return WiFi.localIP().toString();
}

String RedController::getRequests(String url){
  /*String result = "";
  if(isConnectedToAccessPoint()){
    WiFiClientSecure client;
    client.connect(url, 443);

    if (client.verify(fingerprint, url.c_str()))
    {
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
             "Host: " + url + "\r\n" +
             "Connection: close\r\n\r\n");
    }
    else
    {
      Serial.println("certificate doesn't match");
    }
  }*/
  return "";
}
