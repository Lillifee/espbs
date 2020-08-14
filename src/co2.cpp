#include "Arduino.h"
#include "BmeHelper.h"
#include "Co2Helper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

void setup() {
  setCpuFrequencyMhz(160);

  Serial.begin(115200);

  WiFiHelper.setup();
  WiFiHelper.server();

  Co2Helper.setup();
  Co2Helper.server();

  BmeHelper.setup();
  BmeHelper.server();

  WebServerHelper.start();
}

void loop() {
  Co2Helper.loop();
  BmeHelper.loop();
}
