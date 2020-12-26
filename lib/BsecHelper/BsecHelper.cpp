#include "BsecHelper.h"

const uint8_t bsec_config_iaq[] = {
#include "config/generic_33v_3s_4d/bsec_iaq.txt"
};

void BsecHelperClass::read() {
  preferences.begin("bsec", true);
  host = preferences.getString("host");
  port = preferences.getInt("port", 3333);
  requestInterval = preferences.getInt("reqInterval", 60000);
  hasState = preferences.getBool("hasState", false);
  tempOffset = preferences.getFloat("tempOffset", 13.5f);
  preferences.getBytes("state", bsecState, BSEC_MAX_STATE_BLOB_SIZE);
  preferences.end();
}

void BsecHelperClass::write() {
  preferences.begin("bsec", false);
  preferences.putString("host", host);
  preferences.putInt("port", port);
  preferences.putInt("reqInterval", requestInterval);
  preferences.putFloat("tempOffset", tempOffset);
  preferences.end();
}

void BsecHelperClass::writeState(bool set) {
  iaqSensor.getState(bsecState);
  Serial.println("Write State set: " + set ? "true" : "false");
  for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++) {
    Serial.print(bsecState[i], HEX);
  }
  Serial.println("");

  preferences.begin("bsec", false);
  preferences.putBool("hasState", set);
  preferences.putBytes("state", bsecState, BSEC_MAX_STATE_BLOB_SIZE);
  preferences.end();
}

void BsecHelperClass::checkSensor() {
  if (iaqSensor.status != BSEC_OK) {
    Serial.println("BSEC error code : " + String(iaqSensor.status));
  }
  if (iaqSensor.bme680Status != BME680_OK) {
    Serial.println("BME680 error code : " + String(iaqSensor.bme680Status));
  }
}

void BsecHelperClass::setup(uint8_t i2cAddr) {
  unsigned long setupStartTime = millis();
  Serial.println("Setup BSEC helper");

  Wire.begin();
  iaqSensor.begin(i2cAddr, Wire);
  iaqSensor.setConfig(bsec_config_iaq);

  bsec_virtual_sensor_t sensorList[10] = {
      BSEC_OUTPUT_RAW_TEMPERATURE,
      BSEC_OUTPUT_RAW_PRESSURE,
      BSEC_OUTPUT_RAW_HUMIDITY,
      BSEC_OUTPUT_RAW_GAS,
      BSEC_OUTPUT_IAQ,
      BSEC_OUTPUT_STATIC_IAQ,
      BSEC_OUTPUT_CO2_EQUIVALENT,
      BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };

  iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);

  checkSensor();
  read();

  if (hasState) {
    Serial.println("Setup BSEC SetState");
    iaqSensor.setState(bsecState);
  }

  iaqSensor.setTemperatureOffset(tempOffset);
  Serial.print("Setup BSEC helper temp offset ");
  Serial.println(tempOffset);

  udp.begin(port);

  setupDuration = millis() - setupStartTime;
  Serial.print("Setup BSEC helper took ");
  Serial.println(setupDuration);
}

void BsecHelperClass::loop() {
  readValues();

  if (millis() - requestTimer >= requestInterval) {
    sendValues();
    requestTimer = millis();
  }
}

void BsecHelperClass::sleep() {
}

void BsecHelperClass::readValues() {
  if (iaqSensor.run()) {  // If new data is available
    Serial.print(String(iaqSensor.rawTemperature));
    Serial.print(", " + String(iaqSensor.pressure));
    Serial.print(", " + String(iaqSensor.rawHumidity));
    Serial.print(", " + String(iaqSensor.gasResistance));
    Serial.print(", " + String(iaqSensor.iaq));
    Serial.print(", " + String(iaqSensor.iaqAccuracy));
    Serial.print(", " + String(iaqSensor.temperature));
    Serial.print(", " + String(iaqSensor.humidity));
    Serial.print(", " + String(iaqSensor.staticIaq));
    Serial.print(", " + String(iaqSensor.co2Equivalent));
    Serial.print(", " + String(iaqSensor.breathVocEquivalent));

    Serial.println("");
    updateState();
  } else {
    checkSensor();
  }
}

void BsecHelperClass::updateState() {
  if (stateUpdateCounter == 0) {
    if (iaqSensor.iaqAccuracy >= 3) {
      stateUpdateCounter++;
      writeState(true);
    }
  } else {
    if ((stateUpdateCounter * STATE_SAVE_PERIOD) < millis()) {
      stateUpdateCounter++;
      writeState(true);
    }
  }
}

void BsecHelperClass::sendValues() {
  if (host.length() == 0) return;
  if (!WiFiHelper.connect()) return;
  unsigned long requestStartTime = millis();

  StaticJsonDocument<300> doc;

  udp.beginPacket(host.c_str(), port);

  doc["rtmp"] = iaqSensor.rawTemperature;
  doc["pressure"] = iaqSensor.pressure;
  doc["humidity"] = iaqSensor.rawHumidity;
  doc["gasResistance"] = iaqSensor.gasResistance;
  doc["iaq"] = iaqSensor.iaq;
  doc["iaqAccuracy"] = iaqSensor.iaqAccuracy;
  doc["temperature"] = iaqSensor.temperature;
  doc["humidity"] = iaqSensor.humidity;
  doc["staticIaq"] = iaqSensor.staticIaq;
  doc["co2Equivalent"] = iaqSensor.co2Equivalent;
  doc["breathVocEquivalent"] = iaqSensor.breathVocEquivalent;

  serializeJson(doc, udp);

  udp.println();
  udp.endPacket();

  requestDuration = millis() - requestStartTime;
  Serial.print("Send UDP request ");
  Serial.println(requestDuration);
}

void BsecHelperClass::server() {
  Serial.println("Setup BSEC server");

  WebServerHelper.server.on("/api/bsec", HTTP_GET, [this](AsyncWebServerRequest *request) {
    int args = request->args();

    if (args > 0) {
      request->send(200, "text/plain", "message received");
      Serial.println("Update BSEC settings");

      if (request->hasArg("host")) host = request->arg("host");
      if (request->hasArg("port")) port = request->arg("port").toInt();
      if (request->hasArg("requestInterval")) requestInterval = request->arg("requestInterval").toInt();
      if (request->hasArg("tempOffset")) tempOffset = request->arg("tempOffset").toFloat();

      write();
      ESP.restart();

    } else {
      AsyncJsonResponse *response = new AsyncJsonResponse();
      response->addHeader("Server", "ESP Async Web Server");
      JsonVariant &root = response->getRoot();

      root["setupDuration"] = setupDuration;
      root["requestDuration"] = requestDuration;

      root["host"] = host;
      root["port"] = port;

      root["requestInterval"] = requestInterval;
      root["tempOffset"] = tempOffset;

      root["rtmp"] = iaqSensor.rawTemperature;
      root["pressure"] = iaqSensor.pressure;
      root["rawHumidity"] = iaqSensor.rawHumidity;
      root["gasResistance"] = iaqSensor.gasResistance;
      root["iaq"] = iaqSensor.iaq;
      root["iaqAccuracy"] = iaqSensor.iaqAccuracy;
      root["temperature"] = iaqSensor.temperature;
      root["humidity"] = iaqSensor.humidity;
      root["staticIaq"] = iaqSensor.staticIaq;
      root["co2Equivalent"] = iaqSensor.co2Equivalent;
      root["breathVocEquivalent"] = iaqSensor.breathVocEquivalent;

      response->setLength();
      request->send(response);
    }
  });

  WebServerHelper.server.on("/api/bsecState", HTTP_GET, [this](AsyncWebServerRequest *request) {
    writeState(!request->hasArg("reset"));
    request->send(200, "text/plain", "message received");
    ESP.restart();
  });
}

BsecHelperClass BsecHelper;
