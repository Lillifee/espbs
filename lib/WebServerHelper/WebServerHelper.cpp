#include "WebServerHelper.h"

WebServerHelperClass::WebServerHelperClass() : server(80){};

void WebServerHelperClass::start() {
  Serial.println("WebServer start");

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/bundle.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/bundle.js");
  });

  server.on("/api/restart", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "message received");
    ESP.restart();
  });

  server.on("/api/sleep", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "message received");

    if (sleep) {
      sleep();
    }
  });

  server.on("/api/esp", HTTP_GET, [this](AsyncWebServerRequest *request) {
    AsyncJsonResponse *response = new AsyncJsonResponse();
    response->addHeader("Server", "ESP Async Web Server");

    JsonVariant &root = response->getRoot();
    root["heap"] = ESP.getFreeHeap();
    root["cupFreq"] = ESP.getCpuFreqMHz();
    root["sketchSize"] = ESP.getSketchSize();
    root["sketchSizeFree"] = ESP.getFreeSketchSpace();

    response->setLength();
    request->send(response);
  });

  server.on(
      "/api/update", HTTP_POST, [this](AsyncWebServerRequest *request) {
            request->send_P(200, "text/plain", "message received");
            ESP.restart(); },
      [this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        uploadFile(U_FLASH, request, filename, index, data, len, final);
      });

  server.on(
      "/api/updateFS", HTTP_POST, [this](AsyncWebServerRequest *request) {
            request->send_P(200, "text/plain", "message received");
            ESP.restart(); },
      [this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        uploadFile(U_SPIFFS, request, filename, index, data, len, final);
      });

  server.begin();
  Serial.println("WebServer running");
}

void WebServerHelperClass::uploadFile(int command, AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index) {
    Serial.printf("UploadStart: %s\n", filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, command)) {
      Update.printError(Serial);
    }
  }

  if (!Update.hasError()) {
    if (Update.write(data, len) != len) {
      Update.printError(Serial);
    }
  }

  if (final) {
    if (Update.end(true)) {
      Serial.printf("Update Success: %uB\n", index + len);
    } else {
      Update.printError(Serial);
    }
  }
}

void WebServerHelperClass::onSleep(SleepFunc sleepFn) {
  sleep = sleepFn;
}

WebServerHelperClass WebServerHelper;
