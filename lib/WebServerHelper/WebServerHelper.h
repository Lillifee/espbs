
#ifndef WebServerHelper_h
#define WebServerHelper_h

#include <ESPAsyncWebServer.h>
#include <Update.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <WiFiHelper.h>

class WebServerHelperClass
{
private:
  void uploadFile(int command, AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
  
public:
  AsyncWebServer asyncServer;
  WebServerHelperClass();
  void start();
  bool isRunning();
};

extern WebServerHelperClass WebServerHelper;

#endif
