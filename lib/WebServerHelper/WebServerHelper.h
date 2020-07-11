
#ifndef WebServerHelper_h
#define WebServerHelper_h

#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "Update.h"
#include "WiFiHelper.h"

class WebServerHelperClass {
 private:
  void uploadFile(int command, AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

 public:
  AsyncWebServer server;
  WebServerHelperClass();
  void start();

  using SleepFunc = void (*)();
  SleepFunc sleep;
  void onSleep(SleepFunc sleep = nullptr);
};

extern WebServerHelperClass WebServerHelper;

#endif
