#include "KnobHelper.h"
AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN);

void KnobHelperClass::read() {
  preferences.begin("knob", true);
  host = preferences.getString("host");
  port = preferences.getInt("port", 3333);
  preferences.end();
}

void KnobHelperClass::write() {
  preferences.begin("knob", false);
  preferences.putString("host", host);
  preferences.putInt("port", port);
  preferences.end();
}

void KnobHelperClass::setup() {
  unsigned long setupStartTime = millis();
  Serial.println("Setup Knob helper");

  read();

  //we must initialize rorary encoder
  rotary.begin();
  rotary.setup([] { rotary.readEncoder_ISR(); });

  rotary.setBoundaries(0, 100, false);
  //optionally we can set boundaries and if values should cycle or not
  // rotary.setBoundaries(0, 10, true);  //minValue, maxValue, cycle values (when max go to min and vice versa)

  setupDuration = millis() - setupStartTime;
  Serial.print("Setup knob helper took ");
  Serial.println(setupDuration);
}

void KnobHelperClass::loop() {
  readValues();
  sendValues();
}

void KnobHelperClass::sleep() {
}

void KnobHelperClass::readValues() {
  // Serial.println("Read knob");
  prevValue = value;
  pressed = false;

  //first lets handle rotary encoder button click
  if (rotary.currentButtonState() == BUT_RELEASED) {
    pressed = true;
    Serial.println("Button Pressed");
  }

  //lets see if anything changed
  //optionally we can ignore whenever there is no change
  int16_t encoderDelta = rotary.encoderChanged();
  if (encoderDelta == 0) return;

  //if value is changed compared to our last read
  if (encoderDelta != 0) {
    //now we need current value

    value = rotary.readEncoder();

    //process new value. Here is simple output.
    Serial.print("Value: ");
    Serial.println(value);
  }
}

void KnobHelperClass::sendValues() {
  if (value == prevValue && !pressed) return;
  if (host.length() == 0) return;
  if (!WiFiHelper.connect()) return;
  unsigned long requestStartTime = millis();

  StaticJsonDocument<32> doc;

  udp.beginPacket(host.c_str(), port);

  doc["value"] = value;
  doc["pressed"] = pressed;
  serializeJson(doc, udp);

  udp.println();
  udp.endPacket();

  requestDuration = millis() - requestStartTime;
  Serial.print("Send UDP request ");
  Serial.println(requestDuration);
}

void KnobHelperClass::server() {
  Serial.println("Setup Knob server");

  WebServerHelper.server.on("/api/knob", HTTP_GET, [this](AsyncWebServerRequest *request) {
    int args = request->args();

    if (args > 0) {
      request->send(200, "text/plain", "message received");
      Serial.println("Update knob settings");

      if (request->hasArg("host")) host = request->arg("host");
      if (request->hasArg("port")) port = request->arg("port").toInt();
      // if (request->hasArg("duration")) duration = request->arg("duration").toInt();
      // if (request->hasArg("threshold")) threshold = request->arg("threshold").toInt();

      write();

    } else {
      AsyncJsonResponse *response = new AsyncJsonResponse();
      response->addHeader("Server", "ESP Async Web Server");
      JsonVariant &root = response->getRoot();

      root["value"] = value;
      root["setupDuration"] = setupDuration;
      root["requestDuration"] = requestDuration;

      root["host"] = host;
      root["port"] = port;

      // root["duration"] = duration;
      // root["threshold"] = threshold;

      response->setLength();
      request->send(response);
    }
  });
}

KnobHelperClass KnobHelper;
