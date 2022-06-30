#include "SplitFlapHelper.h"

SplitFlapHelperClass::SplitFlapHelperClass() {}

void SplitFlapHelperClass::setup() {
  unsigned long setupStartTime = millis();
  Serial.println("Setup SplitFlap helper");

  load();

  pinMode(enablePin, OUTPUT);
  enableUnits(true);

  for (size_t i = 0; i < MAX_UNIT_COUNT; i++) {
    units[i].index = i;
    units[i].setup();
  }

  setupDuration = millis() - setupStartTime;
  Serial.print("Setup SplitFlap helper took ");
  Serial.println(setupDuration);
}

void SplitFlapHelperClass::load() {
  preferences.begin("splitflap", true);
  preferences.end();
}

void SplitFlapHelperClass::save() {
  preferences.begin("splitflap", false);
  preferences.end();
}

/*
 * Display the character for each unit.
 */
void SplitFlapHelperClass::loop() {
  switch (mode) {
    case 0: {
      setText();
      break;
    }
    case 1: {
      calibrate();
      break;
    }
    default: {
      break;
    }
  }

  if (enabled) {
    loopUnits();
  }
}

/*
 * Display the character for each unit.
 */
void SplitFlapHelperClass::setText() {
  // xSemaphoreTake(prefMutex, portMAX_DELAY);
  if (targetText == text) return;

  targetText = text;
  // xSemaphoreGive(prefMutex);

  enableUnits(true);

  // Set the characters for each unit
  for (SplitFlapUnit &unit : units) {
    unit.setChar(targetText.charAt(unit.index));
  }
}

/*
 * Display the character for each unit.
 */
void SplitFlapHelperClass::calibrate() {
  enableUnits(true);

  // Set all units in calibration mode
  for (SplitFlapUnit &unit : units) {
    unit.calibrate();
  }
}

/*
 * Flap each unit once.
 */
void SplitFlapHelperClass::loopUnits() {
  bool enabled = false;

  for (SplitFlapUnit &unit : units) {
    if (unit.running()) {
      unit.loop();
      enabled = true;
    }
  }

  if (!enabled) {
    // WebServerHelper.resume();
    enableUnits(false);
  }
}

/*
 * Set the flapping state and enable/disable all units
 */
void SplitFlapHelperClass::enableUnits(bool state) {
  enabled = state;

  Serial.println("en/dis-able SplitFlap units");
  // Disable webserver during flap
  // state ? WebServerHelper.pause() : WebServerHelper.resume();
  digitalWrite(enablePin, state ? LOW : HIGH);
}

void SplitFlapHelperClass::server() {
  Serial.println("Setup SplitFlap server");

  WebServerHelper.server.on("/api/splitflap", HTTP_GET, [this](AsyncWebServerRequest *request) {
    int args = request->args();

    if (args > 0) {
      request->send(200, "text/plain", "message received");
      if (request->hasArg("mode")) mode = request->arg("mode").toInt();
      if (request->hasArg("text")) text = request->arg("text");
    } else {
      AsyncJsonResponse *response = new AsyncJsonResponse();
      response->addHeader("Server", "ESP Async Web Server");
      JsonVariant &root = response->getRoot();

      root["setupDuration"] = setupDuration;
      root["text"] = text;

      for (size_t i = 0; i < MAX_UNIT_COUNT; i++) {
        root["units"][i]["index"] = units[i].index;
        root["units"][i]["stepPin"] = units[i].stepPin;
        root["units"][i]["hallPin"] = units[i].hallPin;
        root["units"][i]["hallOffset"] = units[i].hallOffset;
      }

      response->setLength();
      request->send(response);
    }
  });

  WebServerHelper.server.on("/api/calibrate", HTTP_GET, [this](AsyncWebServerRequest *request) {
    units[0].calibrate();
  });

  WebServerHelper.server.on(
      "/api/splitflap/unit", HTTP_POST,
      [](AsyncWebServerRequest *request) {},
      NULL,
      [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        // static const int unitCapacity = JSON_OBJECT_SIZE(10);
        DynamicJsonDocument doc(1024);
        DeserializationError err = deserializeJson(doc, (const char *)data);

        if (err) {
          Serial.print(F("deserialize json failed with code "));
          Serial.println(err.f_str());
          request->send(400, "text/plain", "Deserialize failed");
          return;
        }

        if (doc.containsKey("index")) {
          int unitIndex = doc["index"].as<int>();
          units[unitIndex].stepPin = doc["stepPin"].as<int>();
          units[unitIndex].hallPin = doc["hallPin"].as<int>();
          units[unitIndex].hallOffset = doc["hallOffset"].as<int>();
          units[unitIndex].save();
          ESP.restart();
        }

        request->send(200, "text/plain", "end");
      });
}

SplitFlapHelperClass SplitFlapHelper;
