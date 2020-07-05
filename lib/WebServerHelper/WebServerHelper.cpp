#include "WebServerHelper.h"
#include "WiFiHelper.h"

WebServerHelperClass::WebServerHelperClass() : asyncServer(80){};

boolean running;

void WebServerHelperClass::start()
{
  if (WiFiHelper.connectWifi())
  {
    WiFiHelper.setupMDNS();
  }
  else
  {
    WiFiHelper.setupAP();
  }

  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  asyncServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  asyncServer.on("/bundle.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/bundle.js");
  });

  asyncServer.on("/api/restart", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "message received");
    ESP.restart();
  });

  asyncServer.on("/api/sleep", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "message received");
    
    // TODO extract to helper or callback (mpu missing) 
    WiFiHelper.disconnectWifi();
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1);
    esp_deep_sleep_start();
  });

  asyncServer.on("/api/esp", HTTP_GET, [this](AsyncWebServerRequest *request) {
    AsyncJsonResponse *response = new AsyncJsonResponse();
    response->addHeader("Server", "ESP Async Web Server");

    JsonVariant &root = response->getRoot();
    root["heap"] = ESP.getFreeHeap();
    root["cupFreq"] = ESP.getCpuFreqMHz();
    root["chipRevision"] = ESP.getChipRevision();
    root["sketchSize"] = ESP.getSketchSize();

    response->setLength();
    request->send(response);
  });

  asyncServer.on("/api/wifi", HTTP_GET, [this](AsyncWebServerRequest *request) {
    int args = request->args();

    if (args > 0)
    {
      request->send(200, "text/plain", "message received");

      Serial.println("Update wifi settings");
      if (request->hasArg("host"))
        WiFiHelper.host = request->arg("host");
      if (request->hasArg("ssid"))
        WiFiHelper.ssid = request->arg("ssid");
      if (request->hasArg("password"))
        WiFiHelper.password = request->arg("password");

      WiFiHelper.write();
      WiFi.disconnect();
      ESP.restart();
    }
    else
    {
      AsyncJsonResponse *response = new AsyncJsonResponse();
      response->addHeader("Server", "ESP Async Web Server");

      JsonVariant &root = response->getRoot();
      root["host"] = WiFiHelper.host;
      root["ssid"] = WiFiHelper.ssid;
      root["password"] = WiFiHelper.password;

      root["rssi"] = String(WiFi.RSSI());
      root["time"] = String(WiFiHelper.getWifiTime());

      response->setLength();
      request->send(response);
    }
  });

  asyncServer.on("/api/network", HTTP_GET, [this](AsyncWebServerRequest *request) {
    int args = request->args();

    if (args > 0)
    {
      request->send(200, "text/plain", "message received");

      Serial.println("Update network settings");
      if (request->hasArg("mode"))
        WiFiHelper.mode = request->arg("mode");
      if (request->hasArg("ipv4"))
        WiFiHelper.ipv4 = request->arg("ipv4");
      if (request->hasArg("dns"))
        WiFiHelper.dns = request->arg("dns");
      if (request->hasArg("subnet"))
        WiFiHelper.subnet = request->arg("subnet");
      if (request->hasArg("gateway"))
        WiFiHelper.gateway = request->arg("gateway");

      WiFiHelper.write();
      WiFi.disconnect();
      ESP.restart();
    }
    else
    {
      AsyncJsonResponse *response = new AsyncJsonResponse();
      response->addHeader("Server", "ESP Async Web Server");

      JsonVariant &root = response->getRoot();
      root["mode"] = WiFiHelper.mode;
      root["ipv4"] = WiFi.localIP().toString();
      root["dns"] = WiFi.dnsIP().toString();
      root["subnet"] = WiFi.subnetMask().toString();
      root["gateway"] = WiFi.gatewayIP().toString();

      response->setLength();
      request->send(response);
    }
  });

  asyncServer.on(
      "/api/update", HTTP_POST, [this](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", "message received");
        Serial.println("Upload file");
        delay(100); // delay to update the website
        ESP.restart(); },
      [this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        uploadFile(U_FLASH, request, filename, index, data, len, final);
      });

  asyncServer.on(
      "/api/updateFS", HTTP_POST, [this](AsyncWebServerRequest *request) {
        Serial.println("Upload FS file");
        request->send_P(200, "text/plain", "message received");
        delay(100); // delay to update the website
        ESP.restart(); },
      [this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        uploadFile(U_SPIFFS, request, filename, index, data, len, final);
      });


  asyncServer.begin();
  running = true;

}

void WebServerHelperClass::uploadFile(int command, AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  if (!index)
  {

    Serial.printf("UploadStart: %s\n", filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, command))
      Update.printError(Serial);
  }

  if (!Update.hasError())
  {
    if (Update.write(data, len) != len)
      Update.printError(Serial);
  }

  if (final)
  {
    if (Update.end(true))
      Serial.printf("Update Success: %uB\n", index + len);
    else
      Update.printError(Serial);
  }
}

boolean WebServerHelperClass::isRunning()
{
  return running;
}


WebServerHelperClass WebServerHelper;
