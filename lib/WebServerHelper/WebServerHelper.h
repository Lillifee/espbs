
#ifndef WebServerHelper_h
#define WebServerHelper_h

#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "Update.h"

using SleepFunc = void (*)();

class WebServerHelperClass {
 private:
  SleepFunc sleep;

  void uploadFile(int command, AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

 public:
  String app;
  AsyncWebServer server;
  WebServerHelperClass();

  void start(String application);
  void onSleep(SleepFunc sleep = nullptr);

  void pause();
  void resume();
};

extern WebServerHelperClass WebServerHelper;

#endif
