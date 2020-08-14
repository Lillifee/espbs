#include "BmeHelper.h"

void BmeHelperClass::read() {
  preferences.begin("bme", true);
  host = preferences.getString("host");
  port = preferences.getInt("port", 3333);
  requestInterval = preferences.getInt("reqInterval", 60000);
  preferences.end();
}

void BmeHelperClass::write() {
  preferences.begin("bme", false);
  preferences.putString("host", host);
  preferences.putInt("port", port);
  preferences.putInt("reqInterval", requestInterval);
  preferences.end();
}

void BmeHelperClass::setup() {
  unsigned long setupStartTime = millis();
  Serial.println("Setup BME680 helper");

  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    return;
  }

  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);  // 320*C for 150 ms

  read();

  setupDuration = millis() - setupStartTime;
  Serial.print("Setup CO2 helper took ");
  Serial.println(setupDuration);
}

void BmeHelperClass::loop() {
  if (millis() - readTimer >= 10000) {
    readValues();
    readTimer = millis();
  }
  if (millis() - requestTimer >= requestInterval) {
    sendValues();
    requestTimer = millis();
  }
}

void BmeHelperClass::sleep() {
}

void BmeHelperClass::readValues() {
  if (!bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }

  temp = bme.temperature;
  pressure = bme.pressure / 100.0;
  humidity = bme.humidity;
  gas = bme.gas_resistance / 1000.0;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  Serial.print("Temp (C): ");
  Serial.print(temp);

  Serial.print(" Humidity (%): ");
  Serial.print(humidity);

  Serial.print(" Pressure (hPa): ");
  Serial.print(pressure);

  Serial.print(" Altitude (m): ");
  Serial.print(altitude);

  Serial.print(" Gas (KOhms): ");
  Serial.println(gas);
}

void BmeHelperClass::sendValues() {
  if (host.length() == 0) return;
  if (!WiFiHelper.connect()) return;
  unsigned long requestStartTime = millis();

  StaticJsonDocument<128> doc;

  udp.beginPacket(host.c_str(), port);

  doc["temp"] = temp;
  doc["humidity"] = humidity;
  doc["pressure"] = pressure;
  doc["gas"] = gas;
  doc["altitude"] = altitude;

  serializeJson(doc, udp);

  udp.println();
  udp.endPacket();

  requestDuration = millis() - requestStartTime;
  Serial.print("Send UDP request ");
  Serial.println(requestDuration);
}

void BmeHelperClass::server() {
  Serial.println("Setup BME server");

  WebServerHelper.server.on("/api/bme", HTTP_GET, [this](AsyncWebServerRequest *request) {
    int args = request->args();

    if (args > 0) {
      request->send(200, "text/plain", "message received");
      Serial.println("Update BME settings");

      if (request->hasArg("host")) host = request->arg("host");
      if (request->hasArg("port")) port = request->arg("port").toInt();
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

      root["requestInterval"] = requestInterval;

      root["temp"] = temp;
      root["humidity"] = humidity;
      root["pressure"] = pressure;
      root["gas"] = gas;
      root["altitude"] = altitude;

      response->setLength();
      request->send(response);
    }
  });
}

BmeHelperClass BmeHelper;
