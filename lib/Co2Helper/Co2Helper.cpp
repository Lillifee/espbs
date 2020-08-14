#include "Co2Helper.h"

Co2HelperClass::Co2HelperClass() : hwSerial(2){};

void Co2HelperClass::read() {
  preferences.begin("co2", true);
  host = preferences.getString("host");
  port = preferences.getInt("port", 3333);
  autoCalibration = preferences.getBool("autoCalib", false);
  requestInterval = preferences.getInt("reqInterval", 60000);
  preferences.end();
}

void Co2HelperClass::write() {
  preferences.begin("co2", false);
  preferences.putString("host", host);
  preferences.putInt("port", port);
  preferences.putBool("autoCalib", autoCalibration);
  preferences.putInt("reqInterval", requestInterval);
  preferences.end();
}

void Co2HelperClass::setup() {
  unsigned long setupStartTime = millis();
  Serial.println("Setup CO2 helper");

  read();

  hwSerial.begin(9600);

  mhz19.begin(hwSerial);
  mhz19.autoCalibration(autoCalibration);

  udp.begin(port);

  setupDuration = millis() - setupStartTime;
  Serial.print("Setup CO2 helper took ");
  Serial.println(setupDuration);
}

void Co2HelperClass::loop() {
  if (millis() - readTimer >= 10000) {
    readValues();
    readTimer = millis();
  }
  if (millis() - requestTimer >= requestInterval) {
    sendValues();
    requestTimer = millis();
  }
}

void Co2HelperClass::sleep() {
}

void Co2HelperClass::readValues() {
  int readCo2 = mhz19.getCO2();
  float readTemp = mhz19.getTemperature(true);

  if (mhz19.errorCode == RESULT_OK && readCo2 != 0) {
    co2 = readCo2;
    temp = readTemp;

    Serial.print("CO2 (ppm): ");
    Serial.print(co2);

    Serial.print(" Temperature (C): ");
    Serial.println(temp);
  } else {
    Serial.print("Failed to read co2 sensor ");
    Serial.println(String(mhz19.errorCode));
  }
}

void Co2HelperClass::sendValues() {
  if (host.length() == 0) return;
  if (!WiFiHelper.connect()) return;
  unsigned long requestStartTime = millis();

  StaticJsonDocument<45> doc;

  udp.beginPacket(host.c_str(), port);

  doc["co2"] = co2;
  doc["temp"] = temp;
  serializeJson(doc, udp);

  udp.println();
  udp.endPacket();

  requestDuration = millis() - requestStartTime;
  Serial.print("Send UDP request ");
  Serial.println(requestDuration);
}

void Co2HelperClass::server() {
  Serial.println("Setup CO2 server");

  WebServerHelper.server.on("/api/co2", HTTP_GET, [this](AsyncWebServerRequest *request) {
    int args = request->args();

    if (args > 0) {
      request->send(200, "text/plain", "message received");
      Serial.println("Update CO2 settings");

      if (request->hasArg("host")) host = request->arg("host");
      if (request->hasArg("port")) port = request->arg("port").toInt();
      if (request->hasArg("autoCalibration")) autoCalibration = request->arg("autoCalibration") == "1";
      if (request->hasArg("requestInterval")) requestInterval = request->arg("requestInterval").toInt();

      write();

    } else {
      AsyncJsonResponse *response = new AsyncJsonResponse();
      response->addHeader("Server", "ESP Async Web Server");
      JsonVariant &root = response->getRoot();

      root["setupDuration"] = setupDuration;
      root["requestDuration"] = requestDuration;

      root["host"] = host;
      root["port"] = port;

      root["autoCalibration"] = autoCalibration ? 1 : 0;
      root["requestInterval"] = requestInterval;

      root["co2"] = co2;
      root["temp"] = temp;

      response->setLength();
      request->send(response);
    }
  });

  WebServerHelper.server.on("/api/co2/calibrate", HTTP_GET, [this](AsyncWebServerRequest *request) {
    mhz19.setRange();
    mhz19.calibrateZero();
    mhz19.setSpan();

    mhz19.autoCalibration(false);
    request->send(200, "text/plain", "message received");
  });
}

Co2HelperClass Co2Helper;
